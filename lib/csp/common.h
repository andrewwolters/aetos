/*
 * CrystalRealms Software Platform
 * Copyright (c) 2002 Andrew Wolters
 *
 * Common include file included by all sources of the csp library
 *
 * $Id: common.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#ifndef CSP_COMMON_H
#define CSP_COMMON_H

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

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS }
#else
#  define BEGIN_CDECLS
#  define END_C_DECLS
#endif

#if __STDC__
#  ifndef NOPROTOS
#     define PROTO(args)	args
#  endif
#endif
#ifndef PROTO
#  define PROTO(args)		()
#endif

#if __STDC__
typedef void * void_ptr;
#else
typedef char * void_ptr;
#endif

#if HAVE_STDARG_H
#  include <stdarg.h>
#  define xva_start(a, f)	va_start(a, f)
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

#endif /* CSP_COMMON_H */
