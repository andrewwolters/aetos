/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * event -- event handling subsystem
 *
 * $Id: event.h,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#ifndef AETOS_EVENT_H
#define AETOS_EVENT_H

#include <csp/sdeque.h>
#include <csp/slist.h>
#include "common.h"


/* Event thingies */
typedef long int event_type;
enum
{	EvtNil = 0,				/* The 'nothing' or empty event */
	EvtIRCMsg,				/* IRC Message */
	EvtTimeout,				/* Timeout event */
	EvtRehash,				/* Resource rehash */
	EvtNumEvents = 32		/* Number of events */
};

/* Event masks */
typedef long int event_mask;
#define	EvtNilMask			bit(0)
#define	EvtPongMask			bit(1)
#define	EvtMessageMask		bit(2)
#define	EvtPrivmsgMask		bit(3)
#define	EvtNoticeMask		bit(4)
#define	EvtJoinMask			bit(5)
#define	EvtTimeoutMask		bit(6)

/* Event types */
typedef struct nil_evt
{	void *nil;
} nil_evt;

typedef struct ircmsg_evt
{	char *pre;
	char *cmd;
	char *param;
} ircmsg_evt;

typedef struct timeout_evt
{	unsigned long interval;
} timeout_evt;

/* Event record - event_t must be treated as an opaque type */
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
int init_events PROTO((void));
event_t new_event PROTO((event_type, ...));
event_t copy_event PROTO((event_t event));
void destroy_event PROTO((event_t event));
void add_event PROTO((event_t event));
void next_event PROTO((event_t *event));
void dispatch_event PROTO((event_t event));

/* Routines for callbacks */
void add_callback (event_mask mask, callback_proc proc);
void remove_callback (event_mask mask, callback_proc proc);


#endif /* AETOS_EVENT_H */
