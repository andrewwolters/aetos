/*
 * Bach module for aetos
 * $Id: bach.c,v 1.5 2004/09/21 13:16:59 semprini Exp $
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include "libpq-fe.h"

#define MOD_NAME bach

#include "../mod.h"

int last_cantata;
static efun_tbl efuns;
static int fd;
PGconn *db_conn;

int leap_year (int year)
{ return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

/* Returns the yday that Easter is, thanks to http://www.davros.org/misc/easter.html */
int easter (int year)
{
	int a = year % 19,
		b = year / 100,
		c = year % 100,
		d = b / 4,
		e = b % 4,
		f = c / 4,
		g = c % 4,
		h = (8 * b + 13) / 25,
		j = (19 * a + b - d - h + 15) % 30,
		m = (a + 11 * j) / 319,
		k = (2 * e + 2 * f - g - j + m + 32) % 7,
		mon = (j - m + k + 90) / 25,
		day = (j - m + k + 19 + mon) % 32;

	fprintf (stderr, "Year: %d, month: %d, day: %d\n", year, mon, day);

	if (mon == 3)
		return day + (leap_year (year) ? 59 : 58);
	else
		return day + (leap_year (year) ? 90 : 89);
}

void search_cantata (char *dest, int cantata)
{
	PGresult *res;
	char *query, *message;
	asprintf (&query, "SELECT box, cd FROM cantatas WHERE bwv = %d", cantata);
	res = PQexec (db_conn, query);
	if (PQresultStatus (res) != PGRES_TUPLES_OK)
		efuns -> send_message (fd, dest, PQerrorMessage (db_conn));
	asprintf (&message, "That cantata (BWV %d) is in Kruidvat box %s on CD %s.", cantata, PQgetvalue (res, 0, 0), PQgetvalue (res, 0, 1));
	efuns -> send_message (fd, dest, message);
	efuns -> tfree (query);
	efuns -> tfree (message);
	PQclear (res);
}

void today_cantata (char *dest, struct tm *t)
{
	int year = t -> tm_year + 1900, easter_d = easter (year);
	char *dayname, *query, *message;
	PGresult *res;

	fprintf (stderr, "yday: %d - easter_d: %d\n", t -> tm_yday, easter_d);
	if ((t -> tm_mon >= 12) && ((t -> tm_mday >= 25) && (t -> tm_mday <= 27)))
		asprintf (&dayname, "Am %d. Weihnachtstag", t -> tm_mday - 24);	
  else if ((t -> tm_mon == 0) && (t -> tm_mday == 1))
		asprintf (&dayname, "Am Neujahrstag");
	else if ((t -> tm_mon == 0) && (t -> tm_mday == 6))
		asprintf (&dayname, "Am %s Epiphanias", t -> tm_wday == 0 ? "Sonntag" : "Feste");
  else if ((t -> tm_mon == 1) && (t -> tm_mday == 2))
		asprintf (&dayname, "Am Feste Mariae Reinigung");
	else if ((t -> tm_mon == 2) && (t -> tm_mday == 25))
		asprintf (&dayname, "Zu Mariae Verkuendigung");
	else if ((t -> tm_mon == 5) && (t -> tm_mday == 24))
		asprintf (&dayname, "Am Feste Johannis des Taeufers");
	else if ((t -> tm_mon == 8) && (t -> tm_mday == 29))
		asprintf (&dayname, "Am Michaelisfest");
	else if ((t -> tm_mon == 9) && (t -> tm_mday == 31))
		asprintf (&dayname, "Am Reformationsfest");
	else if (t -> tm_yday - easter_d == 39)
		asprintf (&dayname, "Am Feste der Himmelfahrt Christi");
  /* Now for next sunday */
	else 
	{
		int yday = t -> tm_yday;
		if (t -> tm_wday != 0)
		{
			efuns -> send_message (fd, dest, "Hm, there seems to be no cantata for today. I'll give you the one for next Sunday.");
			yday += (7 - t -> tm_wday);
			fprintf (stderr, "yday of next Sunday: %d\n", yday);
		}
		if (yday >= (leap_year (year) ? 359 : 358))
			asprintf (&dayname, "Am Sonntag nach Weihnachten");
		else if (yday < 5)
			asprintf (&dayname, "Am Sonntag nach Neujahr");
		else if (yday >= (leap_year (year) ? 331 : 330))
			asprintf (&dayname, "Am %d. Advent", (yday - (leap_year (year) ? 324 : 323)) / 7);
		else if ((yday >= easter_d) && (yday <= easter_d + 2))
			asprintf (&dayname, "Am %d. Ostertag", (yday - easter_d + 1));
		else if (easter_d - yday == 63)
			asprintf (&dayname, "Am Sonntag Septuagesimae");
		else if (easter_d - yday == 56)
			asprintf (&dayname, "Am Sonntag Sexagesimae");
		else if (easter_d - yday == 49)
			asprintf (&dayname, "Am Sonntag Estomihi");
		else if (easter_d - yday == 42)
			asprintf (&dayname, "Am Sonntag Invocavit");
		else if (easter_d - yday == 35)
			asprintf (&dayname, "Am Sonntag Reminiscere");
		else if (easter_d - yday == 28)
			asprintf (&dayname, "Am Sonntag Oculi");
		else if (easter_d - yday == 21)
			asprintf (&dayname, "Am Sonntag Laetare");
		else if (easter_d - yday == 14)
			asprintf (&dayname, "Am Sonntag Judica");
		else if (easter_d - yday == 7)
			asprintf (&dayname, "Am Palmensonntag");
		else if (yday - easter_d == 7)
			asprintf (&dayname, "Am Sonntag Quasimodogeniti");
		else if (yday - easter_d == 14)
			asprintf (&dayname, "Am Sonntag Misericordias Domini");
		else if (yday - easter_d == 21)
			asprintf (&dayname, "Am Sonntag Jubilate");
		else if (yday - easter_d == 28)
			asprintf (&dayname, "Am Sonntag Cantate");
		else if (yday - easter_d == 35)
			asprintf (&dayname, "Am Sonntag Rogate");
		else if (yday - easter_d == 42)
			asprintf (&dayname, "Am Sonntag Exaudi");
		else if ((yday - easter_d >= 49) && (yday - easter_d <= 51))
			asprintf (&dayname, "Am %d. Pfingsttag ", yday - easter_d - 48);
		else if (yday - easter_d == 56)
			asprintf (&dayname, "Am Sonntag Trinitatis");
  	else if (yday < easter_d - 63)
			asprintf (&dayname, "Am %d. Sonntag nach Epiphanias", (yday - 5) / 7);
		else if (yday > easter_d + 56)
			asprintf (&dayname, "Am %d. Sonntag nach Trinitatis", (yday - easter_d - 56) / 7); 
	}
	fprintf (stderr, "Dayname: %s\n", dayname);
	asprintf (&query, "SELECT bwv, name FROM cantatas WHERE day = '%s'", dayname);
	res = PQexec (db_conn, query);
	if (PQresultStatus (res) != PGRES_TUPLES_OK)
	{
		efuns -> send_message (fd, dest, "Whoops!");
		fprintf (stderr, "bach: postgres error: %s\n", PQerrorMessage (db_conn));
	}
	else
	{
		if (PQntuples (res) > 0)
		{
			int i = random () % PQntuples (res);
			asprintf (&message, "Try '%s' (BWV %s)", PQgetvalue (res, i, 1), PQgetvalue (res, i, 0));
			last_cantata = atoi (PQgetvalue (res, i, 0));
			efuns -> send_message (fd, dest, message);
			efuns -> tfree (message);
		}
		else
			efuns -> send_message (fd, dest, "Sorry, no cantatas have survived for this day.");
	}
	efuns -> tfree (query);
	efuns -> tfree (dayname);
	PQclear (res);
}

static void do_privmsg (event_t event)
{
	ircmsg_evt *ircmsg;
	char *argv[2], *dest;
  time_t tl = time (NULL);
  struct tm *t = localtime (&tl);

	efuns -> get_from_event (event, (void **) &ircmsg);
	dest = efuns -> source_privmsg (*ircmsg);
	efuns -> tokenize_string (ircmsg -> param, argv, 3);
	if (efuns -> is_command (argv [1], "kruidvat"))
	{
		int cantata;
		if (argv [2] == NULL)
		{
			if (last_cantata == -1)
			{
				efuns -> send_message (fd, dest, "Sorry, what cantata do you mean?");
				return;
			}
			cantata = last_cantata;
		}
		else
			sscanf (argv [2], "%d", &cantata);	
		search_cantata (dest, cantata);
		return;
	}
  if (!(efuns -> is_command (argv [1], "cantata")))
    return;
	
	today_cantata (dest, t);
}

char *bach_init (efun_tbl table, int argc, char **argv)
{ gs_table gst; 
	efuns = table;
	gst = efuns -> get_gst ();
	fd = gst -> serversocket;
	last_cantata = -1;
	efuns -> mod_initialize ("bach", 1, 0);
	efuns -> add_callback (EvtPrivmsgMask, do_privmsg);
	efuns -> send_message (fd, gst -> channelname, "Jauchzet, frohlocket!");
  db_conn = PQconnectdb ("host=postgres.kerguelen.org dbname=bach user=aetos "
	                       "password=aetos");
  if (PQstatus (db_conn) != CONNECTION_OK)
	{ char *errmsg;
	  asprintf (&errmsg, "Database error: %s", PQerrorMessage (db_conn));
		efuns -> send_message (fd, gst -> channelname, errmsg);
		free (errmsg);
		return "Ruhe sanfte, sanfte ruh";
	}
	srandom (time (NULL));
	efuns -> mod_mainloop ();
	return NULL;
}
