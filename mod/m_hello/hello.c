/*
 * A test "hello world" module for Aetos
 * $Id: hello.c,v 1.3 2002/09/14 23:14:03 semprini Exp $
 */

#define MOD_NAME hello

#include "../mod.h"

static efun_tbl efuns;
static int fd;

static void do_privmsg(event_t event)
{	ircmsg_evt *ircmsg;
	char *argv[2];
	char *dest;

	efuns -> get_from_event(event, (void **) &ircmsg);
	dest = efuns -> source_privmsg(*ircmsg);
	efuns -> tokenize_string(ircmsg -> param, argv, 2);
	if (efuns -> is_command(argv[1], "hello"))
		efuns -> send_message (fd, dest, "Hello World!");
}

void *hello_init (efun_tbl tbl, int argc, char **argv)
{	gs_table gst; 
	efuns = tbl;
	gst = efuns -> get_gst();
	fd = gst -> serversocket;

	efuns -> mod_initialize ("hello", 0, 1);
	efuns -> add_callback (EvtPrivmsgMask, do_privmsg);
	efuns -> send_message (fd, gst -> channelname, "Hello World!");

	efuns -> mod_mainloop();
	return NULL;
}
