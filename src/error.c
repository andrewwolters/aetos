/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * The error subsystem handles all kinds of errors and fatal errors.
 *
 * $Id: error.c,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#include "common.h"
#include "error.h"


void _error_mesg (int exit_status, const char *mode,
                          const char *message, const char *file, int line)
{	if (line == 0)
		fprintf(stderr, "aetos: %s %s.\n", mode, message);
	else
		fprintf(stderr, "aetos: %s in %s on line %d: %s.\n", mode, file,
			line, message);
	if (exit_status >= 0)
		exit(exit_status);
}

