/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * event -- event handling subsystem
 *
 * $Id: event.c,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#include "aetos.h"

#include <irc/irc.h>
#include <csp/csp.h>
#include <csp/sdeque.h>
#include <csp/slist.h>
#include <pth.h>

#include "mem.h"
#include "event.h"
#include "error.h"
#include "system.h"


/* A lost extern */
extern int mod_self_id(void);
extern pth_key_t callback_key;

/* Setup event queue and event_mask_map */
struct queue_st
{   SLIST_LINK(queue_st);
    int id;
    sdeque_t(eventq_st, event_st) evq;
    event_mask mask;
};

slist_t(qlist_st, queue_st) *quelist;
slist_t(callback_list, callback_st);

struct event_mask_map
{	event_mask mask;
	event_type type;
	char *param;
} mapping[] =
{   {	EvtNilMask, EvtNil, NIL },
	{   EvtMessageMask, EvtIRCMsg, NIL },
    {   EvtPongMask, EvtIRCMsg, "PING" },
    {   EvtPrivmsgMask, EvtIRCMsg, "PRIVMSG" },
    {   EvtNoticeMask, EvtIRCMsg, "NOTICE" },
    {   EvtJoinMask, EvtIRCMsg, "JOIN" },
    {   EvtTimeoutMask, EvtTimeout, NIL },
    {   0, 0, NIL }
};

/* Initialize event subsystem */
intern int init_events ()
{	quelist = tmalloc(sizeof(struct qlist_st));
	slist_instantiate(quelist);
	return 1;
}

/* Check if the mask includes the type */
private int event_mask_match(event_mask mask, event_t ev)
{	int i;
	for (i = 0; mapping[i].mask; i++)
	{	if ((checkflag(mask, mapping[i].mask)) && (mapping[i].type == ev -> type))
		{	if (ev -> type == EvtIRCMsg)
				return ( (strncmp(mapping[i].param, ev -> e.ircmsg.cmd, 10) == 0) );
			else
				return TRUE;
		}
	}
	return FALSE;
}

/* Create an event */
intern event_t new_event (event_type type, ...)
{	va_list ap;
	struct event_st *ret;
	va_start (ap, type);
	ret = tmalloc(sizeof(struct event_st));
	ret -> type = type;
	switch (type)
	{	case EvtNil:
			ret -> e.nil.nil = NULL;
			break;
		case EvtIRCMsg:
			ret -> e.ircmsg.pre = ckstrdup(va_arg(ap, char *));
			ret -> e.ircmsg.cmd = ckstrdup(va_arg(ap, char *));
			ret -> e.ircmsg.param = ckstrdup(va_arg(ap, char *));
			break;
		case EvtTimeout:
			ret -> e.timeout.interval = va_arg(ap, unsigned long);
			break;
		default:
			ret -> type = EvtNil;
		/* FALLTHROUGH */
	}
	return (event_t) ret;
}

/* Copy an event */
intern event_t copy_event (event_t event)
{	struct event_st *ret;
	ret = tmalloc(sizeof(struct event_st));
	ret -> type = event -> type;
	switch (event -> type)
	{	case EvtNil:
			break;
		case EvtIRCMsg:
			ret -> e.ircmsg.pre = ckstrdup(event -> e.ircmsg.pre);
			ret -> e.ircmsg.cmd = ckstrdup(event -> e.ircmsg.cmd);
			ret -> e.ircmsg.param = ckstrdup(event -> e.ircmsg.param);
			break;
		case EvtTimeout:
			ret -> e.timeout.interval = event -> e.timeout.interval;
			break;
	}
	return ((event_t) ret);
}

/* Destroy an event */
intern void destroy_event (event_t event)
{	switch (event -> type)
	{	case EvtNil:
			break;
		case EvtIRCMsg:
			tfree(event -> e.ircmsg.pre);
			tfree(event -> e.ircmsg.cmd);
			tfree(event -> e.ircmsg.param);
		case EvtTimeout:
			break;
	}
	tfree(event);
}

/* Add an event to each module's event queue */
intern void add_event (event_t event)
{	struct queue_st *queue;
	event_t ev;
	slist_foreach(quelist, queue)
	{	if (event_mask_match(queue -> mask, event))
		{	ev = copy_event (event);
			sdeque_insert_tail(&queue -> evq, ev);
		}
	}
	destroy_event (event);
}

/* Retrieve next event from event queue from this module */
intern void next_event (event_t *event)
{	struct queue_st *queue;
	struct eventq_st *evq;
	int id;
	id = mod_self_id();
	slist_foreach(quelist, queue)
	{	if (queue -> id == id)
		{	evq = &queue -> evq;
			*event = sdeque_first(evq);
			sdeque_remove_head(evq);
		}
	}
}

/* Dispatch event - call all callbacks for this event on this module */
intern void dispatch_event(event_t event)
{	struct callback_list *cblist;
	callback_t cb;
	cblist = pth_key_getdata(callback_key);
	slist_foreach(cblist, cb)
	{	if (event -> type & cb -> mask) /* Mask includes event -> type */
			(*cb -> callback) (event); /* call callback */
	}
}

/* Add a callback routine to certain types of events */
intern void add_callback (event_mask mask, callback_proc proc)
{	struct callback_list *cblist;
	callback_t cb;
	if ((cblist = pth_key_getdata(callback_key)) == NULL) fatal("akjsdhfjaksdhf", 1);
	cb = tmalloc(sizeof(struct callback_st));
	cb -> callback = proc;
	cb -> mask = mask;
	slist_insert_head(cblist, cb);
}


/* Remove a callback routine for certain types of events */
intern void remove_callback (event_mask mask, callback_proc proc)
{	struct callback_list *cblist;
	callback_t cb;
	cblist = pth_key_getdata(callback_key);
	slist_foreach(cblist, cb)
	{	if (cb -> callback == proc)
			slist_remove(cblist, cb, callback_st);
	}
}

