/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * options -- handles command-line options
 * $Id: options.c,v 1.4 2002/09/14 23:14:03 semprini Exp $
 */

#include "aetos.h"

#include <string.h>
#include <unistd.h>

#include "mem.h"
#include "utility.h"
#include "options.h"

static void usage (char *program_name)
{ fprintf (stdout, "Usage: %s [-c channel] [-h] [-n nick] [-p port] "
                   "[-s server name]\n", program_name);
  fprintf (stdout, "  -c: join a specific channel\n");                
  fprintf (stdout, "  -h: print this help message\n");
  fprintf (stdout, "  -n nick: use a specific nick\n");
  fprintf (stdout, "  -p port: connect to a specific port\n");
  fprintf (stdout, "  -s server name: connect to a specific server\n");
  exit (1);
}

void get_options (int argc, char **argv, gs_table *t)
{ extern char *optarg;
  int optch;
  while ((optch = getopt (argc, argv, "c:hn:p:s:")) != -1)
  { switch (optch)
    { case 'c':
				free ((*t) -> channelname);
				(*t) -> channelname = duplicate_string (optarg);
        break;
      case 'n':
				free ((*t) -> botname);
				(*t) -> botname = duplicate_string (optarg);
        break;
      case 'p':
        (*t) -> serverport = (unsigned short) atoi (optarg); 
        break;
      case 's':
				free ((*t) -> servername);
				(*t) -> servername = duplicate_string (optarg);
        break;
      case 'h':
      case '?':
      default:
        usage (argv [0]);
    }
  }
}
