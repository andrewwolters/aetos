/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * modules -- support for modules in aetos
 *
 * $Id: mod.h,v 1.2 2002/09/10 13:46:45 andrewwo Exp $
 */

#ifndef AETOS_MOD_H
#define AETOS_MOD_H

#include <stdio.h>

/* Module record */
typedef struct module_st *module_t;
struct module_st;

/* Callback record */
typedef struct callback_st *callback_t;
struct callback_st;

/* Event record */
typedef struct event_st *event_t;
struct event_st;

/* Global state table */
typedef struct gs_table_st *gs_table;
struct gs_table_st
{   char *botname;              /* Aetos nick name */
    char *botusername;          /* Registered username */
    char *botrealname;          /* Our Real name */
    char *servername;           /* Name server connected to */
    unsigned short serverport;      /* Port of irc server */
    int serversocket;           /* R/W socket */
};

typedef void (*callback_proc) (event_t);

#include "../src/eventtypes.h"
#include "../src/efuns.h"


#endif /* AETOS_MOD_H */
