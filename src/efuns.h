/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * efuns - All the exported functions
 *
 * $Id: efuns.h,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#ifndef AETOS_EFUNS_H
#define AETOS_EFUNS_H

#include "aetos.h"

/*
 * Create a script to autogenerate this file (and /mod/mod.h) from
 * a 'grep export *.c'
 */

/* External function table */
typedef struct efun_st *efun_tbl;
struct efun_st
{	void (*mod_initialize) (char *, int, int);
	void (*mod_exit) (void);
	void (*send_message) (int, char *, char *);
	gs_table (*get_gst) (void);
};

/* Import the functions which have to be exported to the modules */
// import void mod_initialize (char *, int, int);
// import void mod_exit (void);
import void irc_send_message (int, char *, char *);
import gs_table get_gst (void);

/* Fill the table */
struct efun_st _efun_table =
{	mod_initialize,
	mod_exit,
	irc_send_message,
	get_gst
};

#endif /* AETOS_EFUNS_H */
