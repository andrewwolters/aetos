/*
 * CrystalRealms Software Platform
 * Copyright (c) 2002 Andrew Wolters
 *
 * Inline functions (implemented as macros) for double linked lists.
 *
 * $Id: list.h,v 1.2 2002/09/10 13:45:51 andrewwo Exp $
 * ts=4
 */

#ifndef CSP_LIST_H
#define CSP_LIST_H

	/* Some def's for double linked lists */
#define LIST_NIL_LIST { NULL }

#define list_t(name, type)										\
struct name														\
{	struct type *first;	/* first element */						\
}

#define LIST_LINK(type)											\
struct															\
{	struct type *next;	/* next element */						\
	struct type **prev;	/* pointer to previous next element */	\
} _dl_link_part

	/* The functions */
#define	list_instantiate(list)	((list)->lh_first = NULL)
#define list_first(list)		((list)->first)
#define list_next(elem)			((elem)->_dl_link_part.next)
#define	list_isempty(list)		((list)->first == NULL)

#define list_insert_head(list, elem) do							\
{	if (((elem)->_dl_link_part.next = (list)->first) != NULL)	\
		(list)->first->_dl_link_part.prev =						\
			&(elem)->_dl_link_part.next;						\
	(list)->first = (elem);										\
	(elem)->_dl_link_part.prev = &(list)->first;				\
} while(0)

#define list_insert_before(listelem, elem)						\
	((elem)->_dl_link_part.next = (listelm),					\
	 (elem)->_dl_link_part.prev = (listelm)->_dl_link_part.prev,\
	 *(listelm)->_dl_link_part.prev = (elem),					\
	 (listelm)->_dl_link_part.prev = &(elem)->_dl_link_part.next)

#define list_insert_after(listelem, elem)						\
	(if (((elem)->_dl_link_part.next =							\
		(listelem)->_dl_link_part.next) != NULL)				\
		(listelem)->_dl_link_part.next->_dl_link_part.prev =	\
		    &(elem)->_dl_link_part.next,						\
	 (listelem)->_dl_link_part.next = (elem),					\
	 (elem)->_dl_link_part.prev = &(listelem)->_dl_link_part.next)

#define list_remove_first(list)									\
	((list)->first = (list)->first->_sl_link_part.next,			\
	 (list)->first->_dl_link_part.prev = &(list)->first))

#define list_remove(elem) do									\
{	if ((elem)->_dl_link_part.next != NULL)						\
		(elem)->_dl_link_part.next->_dl_link_part.prev =		\
		    (elem)->_dl_link_part.prev;							\
	 *(elem)->_dl_link_part.prev = (elem)->_dl_link_part.next;	\
} while(0)

#define list_foreach(list, var)									\
	for((var) = (list)->first; (var); (var) = (var)->_dl_link_part.next)

#define list_empty(list, type, free_func)						\
	while((list) != NULL)										\
		(struct type *_dl_cur = (list)->first,					\
		 (list)->first = (list)->first->_dl_link_part.next,		\
		 free_func(_dl_cur))

#endif /* CSP_LIST_H */
