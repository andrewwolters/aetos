/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * modules -- support for modules in aetos
 *
 * $Id: mod.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#ifndef AETOS_MOD_H
#define AETOS_MOD_H

/* Module record */
typedef struct module_st *module_t;
struct module_st;

/* Callback record */
typedef struct callback_st *callback_t;
struct callback_st;

/* Global state table */
typedef struct gs_table_st *gs_table;
struct gs_table_st
{	char *botname;				/* Aetos nick name */
	char *botusername;			/* Registered username */
	char *botrealname;			/* Our Real name */
	char *servername;			/* Name server connected to */
	unsigned short serverport;	/* Port of irc server */
	int serversocket;			/* R/W socket */
};

/* External function table */
typedef struct efun_st *efun_tbl;
struct efun_st
{	void (*mod_initialize) (char *, int, int);
	void (*mod_exit) (void);
	void (*send_message) (int, char *, char *);
	gs_table (*get_gst) (void);
};

#endif /* AETOS_MOD_H */
