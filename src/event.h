/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * event -- event handling subsystem
 *
 * $Id: event.h,v 1.2 2002/09/10 13:40:46 andrewwo Exp $
 */

#ifndef AETOS_EVENT_H
#define AETOS_EVENT_H

#include <csp/sdeque.h>
#include <csp/slist.h>
#include "common.h"
#include "eventtypes.h"


/* Event record */
typedef struct event_st *event_t;
struct event_st
{	SDEQUE_LINK(event_st);
	event_type type;
	union
	{	nil_evt nil;
		ircmsg_evt ircmsg;
		timeout_evt timeout;
	} e;
} event_st;

/* Function typedefs */
typedef void (*callback_proc) (event_t);

/* Callback record */
typedef struct callback_st *callback_t;
struct callback_st
{   SLIST_LINK(callback_st);
    callback_proc callback;
    event_mask mask;
};

/* Event administration */
int init_events (void);
void init_callback_list (void);
void init_event_queue (void);
event_t new_event (event_type type, ...);
event_t copy_event (event_t event);
void destroy_event (event_t event);
void add_event (event_t event);
void next_event (event_t *event);
void dispatch_event (event_t event);
void get_from_event (event_t event, void **arg);

/* Routines for callbacks */
void add_callback (event_mask mask, callback_proc proc);
void remove_callback (event_mask mask, callback_proc proc);


#endif /* AETOS_EVENT_H */
