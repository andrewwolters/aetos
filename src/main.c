/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * Application top level section
 * $Id: main.c,v 1.2 2002/08/31 13:40:17 andrewwo Exp $
 */

#include "common.h"
#include "aetos.h"

#include <csp/csp.h>
#include <irc/irc.h>
#include <irc/socket.h>
#include <pth.h>

// #include "resources.h"
#include "options.h"
#include "event.h"
#include "modules.h"
#include "mem.h"
#include "system.h"
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
	char dest[20];

	notice("Doing do_cmd");
/*	source_privmsg(e -> privmsg, &dest);
	tokenize_string(e -> privmsg.mesg, &argv, 8);
	if (is_command(argv[1], "quit"))
	{	notice ("Receiving quit command");
		irc_send_message (AETOS->serversocket, dest, "**poefzZz**");
		irc_quit (AETOS->serversocket, "Goodbye, SUCKAHS!");
	} else if (is_command(argv[1], "load"))
	{	if (argv[2])
			mod_load (argv[2], 0, args);
		else
			irc_send_message (AETOS->serversocket, dest,
				"[CORE] No module specified");
	} else if (is_command(argv[1], "unload"))
	{	if (argv[2])
			mod_unload (argv[2]);
		else
			irc_send_message (AETOS->serversocket, dest,
				"[CORE] No module specified");
	} else if (is_command(argv[1], "help"))
	{	irc_send_message (AETOS->serversocket, dest, "[CORE] Help:");
		irc_send_message (AETOS->serversocket, dest, "[CORE] .quit   .load [mod]   .unload [mod]");
	} */
}

/*
 * TODO: Aestheticalize me!
 */
private void setup_gst(gs_table *t)
{	*t = tmalloc(sizeof(struct gs_table_st));
	(*t) -> botname = ckstrdup(DEFAULT_NICK);
	(*t) -> botusername = ckstrdup(DEFAULT_USER);
	(*t) -> botrealname = ckstrdup(DEFAULT_REAL);
	(*t) -> servername = ckstrdup(DEFAULT_SERVER);
	(*t) -> serverport = 6667;
}

export gs_table get_gst(void)
{	return _aetos_gst;
}

private void aetos_main_loop ()
{	message_rec *reply;
	event_t event;

//	mod_load("hello");
//	mod_load("bye");

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
{	options_rec *options;
	pth_attr_t attr;
	pth_t self;
	char *rcfile;
	int fd; 

	/* Get commandline options and read configuration file */
	options = get_options (argc, argv);
	/*
	if (! (options -> rcfile))
		rcfile = DEFAULT_RCFILE;
	else
		rcfile = options -> rcfile;
	hash_resource_file (rcfile);
	*/
	setup_gst (&AETOS);
 	
	/* Initalize Aetos: start internal services */
	pth_init();
	init_modules();
	init_events();
	add_callback (EvtPrivmsgMask, do_cmd);
	add_callback (EvtPongMask, do_pong);

	/* Connect to server and do irc handshaking */
	if ((fd = open_connection (AETOS->servername, AETOS->serverport)) < 0)
		pth_exit(0);
	AETOS -> serversocket = fd;
	irc_connect (AETOS->serversocket, AETOS->botname, AETOS->botusername, 0,
		AETOS->botrealname);
	notice("IRC handshake complete, connected to server");
	irc_join (AETOS->serversocket, DEFAULT_CHANNEL);
	notice("IRC joined channel");

	/* Enter application loop */
	self = pth_self();
	attr = pth_attr_of(self);
	pth_attr_set(attr, PTH_ATTR_PRIO, PTH_PRIO_STD + 1);
	pth_attr_set(attr, PTH_ATTR_NAME, "core");
	aetos_main_loop ();
	
	pth_exit(0);
	exit(0); // Never reached, but stops compiler from complaining (fix: attributes)
}
