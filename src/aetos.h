/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * Miscellaneous standard defines, must always be included
 *
 * $Id: aetos.h,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#ifndef AETOS_AETOS_H
#define AETOS_AETOS_H

#include "common.h"

/* Global state table */
typedef struct gs_table_st *gs_table;
struct gs_table_st
{	char *botname;				/* Aetos nick name */
	char *botusername;			/* Registered username */
	char *botrealname;			/* Our Real name */
	char *servername;			/* Name server connected to */
	unsigned short serverport;		/* Port of irc server */
	int serversocket;			/* R/W socket */
};

#define AETOS	(_aetos_gst)
extern gs_table _aetos_gst;

#endif /* AETOS_AETOS_H */
