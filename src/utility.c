/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * utility -- miscelaneous useful functions
 *
 * $Id: utility.c,v 1.4 2004/09/21 13:45:45 semprini Exp $
 */

#include "aetos.h"

#include <irc/irc.h>
#include <csp/csp.h>

#include "stdconfig.h"
#include "mem.h"
#include "utility.h"
#include "event.h"
#include "error.h"


/* Return a newly allocated chunk of memory with the duplicate string */
export char *duplicate_string (char *str)
{	char *ret;
	int len = strlen(str);
	if (! (ret = (char *) malloc (len + 1)) ) fatal("Wham", 2);
	bzero(ret, len + 1);
	strcpy (ret, str);
	return ret;
}

/* Get the source of the message from the ircmsg_evt, returns the pointer
 * to this newly allocated string
 */
export char *source_privmsg (ircmsg_evt mesg)
{	char *dest;
  char *argv[1];
	if (mesg.param[0] == '#')
	{
		tokenize_string (mesg.param, argv, 1);
		dest = duplicate_string (argv [0]);
	}
	else
	{
		/* [semprini] This is a string of the form [nick]![realname].
		 * Quick hack here for extracting this, not sure if it's quite all right (andrewwo?)
		 */
		char nick [strlen (mesg.pre)];
		sscanf (mesg.pre, "%[^!]!%*s", nick);
		dest = duplicate_string(nick);
	}
	return dest;
}

/* Chop the string in parts, ie tokens, limited by count */
export void tokenize_string (char *str, char *argv[], int count)
{	char **ptr;
	char *buf = duplicate_string (str);
	ptr = argv;
#ifdef HAVE_STRSEP
	while (ptr < argv + count)
	{	*ptr = strsep(&buf, " \t:");
		if (*ptr == NULL)
			break;
		if (**ptr != '\0')
			ptr++;
	}
#else
	/* [andrewwo]
 	 * Quick hack, still buggy I think */
	*ptr = strtok(&buf, " \t:");
	ptr++;
	while (ptr < argv + count)
	{	*ptr = strtok(NULL, " \t:");
		if (*ptr == NULL) break;
		if (**ptr != NULL) ptr++;
	}
#endif
	/*
	 * buf will be completely free'd when the argv is free'd
	 * so a tfree here of buf is not necessary
	 */
}

/* Check if str is command cmd (max. 15 chars)
 * [andrewwo] make an easier #define?
 */
export int is_command (char *str, char *cmd)
{	char buf[16];
	int len = strlen(cmd);
	if (str == NULL) return 0;
	len = min(len, 15);
	bzero(buf, 16);
	strncpy(buf + 1, cmd, len);
	buf[0] = COMMAND_PREFIX;
	return ( (strcmp(str, buf) == 0) );
}
