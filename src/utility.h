/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * utility -- miscelaneous useful functions
 *
 * $Id: utility.h,v 1.1 2002/09/14 21:15:55 andrewwo Exp $
 */

#ifndef AETOS_UTILITY_H
#define AETOS_UTILITY_H

#include "common.h"

#include <event.h>


char *source_privmsg (ircmsg_evt mesg);
void tokenize_string (char *str, char *argv[], int count);
char *duplicate_string (char *str);
void free_argument_list (char *argv[], int count);
int is_command (char *str, char *cmd);


#endif /* AETOS_UTILITY_H */
