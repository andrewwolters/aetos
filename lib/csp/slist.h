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
 * $Id: slist.h,v 1.2 2002/09/10 13:45:51 andrewwo Exp $
 * ts=4
 */

#ifndef CSP_SLIST_H
#define CSP_SLIST_H

/* Def's for single linked lists */
#define SLIST_NIL_LIST { NULL }

#define slist_t(name, type)										\
struct name														\
{	struct type *first;	/* first element */						\
}

#define SLIST_LINK(type)										\
struct															\
{	struct type *next;	/* next element */						\
} _sl_link_part
 
/* The functions */
#define slist_instantiate(slist)((slist)->first = NULL)
#define	slist_first(slist)		((slist)->first)
#define slist_next(elem)		((elem)->_sl_link_part.next)
#define	slist_isempty(slist)	((slist)->first == NULL)

#define slist_insert_head(slist, elem)							\
	((elem)->_sl_link_part.next = (slist)->first,				\
	 (slist)->first = (elem))

#define slist_insert_after(listelem, elem)						\
	((elem)->_sl_link_part.next = (listelem)->_sl_link_part.next,\
	 (listelem)->_sl_link_part.next = (elem))

#define slist_remove_first(slist)								\
	((slist)->first = (slist)->first->_sl_link_part.next)

#define slist_remove(slist, elem, type) do	 					\
{	if ((elem) == (slist)->first)								\
	{	(slist)->first = (slist)->first->_sl_link_part.next;	\
	} else														\
	{	struct type *_sl_cur = (slist)->first;					\
		while(_sl_cur->_sl_link_part.next != (elem))			\
			_sl_cur = _sl_cur->_sl_link_part.next;				\
		_sl_cur->_sl_link_part.next =							\
		    _sl_cur->_sl_link_part.next->_sl_link_part.next;	\
	}															\
} while (0)

#define slist_foreach(slist, var)								\
	for((var) = (slist)->first; (var); (var) = (var)->_sl_link_part.next)

#define slist_empty(slist, type, free_func)					\
	while((slist)->first != NULL)								\
	{	struct type *_sl_cur = (slist)->first;					\
		(slist)->first = (slist)->first->_sl_link_part.next;	\
		free_func(_sl_cur);										\
	}

#endif /* CSP_SLIST_H */
