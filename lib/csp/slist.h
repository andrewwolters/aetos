/*
 * CrystalRealms Software Platform
 * Copyright (c) 2002 Andrew Wolters
 *
 * Inline functions (macros) for single linked lists
 * Examples:
 *   typedef struct a_rec_st *a_rec;
 *   struct a_rec_st
 *   {  ...
 *      SLIST_LINK(a_rec);
 *      ...
 *   };
 *   a_rec *r1;
 *   slist_new(a_list, a_rec_st) a = SLIST_NIL_LIST;
 *   slist_insert_head(&a, r1);
 *   struct a_list *b = xcalloc(1, struct a_list);
 *   slist_instantiate(b);
 *   slist_insert_head(b, r1);
 *
 * $Id: slist.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 * ts=4
 */

#ifndef CSP_SLIST_H
#define CSP_SLIST_H

	/* Def's for single linked lists */
#define SLIST_NIL_LIST { NULL }

#define slist_t(name, type)									\
struct name														\
{	struct type *first;	/* first element */						\
}

#define SLIST_LINK(type)										\
struct															\
{	struct type *next;	/* next element */						\
} _sl_link_part
 
	/* The functions */
#define slist_instantiate(head)	((head)->first = NULL)
#define	slist_first(head)		((head)->first)
#define slist_next(elem)		((elem)->_sl_link_part.next)
#define	slist_isempty(head)		((head)->first == NULL)

#define slist_insert_head(head, elem)							\
	((elem)->_sl_link_part.next = (head)->first,				\
	 (head)->first = (elem))

#define slist_insert_after(listelem, elem)						\
	((elem)->_sl_link_part.next = (listelem)->_sl_link_part.next,	\
	 (listelem)->_sl_link_part.next = (elem))

#define slist_remove_first(head)								\
	((head)->first = (head)->first->_sl_link_part.next)

#define slist_remove(head, elem, type) do	 					\
{	if ((elem) == (head)->first)								\
	{	(head)->first = (head)->first->_sl_link_part.next;		\
	} else														\
	{	struct type *_sl_cur = (head)->first;					\
		while(_sl_cur->_sl_link_part.next != (elem))			\
			_sl_cur = _sl_cur->_sl_link_part.next;				\
		_sl_cur->_sl_link_part.next =							\
		    _sl_cur->_sl_link_part.next->_sl_link_part.next;	\
	}															\
} while (0)

#define slist_foreach(head, var)								\
	for((var) = (head)->first; (var); (var) = (var)->_sl_link_part.next)

#define slist_delete(head, type, free_func)						\
	while((head)->first != NULL)								\
	{	struct type *_sl_cur = (head)->first;					\
		(head)->first = (head)->first->_sl_link_part.next;		\
		free_func(_sl_cur);										\
	}

#endif /* CSP_SLIST_H */
