/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * The error subsystem handles all kinds of errors and fatal errors.
 *
 * $Id: error.h,v 1.2 2002/09/09 19:27:22 andrewwo Exp $
 */

#ifndef AETOS_ERROR_H
#define AETOS_ERROR_H

#include "common.h"

extern void _error_mesg PROTO((int, const char *, const char *, const char *, int));

/* Convenience routines encapsulating the _aetos_error() function */
#define notice(mesg)	_error_mesg(-1, "", mesg, "", 0)
#define warning(mesg)	_error_mesg(-1, "warning:", mesg, "", 0)
#define error(mesg)		_error_mesg(-1, "ERROR:", mesg, __FILE__, __LINE__)
#define fatal(mesg,no)	_error_mesg(no, "PANIC:", mesg, __FILE__, __LINE__)
#define trace(mesg)		_error_mesg(-1, "trace:", mesg, __FILE__, __LINE__)

#endif /* AETOS_ERROR_H */
