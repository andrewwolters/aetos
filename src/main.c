/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * Application top level section
 * $Id: main.c,v 1.7 2004/09/21 15:19:22 semprini Exp $
 */

#include "common.h"
#include "aetos.h"

#include <csp/csp.h>
#include <irc/irc.h>
#include <irc/socket.h>
#include <pth.h>

// #include "resources.h"
#include "mem.h"
#include "event.h"
#include "modules.h"
#include "options.h"
#include "utility.h"
#include "error.h"

/* Global declarations */
gs_table _aetos_gst;

/* Callbacks and helper functions */
private void do_pong (event_t e)
{	notice ("Ping received, ponging...");
	irc_pong (AETOS->serversocket, e->e.ircmsg.param);
}

private void do_cmd (event_t e)
{	char *argv[8];
	char *dest;
	char **args = { NULL };
	int mod_id;

	dest = source_privmsg(e -> e.ircmsg);
	// argv = tcalloc(8, sizeof(char *));
	tokenize_string(e -> e.ircmsg.param, argv, 8);
	if (is_command(argv[1], "quit"))
	{	notice ("Receiving quit command");
		irc_send_message (AETOS->serversocket, dest, "**poefzZz**");
		irc_quit (AETOS->serversocket, "Goodbye, SUCKAHS!");
	} else if (is_command(argv[1], "load"))
	{	if (argv[2])
			mod_load (argv[2], 0, args);
		else
			irc_send_message (AETOS->serversocket, dest, "[CORE] No module specified");
	} else if (is_command(argv[1], "unload"))
	{	if (argv[2])
		{	mod_id = atoi(argv[2]);
			/* first check if module id exists, then unload */
			if (mod_id == 1)
			{	irc_send_message (AETOS->serversocket, dest, "[CORE] Cannot unload the core");
			} else if ((module_t) mod_search(mod_id))
			{	mod_unload (mod_id);
				irc_send_message (AETOS->serversocket, dest, "[CORE] Module unloaded");
			} else
			{	irc_send_message (AETOS->serversocket, dest, "[CORE] Please give the id of a loaded module");
			}
		} else
		{	irc_send_message (AETOS->serversocket, dest, "[CORE] No module specified");
		}
	} else if (is_command(argv[1], "lsmod"))
	{	mod_listing (dest);
	} else if (is_command(argv[1], "help"))
	{	irc_send_message (AETOS->serversocket, dest, "[CORE] Help:");
		irc_send_message (AETOS->serversocket, dest, "[CORE] .quit   .load [mod]   .unload [mod]");
	}
	tfree(*argv);
	tfree(dest);
}

private void do_join (event_t e)
{
	ircmsg_evt *ircmsg;
	char *source;

  get_from_event(e, (void **) &ircmsg);
	source = (char *) malloc (strlen (ircmsg -> pre));
	sscanf (ircmsg -> pre, "%[^!]!%*s", source);

	if (strcmp (source, AETOS -> botname) != 0)
	{
		AETOS -> names = (char **) realloc (AETOS -> names,
			++(AETOS -> nr_names) * sizeof (char *));
		AETOS -> names [AETOS -> nr_names - 1] = duplicate_string (source); 
	}

	tfree (source);
}

private void do_leave (event_t e)
{
	ircmsg_evt *ircmsg;
	char *source;

  get_from_event(e, (void **) &ircmsg);
	source = (char *) malloc (strlen (ircmsg -> pre));
	sscanf (ircmsg -> pre, "%[^!]!%*s", source);

	fprintf (stderr, "Part: %s\n", source);

	if (strcmp (source, AETOS -> botname) != 0)
	{
		int i;
		for (i = 0; i < AETOS -> nr_names; i++)
		{
			fprintf (stderr, "Parting, name %d is %s and ", i, AETOS -> names [i]);
			if (strcmp (source, AETOS -> names [i]) == 0)
			{
				fprintf (stderr, "it\n");
				strcpy (AETOS -> names [i], AETOS -> names [AETOS -> nr_names - 1]);
				tfree (AETOS -> names [AETOS -> nr_names - 1]);
				AETOS -> names = (char **) realloc (AETOS -> names,
					--(AETOS -> nr_names) * sizeof (char *));
			}
			else
				fprintf (stderr, "not it\n");
		}
	}

	tfree (source);
}

private void do_message (event_t e)
{
	ircmsg_evt *ircmsg;

  get_from_event(e, (void **) &ircmsg);
	/* NAMES list: 353 */
	if (strcmp (ircmsg -> cmd, "353") == 0)
	{
		/* Find the first colon thst's where the names start */
		char *c = strchr (ircmsg -> param, ':') + 1,
			*tmp = (char *) malloc (strlen (ircmsg -> param));		
		int i = 0, j;
		while (i < strlen (c))
		{
			/* Operator */
			if (c [i] == '@')
				i++;
			/* No clue what this means */
			else if (c [i] == '+')
				i++;
			for (j = i; c [j] != ' '; j++)
				tmp [j - i] = c [j];	
			tmp [j - i] = '\0';
			AETOS -> names = (char **) realloc (AETOS -> names,
				++(AETOS -> nr_names) * sizeof (char *));
			AETOS -> names [AETOS -> nr_names - 1] = duplicate_string (tmp);
			i = j + 1;
		}
		tfree (tmp);
	}
	/* End of NAMES list: 366 */
	else if (strcmp (ircmsg -> cmd, "366") == 0)
	{
		int i;
		for (i = 0; i < AETOS -> nr_names; i++)
			fprintf (stderr, "Name %d: %s\n", i + 1, AETOS -> names [i]);
	}
}

/* Fill the global state table */
private void setup_gst(gs_table *t)
{	*t = tmalloc(sizeof(struct gs_table_st));
	(*t) -> botname = duplicate_string(DEFAULT_NICK);
	(*t) -> botusername = duplicate_string(DEFAULT_USER);
	(*t) -> botrealname = duplicate_string(DEFAULT_REAL);
	(*t) -> servername = duplicate_string(DEFAULT_SERVER);
	(*t) -> channelname = duplicate_string(DEFAULT_CHANNEL);
	(*t) -> serverport = 6667;
	(*t) -> names = NULL;
	(*t) -> nr_names = 0;
}

/* A module must be able to grab the state table */
export gs_table get_gst(void)
{	return _aetos_gst;
}

/* Main event loop of the irc bot */
private void aetos_main_loop ()
{	message_rec *reply;
	event_t event;

	while (TRUE)
	{	reply = irc_get_message (AETOS->serversocket);
		event = new_event (EvtIRCMsg, reply->prefix, reply->command, reply->params);
		add_event(event);

		/* Process the event in the core */
		dispatch_event(event);
		destroy_event(event);
        free_message_rec (reply);
		pth_yield(NULL);
	}
}

/* And here is the start of life */
int main (int argc, char **argv)
{	pth_attr_t attr;
	pth_t self;
	char *rcfile __attribute__((unused));
	int fd; 

	/* Get commandline options and read configuration file */
	/*
	if (! (options -> rcfile))
		rcfile = DEFAULT_RCFILE;
	else
		rcfile = options -> rcfile;
	hash_resource_file (rcfile);
	*/
	setup_gst (&AETOS);
	get_options (argc, argv, &AETOS);
 	
	/* Initalize Aetos: start internal services (order is important!) */
	pth_init();
	init_events();
	init_modules();
	add_callback (EvtPrivmsgMask, do_cmd);
	add_callback (EvtPongMask, do_pong);
	add_callback (EvtJoinMask, do_join);
	add_callback (EvtMessageMask, do_message);
	add_callback (EvtPartMask, do_leave);
	add_callback (EvtQuitMask, do_leave);

	/* Connect to server and do irc handshaking */
	if ((fd = open_connection (AETOS->servername, AETOS->serverport)) < 0)
		pth_exit(0);
	AETOS -> serversocket = fd;
	irc_connect (AETOS->serversocket, AETOS->botname, AETOS->botusername, 0,
		AETOS->botrealname);
	notice("IRC handshake complete, connected to server");
	irc_join (AETOS->serversocket, AETOS->channelname);
	notice("IRC joined channel");

	/* Enter application loop */
	self = pth_self();
	attr = pth_attr_of(self);
	pth_attr_set(attr, PTH_ATTR_PRIO, PTH_PRIO_STD + 1);
	pth_attr_set(attr, PTH_ATTR_NAME, "core");
	aetos_main_loop ();
	
	pth_exit(0);

	/* Stop compiler from complaining (fix: attributes) */
	exit(0); /* NOTREACHED */
}
