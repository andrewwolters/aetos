/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * options -- handles command-line options
 * $Id: options.c,v 1.3 2002/09/14 21:57:59 semprini Exp $
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

options_rec *get_options (int argc, char **argv)
{ extern char *optarg;
  int optch;
  options_rec *options = make_options_rec (DEFAULT_SERVER, DEFAULT_NICK,
                                           DEFAULT_USER, DEFAULT_REAL,
                                           DEFAULT_CHANNEL, DEFAULT_PORT);
  while ((optch = getopt (argc, argv, "c:hn:p:s:")) != -1)
  { switch (optch)
    { case 'c':
        strcpy (options -> channel, optarg);
        break;
      case 'h':
        usage (argv [0]);
        break;
      case 'n':
        strcpy (options -> nickname, optarg);
        break;
      case 'p':
        options -> port = (unsigned short) atoi (optarg); 
        break;
      case 's':
        strcpy (options -> server_name, optarg);
        break;
      case '?':
      default:
        usage (argv [0]);
    }
  }
  return options;
}

options_rec *make_options_rec (char *server, char *nick, char *user, char *real,
                               char *channel, unsigned short port)
{ options_rec *retval = (options_rec *) tmalloc (sizeof (options_rec));
  retval -> server_name = duplicate_string (server); 
  retval -> nickname = duplicate_string (nick);
  retval -> username = duplicate_string (user);
  retval -> realname = duplicate_string (real);
  retval -> channel = duplicate_string (channel);
  retval -> port = port;
  return retval;
}

void free_options_rec (options_rec *opt)
{ free (opt -> server_name);
  free (opt -> nickname);
  free (opt -> username);
  free (opt -> realname);
  free (opt -> channel);
  free (opt);
}
