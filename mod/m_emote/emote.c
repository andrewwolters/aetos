/*
 * A "social" module for Aetos 
 * $Id: emote.c,v 1.4 2004/09/21 15:29:50 semprini Exp $
 */

#include <stdlib.h>
#include <string.h>
#include "libpq-fe.h"

#define MOD_NAME hello

#include "../mod.h"

static efun_tbl efuns;
static int fd;
PGconn *db_conn;
gs_table gst; 

void emote_send (char *dest, char *msg_template, char *first, char *second)
{
	char *first_s, *msg;
	if (strlen (msg_template) == 0)
		return;
	if ((first_s = strstr (msg_template, "%s")) != NULL)
	{
		if (strstr (first_s, "%s") != NULL)
			asprintf (&msg, msg_template, first == NULL ? "Henk de Frits" : first,
			                              second == NULL ? "Bloitbeard" : second);
		else
			asprintf (&msg, msg_template, first == NULL ? "Henk de Frits" : first);
		efuns -> send_message (fd, dest, msg);
		efuns -> tfree (msg);
	}
	else
	{
		efuns -> send_message (fd, dest, msg_template);
	}
}

static void do_privmsg(event_t event)
{	ircmsg_evt *ircmsg;
	char *argv[3];
	char *cmd, *dest, *query;
	char *source, *target;
	PGresult *res;

	efuns -> get_from_event(event, (void **) &ircmsg);
	dest = efuns -> source_privmsg(*ircmsg);
	efuns -> tokenize_string(ircmsg -> param, argv, 3);

	cmd = (char *) malloc (strlen (argv [1]) + 1);
	if (sscanf (argv [1], ".%s", cmd) != 1)
	{
		efuns -> tfree (cmd);
		return;
	}
	source = (char *) malloc (strlen (ircmsg -> pre) + 1);
	if (sscanf (ircmsg -> pre, "%[^!]!%*s", source) != 1)
	{
		efuns -> tfree (source);
		efuns -> tfree (cmd);
		return;
	}
	target = argv [2] == NULL ? NULL : efuns -> duplicate_string (argv [2]);

	asprintf (&query, "SELECT target_message, source_message, other_message, "
	                  "refl_source_message, refl_other_message, "
										"bot_source_message, bot_other_message, source_first "
	                  "FROM emote WHERE command = '%s' AND has_parameter = %s",
										cmd, target == NULL ? "false" : "true");

	res = PQexec (db_conn, query);
	if (PQresultStatus (res) != PGRES_TUPLES_OK)
	{
		char *errmsg;
		asprintf (&errmsg, "Database error: %s", PQresultErrorMessage (res));
		efuns -> send_message (fd, dest, errmsg);
		PQclear (res);
		efuns -> tfree (target);
		efuns -> tfree (source);
		efuns -> tfree (errmsg);
		efuns -> tfree (cmd);
		efuns -> tfree (query);
		return;
	}
	if (PQntuples (res) > 0)
	{
		if (target == NULL)
		{
			emote_send (source, PQgetvalue (res, 0, 1), source, target);
			emote_send (gst -> channelname, PQgetvalue (res, 0, 2), source, target);
		}
		else
		{
			int i, target_exists = 0;
			for (i = 0; i < gst -> nr_names; i++)
				if (strcasecmp (gst -> names [i], target) == 0)
					target_exists = -1;
			if (!target_exists)
				efuns -> send_message (fd, source, "I can't find any person of that name!");	
			else if (strcasecmp (source, target) == 0)
			{
				emote_send (source, PQgetvalue (res, 0, 3), source, target);
				emote_send (gst -> channelname, PQgetvalue (res, 0, 4), source, target);
			}
			else if (strcasecmp (target, gst -> botname) == 0)
			{
				emote_send (source, PQgetvalue (res, 0, 5), source, target);
				emote_send (gst -> channelname, PQgetvalue (res, 0, 6), source, target);
			}
			else
			{
				emote_send (source, PQgetvalue (res, 0, 1), target, source);
				emote_send (target, PQgetvalue (res, 0, 0), source, target);
				if (PQgetvalue (res, 0, 7) [0] == 't')
					emote_send (gst -> channelname, PQgetvalue (res, 0, 2), source, target);
				else
					emote_send (gst -> channelname, PQgetvalue (res, 0, 2), target, source);
			}
		}
	}
	PQclear (res);
	efuns -> tfree (target);
	efuns -> tfree (source);
	efuns -> tfree (cmd);
	efuns -> tfree (query);
}

void *emote_init (efun_tbl tbl, int argc, char **argv)
{
	efuns = tbl;
	gst = efuns -> get_gst();
	fd = gst -> serversocket;

	efuns -> mod_initialize ("emote", 1, 0);
	efuns -> add_callback (EvtPrivmsgMask, do_privmsg);
	efuns -> send_message (fd, gst -> channelname, "Emotions are go green, repeat, go green.");

	db_conn = PQconnectdb ("host=postgres.kerguelen.org dbname=aetos user=aetos "
	                       "password=aetos");
  if (PQstatus (db_conn) != CONNECTION_OK)
  { char *errmsg;
    asprintf (&errmsg, "Database error: %s", PQerrorMessage (db_conn));
    efuns -> send_message (fd, gst -> channelname, errmsg);
    efuns -> tfree (errmsg);
    return "Error";
	}

	efuns -> mod_mainloop();
	return NULL;
}
