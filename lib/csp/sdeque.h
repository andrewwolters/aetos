/*
 * CrystalRealms Software Platform
 * Copyright (c) 2002 Andrew Wolters
 *
 * Inline functions (implemented as macros) for singly linked double ended
 * queues.
 *
 * $Id: sdeque.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 * ts=4
 */

#ifndef CSP_SDEQUE_H
#define CSP_SDEQUE_H

	/* Some def's for double ended queues */
#define SDEQUE_NIL_DEQUE { NULL }

#define sdeque_t(name, type)												\
struct name																	\
{	struct type *first;														\
	struct type *last;														\
}

#define SDEQUE_LINK(type)													\
struct																		\
{	struct type *next;														\
} _sd_link_part;

	/* The functions */
#define sdeque_instantiate(sdeque_t) 										\
	((sdeque_t)->first = NULL, (sdeque_t)->last = NULL)
#define sdeque_first(sdeque_t)		((sdeque_t)->first)
#define sdeque_last(sdeque_t)		((sdeque_t)->last)
#define sdeque_next(elem)			((elem)->_sd_link_part.next)
#define sdeque_isempty(sdeque_t)	((sdeque_t)->first == NULL)

#define sdeque_insert_head(sdeque_t, elem) do								\
{	if (((elem)->_sd_link_part.next = (sdeque_t)->first) == NULL)			\
		(sdeque_t)->last = (elem);											\
	 (sdeque-t)->first = (elem);											\
}

#define sdeque_insert_after(sdeque_t, sdelem, elem) do						\
{	if (((elem)->_sd_link_part.next = (sdelem)->_sd_link_part.next) ==NULL)	\
		(sdeque_t)->last = (elem);											\
	 (sdelem)->_sd_link_part.next = (elem);									\
}

#define sdeque_insert_tail(sdeque_t, elem)									\
	((elem)->_sd_link_part.next = NULL,										\
	 (sdeque_t)->last->_sd_link_part.next = (elem),							\
	 (sdeque_t)->last = (elem))

#define sdeque_remove_head(sdeque_t)										\
	if (((sdeque_t)->first = (sdeque_t)->first->_sd_link_part.next) ==NULL)	\
		(sdeque_t)->last = (sdeque_t)->first;


/* Not supported yet
#define sdeque_remove(sdeque_t, elem) do									\
{	if ((sdeque_t)->first == (elem))										\
	{	sdeque_remove_head(sdeque_t);										\
	} else																	\
	{
*/

#define sdeque_foreach(sdeque_t, var)										\
	for ((var) = (sdeque_t)->first; (var); (var = (var)->_sd_link_part.next)


#endif /* CSP_SDEQUE_H */
