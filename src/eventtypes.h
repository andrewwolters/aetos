/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * This include file is to prevent a lot of dependencies
 *
 * $Id: eventtypes.h,v 1.2 2004/09/21 15:19:22 semprini Exp $
 */

#ifndef AETOS_EVENTTYPES_H
#define AETOS_EVENTTYPES_H

#define bit(n)				(1L<<(n))

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
#define EvtPartMask			bit(7)
#define EvtQuitMask			bit(8)

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


#endif /* AETOS_EVENTTYPES_H */
