/*
 * CrystalRealms Software Platform
 * Copyright (c) 2002 Andrew Wolters
 *
 * Inline functions (implemented as macros) for singly linked double ended
 * queues.
 *
 * $Id: sdeque.h,v 1.2 2002/09/10 13:45:51 andrewwo Exp $
 * ts=4
 */

#ifndef CSP_SDEQUE_H
#define CSP_SDEQUE_H

	/* Some def's for double ended queues */
#define SDEQUE_NIL_DEQUE(sdeque) { NULL, &(sdeque)._sd_link_part.first }

#define sdeque_t(name, type)												\
struct name																	\
{	struct type *first;		/* pointer to first element */					\
	struct type **last;		/* pointer to last next element */ 				\
}

#define SDEQUE_LINK(type)													\
struct																		\
{	struct type *next;		/* pointer to next element */					\
} _sd_link_part;

	/* The functions */
#define sdeque_instantiate(sdeque) 											\
	((sdeque)->first = NULL, (sdeque)->last = &(sdeque)->first)
#define sdeque_first(sdeque)		((sdeque)->first)
#define sdeque_last(sdeque)			((sdeque)->last)
#define sdeque_next(elem)			((elem)->_sd_link_part.next)
#define sdeque_isempty(sdeque)		((sdeque)->first == NULL)

#define sdeque_insert_head(sdeque, elem) do									\
{	if (((elem)->_sd_link_part.next = (sdeque)->first) == NULL)				\
		(sdeque)->last = &(elem)->_sd_link_part.next;						\
	 (sdeque)->first = (elem);												\
} while (0)

#define sdeque_insert_after(sdeque, sdelem, elem) do						\
{	if (((elem)->_sd_link_part.next = (sdelem)->_sd_link_part.next) == NULL)\
		(sdeque)->last = &(elem)->_sd_link_part.next;						\
	(sdelem)->_sd_link_part.next = (elem);									\
} while(0)

#define sdeque_insert_tail(sdeque, elem)									\
	((elem)->_sd_link_part.next = NULL,										\
	 *(sdeque)->last = (elem),												\
	 (sdeque)->last = &(elem)->_sd_link_part.next)

#define sdeque_remove_head(sdeque)											\
	if (((sdeque)->first = (sdeque)->first->_sd_link_part.next) == NULL)	\
		(sdeque)->last = &(sdeque)->first;

/* Not supported yet
#define sdeque_remove(sdeque, elem) do										\
{	if ((sdeque)->first == (elem))											\
	{	sdeque_remove_head(sdeque);											\
	} else																	\
	{
*/

#define sdeque_foreach(sdeque, var)											\
	for ((var) = (sdeque)->first; (var); (var = (var)->_sd_link_part.next)

#define sdeque_empty(sdeque, type, free_func)								\
	while((sdeque)->first != NULL)											\
	{	struct type *_sd_cur = (sdeque)->first;								\
		(sdeque)->first = (sdeque)->first->_sd_link_part.next;				\
		free_func(_sd_cur);													\
	}


#endif /* CSP_SDEQUE_H */
