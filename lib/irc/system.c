/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * system -- system functions 
 * $Id: system.c,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "system.h"

void *ckmalloc (size_t size)
{ void *ret = malloc (size);
  if (ret == NULL)
  { fprintf (stderr, "Kahoogawooga! Memory allocation error!\n");
    exit (1);
  }
  return ret;
}

void *ckrealloc (void *ptr, size_t size)
{
	void *ret = realloc (ptr, size);
	if (ret == NULL)
	{
		fprintf (stderr, "Blup, could not reallocate memory! BOink...\n");
		exit(1);
	}
	return ret;
}

char *ckstrdup (char *str)
{ char *ret = (char *) ckmalloc (strlen (str) + 1);
  strcpy (ret, str);
  return ret;
}
