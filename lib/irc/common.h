/*
 * IRC Library
 *
 * Common include file included by all sources.
 *
 * $Id: common.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#ifndef IRC_COMMON_H
#define IRC_COMMON_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>

#ifdef STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_ERRNO_H
#  include <errno.h>
#endif
#ifndef errno
extern int errno;
#endif

#if __STDC__
#  ifndef NOPROTOS
#     define PROTO(args)	args
#  endif
#endif
#ifndef PROTO
#  define PROTO(args)		()
#endif

/*
#if __STDC__
typedef void * void_ptr;
#else
typedef char * void_ptr;
#endif
*/

#if HAVE_STDARG_H
#  include <stdarg.h>
#  define xva_start(a, f)		va_start(a, f)
#else
#  ifdef HAVE_VARARGS_H
#     include <varargs.h>
#     define xva_start(a, f)	va_start(a)
#  endif
#endif
#ifndef xva_start
	error no variadic api
#endif

#if !HAVE_BZERO && HAVE_MEMSET
#  define bzero(buf, bytes)	((void) memset (buf, 0, bytes))
#endif

#endif /* IRC_COMMON_H */
