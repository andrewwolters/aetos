/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * socket -- network support for aetos
 * $Id: socket.c,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#include "common.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "socket.h"

int open_connection (char *hostname, unsigned short port)
{ int fd;
  struct sockaddr_in sa;
  struct hostent *hp;
  if ((hp = gethostbyname (hostname)) == NULL)
  {
#ifdef HAVE_HERROR
	    herror ("gethostbyname");
#else
		  perror ("gethostbyname");
#endif
    return -1;
  }
  memset (&sa, 0, sizeof (struct sockaddr_in));
  memcpy ((char *) &sa.sin_addr, hp -> h_addr, hp -> h_length);
  sa.sin_family = hp -> h_addrtype;
  sa.sin_port = htons ((u_short) port);
  if ((fd = socket (hp -> h_addrtype, SOCK_STREAM, 0)) < 0)
  { perror ("socket");
    return -1;
  }
  if (connect (fd, (struct sockaddr *) &sa, sizeof (struct sockaddr_in)) < 0)
  { perror ("connect");
    close (fd);
    return -1;
  }
  return fd;
}

int data_ready (int fd)
{ struct timeval wait_time = {0L, 100000L}; // wait for 1/10 of a second
  fd_set read_fds;
  FD_ZERO (&read_fds);
  FD_SET (fd, &read_fds);
  if (pth_select (fd + 1, &read_fds, NULL, NULL, &wait_time) < 0)
  { perror ("select");
    return 0;
  }
  return FD_ISSET (fd, &read_fds);
}

void put_string (int fd, char *str)
{ pth_write (fd, str, strlen (str));
}

char get_char (int fd)
{ char retval;
  pth_read (fd, &retval, 1);
  return retval;
}

void get_string (int fd, char *str, size_t maxlen)
{ read (fd, str, maxlen);
}

void get_line (int fd, char *str, size_t maxlen)
{ int i = 0; 
  do
  { str [i] = get_char (fd);
  }
  while ((str [i++] != '\n') && (i < maxlen - 1));
  str [i] = '\0';
}

void close_connection (int fd)
{ close (fd); 
}
