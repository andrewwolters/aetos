/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * system -- system functions 
 * $Id: system.c,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#include "aetos.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "system.h"
#include "options.h"
#include "error.h"

char *ckstrdup (char *str)
{ char *ret;
  if (! (ret = (char *) malloc (strlen (str) + 1)) ) fatal("Wham", 2);
  bzero(ret, strlen (str) + 1);
  strcpy (ret, str);
  return ret;
}
