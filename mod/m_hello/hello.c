/*
 * module for aetos
 * test
 */

#include <stdio.h>
#include <pth.h>

#include "../mod.h"

static efun_tbl efuns;

void *hello_init (efun_tbl tbl, int argc, char **argv)
{	gs_table gst; 
	int fd;
	efuns = tbl;
	gst = efuns -> get_gst();
	fd = gst -> serversocket;

	efuns -> mod_initialize ("hello", 0, 1);
	fprintf(stderr, "Hello World Loaded & Registered");
	// efuns -> add_callback ("m_hello", EvtPrivmsgMask, do_privmsg);
	efuns -> send_message (fd, "#aetos", "Goooodmorning!");

	while(1)
	{	efuns -> send_message (fd, "#aetos", "Hello World!");
		pth_sleep(1);
	}
	
	efuns -> mod_exit();
}
