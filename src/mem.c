/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * These functions handle memory allocation and garbage collecting
 *
 * $Id: mem.c,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#define AETOS_SUBSYSTEM	"MEMORY"

#include "common.h"
#include "error.h"

void *tmalloc (size_t size)
{	void *ret = malloc (size);
	if (ret == NULL)
		fatal ("Khahooogawooga, Memory exhausted", 1);
	return ret;
}

void *trealloc (void *ptr, size_t size)
{	void *ret = realloc (ptr, size);
	if (ret == NULL)
		fatal ("Blup, memory exhausted. Boink...", 1);
	return ret;
}

void tfree (void *ptr)
{	if (ptr == NULL)
		error ("Hmmm, where did that memory chunk go");
	else
		free (ptr);
}

void *tcalloc (size_t num, size_t size)
{	void *ret = tmalloc (num * size);
	bzero (ret, num * size);
	return ret;
}
