/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * event -- event handling subsystem
 *
 * $Id: event.c,v 1.5 2004/09/21 15:19:22 semprini Exp $
 */

#include "aetos.h"

#include <irc/irc.h>
#include <csp/csp.h>
#include <csp/sdeque.h>
#include <csp/slist.h>
#include <pth.h>

#include "event.h"
#include "utility.h"
#include "error.h"
#include "mem.h"


/* Define queue structure, event_mask_map and callback structure */
struct queue_st
{   SLIST_LINK(queue_st);
    event_mask mask;
    sdeque_t(event_queue_st, event_st) evq;
};

struct event_mask_map
{	event_mask mask;
	event_type type;
	char *param;
};

slist_t(callback_list, callback_st);

/* Setup queue list and event/mask mapping */
slist_t(queue_list_st, queue_st) *que_list;

struct event_mask_map mapping[] =
{   {	EvtNilMask,			EvtNil,			NULL },
	{   EvtMessageMask,		EvtIRCMsg,		NULL },
    {   EvtPongMask,		EvtIRCMsg,		"PING" },
    {   EvtPrivmsgMask,		EvtIRCMsg,		"PRIVMSG" },
    {   EvtNoticeMask,		EvtIRCMsg,		"NOTICE" },
    {   EvtJoinMask,		EvtIRCMsg,		"JOIN" },
    {   EvtTimeoutMask,		EvtTimeout,		NULL },
		{		EvtPartMask,		EvtIRCMsg,		"PART" },
		{		EvtQuitMask,		EvtIRCMsg,		"QUIT" },
    {   0,					0,				NULL }
};

/* Keys for thread local data */
private pth_key_t callback_key = PTH_KEY_INIT;
private pth_key_t queue_key = PTH_KEY_INIT;

/* Destroy thread local callback list */
private void destroy_callback_list(void *arg)
{   struct callback_list *cb_list = (struct callback_list *) arg;
    slist_empty(cb_list, callback_st, tfree);
    tfree(cb_list);
}

/* Destroy thread local event queue */
private void destroy_event_queue(void *arg)
{	struct queue_st *queue = (struct queue_st *) arg;
	sdeque_empty(&queue -> evq, event_st, tfree);
	slist_remove(que_list, queue, queue_st);
	tfree(queue);
}

/* Initialize event subsystem */
intern int init_events ()
{	pth_key_create(&callback_key, destroy_callback_list);
	pth_key_create(&queue_key, destroy_event_queue);
	que_list = tmalloc(sizeof(struct queue_list_st));
	slist_instantiate(que_list);
	return 1;
}

/* Check if the mask includes the type */
private int event_mask_match(event_mask mask, event_t ev)
{	int i;
	for (i = 0; mapping[i].mask; i++)
	{	if ((mapping[i].mask == (mapping[i].mask & mask))
			&& (ev -> type == mapping[i].type))
		{	if (mapping[i].param)
			{	if (ev -> type == EvtIRCMsg)
				{	if ( (strcmp(mapping[i].param, ev -> e.ircmsg.cmd) == 0) )
						return TRUE;
					else
						return FALSE;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

/* Add an event queue to the queue list and to the thread local data */
intern void init_event_queue()
{	struct queue_st *queue;
	queue = tmalloc(sizeof(struct queue_st));
	queue -> mask = EvtNilMask;
	sdeque_instantiate(&queue -> evq);
	slist_insert_head(que_list, queue);
	if ((struct queue_st *) pth_key_getdata(queue_key) != NULL)
		fatal("Thread already has an event queue??", 1);
	pth_key_setdata(queue_key, (const void *) queue);
}

/* Add a callback list as thread local data */
intern void init_callback_list()
{	struct callback_list *cb_list;
	if ((struct callback_list *) pth_key_getdata(callback_key) != NULL)
		fatal("Thread already has a callback list??", 1);
	cb_list = tmalloc(sizeof(struct callback_list));
	slist_instantiate(cb_list);
	pth_key_setdata(callback_key, (const void *) cb_list);
}
		
/* Create an event */
intern event_t new_event (event_type type, ...)
{	struct event_st *ret;
	va_list ap;
	va_start (ap, type);
	ret = tmalloc(sizeof(struct event_st));
	ret -> type = type;
	switch (type)
	{	case EvtNil:
			ret -> e.nil.nil = NULL;
			break;
		case EvtIRCMsg:
			ret -> e.ircmsg.pre = duplicate_string(va_arg(ap, char *));
			ret -> e.ircmsg.cmd = duplicate_string(va_arg(ap, char *));
			ret -> e.ircmsg.param = duplicate_string(va_arg(ap, char *));
			break;
		case EvtTimeout:
			ret -> e.timeout.interval = va_arg(ap, unsigned long);
			break;
		default:
			ret -> type = EvtNil;
			/* FALLTHROUGH */
	}
	return ((event_t) ret);
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
			ret -> e.ircmsg.pre = duplicate_string(event -> e.ircmsg.pre);
			ret -> e.ircmsg.cmd = duplicate_string(event -> e.ircmsg.cmd);
			ret -> e.ircmsg.param = duplicate_string(event -> e.ircmsg.param);
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

/* Get the contents of an event_t */
export void get_from_event (event_t event, void **arg)
{	switch (event -> type)
	{	case EvtNil:
			break;
		case EvtIRCMsg:
			(ircmsg_evt *) *arg = &(event -> e.ircmsg);
			break;
		case EvtTimeout:
			(timeout_evt *) *arg = &(event -> e.timeout);
			break;
	}
}

/* Add an event to each module's event queue */
intern void add_event (event_t event)
{	struct queue_st *queue;
	event_t ev;
	slist_foreach(que_list, queue)
	{	if (event_mask_match(queue -> mask, event))
		{	ev = copy_event (event);
			sdeque_insert_tail(&queue -> evq, ev);
			// pth_resume(mod -> thread_hnd);
		}
	}
}

/* Retrieve next event from event queue from this module */
intern void next_event (event_t *event)
{	struct queue_st *queue;
	if ((queue = pth_key_getdata(queue_key)) == NULL)
		fatal("Argh, the event queue is gone!", 1);
	*event = NULL;
	while (sdeque_isempty(&queue -> evq))
		pth_nap(pth_time(0, 500));		/* Poll every half second */
	*event = sdeque_first(&queue -> evq);
	sdeque_remove_head(&queue -> evq);
}

/* Dispatch event - call all callbacks for this event on this module */
intern void dispatch_event(event_t event)
{	struct callback_list *cb_list;
	callback_t cb;
	cb_list = pth_key_getdata(callback_key);
	slist_foreach(cb_list, cb)
	{	if (event_mask_match(cb -> mask, event)) /* Mask includes event -> type */
			(*cb -> callback) (event);
	}
}

/* Add a callback routine to certain types of events */
export void add_callback (event_mask mask, callback_proc proc)
{	struct callback_list *cb_list;
	struct queue_st *queue;
	callback_t cb;

	/* Add the mask & callbackproc to this thread's callback list */
	if ((cb_list = pth_key_getdata(callback_key)) == NULL)
		fatal("Hey, where did the callback list go?", 1);
	cb = tmalloc(sizeof(struct callback_st));
	cb -> callback = proc;
	cb -> mask = mask;
	slist_insert_head(cb_list, cb);

	/* Add the mask to the mask of this thread's event queue */
	if ((queue = pth_key_getdata(queue_key)) == NULL)
		notice("Hey, where did the event queue go?");
	else
		setflag (queue -> mask, mask);
}

/* Remove a callback routine for certain types of events */
export void remove_callback (event_mask mask, callback_proc proc)
{	struct callback_list *cb_list;
	callback_t cb;
	cb_list = pth_key_getdata(callback_key);
	slist_foreach(cb_list, cb)
	{	if (cb -> callback == proc)
			slist_remove(cb_list, cb, callback_st);
	}
}
