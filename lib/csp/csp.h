/*
 * CrystalRealms Software Platform
 * Copyright (c) 2002 Andrew Wolters
 *
 * CSP Intrinsics - some useful macros
 *
 * $Id: csp.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#ifndef CSP_CSP_H
#define CSP_CSP_H

#include "common.h"

/* Essential values */
#ifndef FALSE
#  define FALSE (0)
#endif
#ifndef TRUE
#  define TRUE (!FALSE)
#endif
#ifndef NIL
#  define NIL '\0'
#endif
#ifndef NULL
#  define NULL (void *)0
#endif

/* Bitmask & flags */
#define bit(n)		(1L<<(n))
#define setflag(x,f)	((x) |= (f))
#define clearflag(x,f)	((x) &= ~(f))
#define checkflag(x,f)	((x) & (f))

/* Character classification */
#ifndef isblank
#  define isblank(c)		((c) == ' ' || (c) == '\t')
#endif

/* Miscelaneous macros */
#define max(a, b)		((a) > (b) ? (a) : (b))
#define min(a, b)		((a) < (b) ? (a) : (b))

#endif /* CSP_CSP_H */
