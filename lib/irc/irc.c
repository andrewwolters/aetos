/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * irc -- implementation of RFC 2812 
 * $Id: irc.c,v 1.2 2002/11/01 20:50:00 semprini Exp $
 */

#include "common.h"

#include <string.h>
#include <ctype.h>
#ifdef HAVE_STDARG_H 
#include <stdarg.h>
#else
#include <varargs.h>
#define MAXARGS 100
#endif /* HAVE_STDARG_H */

#include "system.h"
#include "socket.h"
#include "irc.h"

/*
 * make_message_rec: create message record from prefix, command and parameters
 * Internal IRClib function, allocates memory
 */
message_rec *make_message_rec (char *prefix, char *command, char *params)
{ message_rec *ret = (message_rec *) ckmalloc (sizeof (message_rec));
  ret -> prefix = ckstrdup (prefix);
  ret -> command = ckstrdup (command);
  ret -> params = ckstrdup (params);
  return ret;
}

/*
 * free_message_rec: free a created message record
 * Exported function
 */
void free_message_rec (message_rec *msg)
{ free (msg -> prefix);
  free (msg -> command);
  free (msg -> params);
  free (msg);
}

/*
 * irc_is_prefix: check whether the prefix is equal to a given string and remove
 * the leading ':' character
 * Internal IRClib function, allocates memory
 */
static char *irc_is_prefix (char *buf)
{ char *ret = (char *) ckmalloc (strlen (buf) + 1);
  if (sscanf (buf, ":%s", ret) == 1)
    return ret; 
  else
    return NULL;
}

/*
 * irc_get_command: get command from read buffer
 * Internal IRClib function, allocates memory
 */
static char *irc_get_command (char *buf)
{ char *ret = (char *) ckmalloc (strlen (buf) + 1);
  sscanf (buf, "%[A-Za-z0-9]", ret);
  return ret;
}

/*
 * irc_get_params: get parameters from read buffer
 * Internal IRClib function, allocates memory
 */
static char *irc_get_params (char *buf)
{ char *ret = (char *) ckmalloc (strlen (buf) + 1);
  sscanf (buf, "%[^\r\n]\r\n", ret);
  return ret;
}

/*
 * irc_get_message: get IRC message from connection
 * Exported function, allocates memory (through make_message_rec)
 */
message_rec *irc_get_message (int fd)
{ char message [MAXMSGLEN + 2], *prefix, *command, *params;
  message_rec *ret;
  get_line (fd, &message, MAXMSGLEN);
  if ((prefix = irc_is_prefix (message)) != NULL)
  { command = irc_get_command (message + strlen (prefix) + 2);
    params = irc_get_params (message + strlen (prefix) + strlen (command) + 3);
  }
  else
  { prefix = ckstrdup ("");
    command = irc_get_command (message);
    params = irc_get_params (message + strlen (command) + 1);
  }
  fprintf (stderr, "(%s, %s, %s)\n", prefix, command, params);
  ret = make_message_rec (prefix, command, params);
  free (prefix);
  free (command);
  free (params);
  return ret;
}

/*
 * irc_is_error: check whether an IRC message is an error
 * Exported function
 * The parameters of this function are a message record and a list of possible
 * error strings (defined in irclib.h)
 */
#ifdef HAVE_STDARG_H
int irc_is_error (message_rec *msg, ...)
{ va_list ap;
#else
int irc_is_error (va_alist)
va_dcl
{ va_list ap;
  message_rec *msg;
#endif /* HAVE_STDARG_H */
  char number [4], *error;
#ifdef HAVE_STDARG_H
  va_start (ap, msg);
#else
  va_start (ap)
  msg = va_arg (ap, message_rec *);
#endif /* HAVE_STDARG_H */
  if (!isdigit ((msg -> command) [0])) 
  { va_end (ap);
    return 0;
  }
  strncpy (number, msg -> command, 3);
  number [3] = '\0';
  error = va_arg (ap, char *);
  while (error != NULL)
  { if (strncmp (number, error, 3) == 0)
    { fprintf (stderr, "IRC error (%s): %s\n" , number, msg -> params);
      va_end (ap);
      return -1;
    }
    error = va_arg (ap, char *); 
  }
  va_end (ap);
  return 0;
}

/*
 * irc_is_ping: check if an IRC message is a ping
 * Exported function
 */
int irc_is_ping (message_rec *msg)
{ return (strncmp (msg -> command, "PING", 4) == 0);
}

/*
 * irc_is_closing: check if an IRC message means that the connection is closing
 * or has closed already
 * Exported function
 */
int irc_is_closing (message_rec *msg)
{ return ((strncmp (msg -> command, "ERROR", 5) == 0) &&
         (strstr (msg -> params, "Closing Link") != NULL)) ||
         (strlen (msg -> prefix) == 0);
}

/*
 * irc_connect: connect to an IRC server over a given socket
 * Exported function
 */
void irc_connect (int fd, char *nick, char *user, int mode, char *real)
{ char message [MAXMSGLEN + 1];
  message_rec *reply;
  int rpl_welcome_received = 0;
  // If we are connected, we can get all kinds of NOTICEs
  while (data_ready (fd))
  { reply = irc_get_message (fd);
  }
  // Connection password
  fprintf (stderr, "Sending connection password...\n");
  sprintf (message, "PASS %s\n", CONNECTION_PASSWORD);
  put_string (fd, message);
  while (data_ready (fd))
  { reply = irc_get_message (fd);
    if (irc_is_error (reply, ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTERED, NULL))
    { close_connection (fd);
      exit (1);
    }
    free_message_rec (reply);
  }
  // Nickname (max 9 characters)
  if (strlen (nick) > MAXNICKLEN)
  { fprintf (stderr, "Nick '%s' is too long (max %d characters)\n", nick,
             MAXNICKLEN);
    close_connection (fd);
    exit (1);
  }
  sprintf (message, "NICK %s\n", nick); 
  fprintf (stderr, "Sending nickname...\n");
  put_string (fd, message);
  while (data_ready (fd))
  { reply = irc_get_message (fd);
    if (irc_is_error (reply, ERR_NONICKNAMEGIVEN, ERR_ERRONEOUSNICKNAME,
                             ERR_NICKNAMEINUSE, ERR_NICKCOLLISION,
                             ERR_UNAVAILRESOURCE, ERR_RESTRICTED, NULL))
    { close_connection (fd);
      exit (1);
    }
    free_message_rec (reply);
  }
  // Username
  sprintf (message, "USER %s %d * :%s\n", user, mode, real);
  fprintf (stderr, "Sending username...\n");
  put_string (fd, message);
  while (!rpl_welcome_received)
  { reply = irc_get_message (fd);
    if (irc_is_error (reply, ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTERED, NULL))
    { close_connection (fd);
      exit (1);
    }
    if (strncmp (reply -> command, RPL_WELCOME, 3) == 0)
    { rpl_welcome_received = -1;
      fprintf (stderr, "RPL_WELCOME\n");
    }  
    free_message_rec (reply);
  }
}

/*
 * irc_join: join an IRC channel
 * Exported function
 */
void irc_join (int fd, char *channel)
{ char message [MAXMSGLEN + 1];
  message_rec *reply;
  sprintf (message, "JOIN %s\n", channel);
  put_string (fd, message);
  while (data_ready (fd))
  { reply = irc_get_message (fd);
    if (irc_is_error (reply, ERR_NEEDMOREPARAMS, ERR_BANNEDFROMCHAN,
                             ERR_INVITEONLYCHAN, ERR_BADCHANNELKEY,
                             ERR_CHANNELISFULL, ERR_BADCHANMASK,
                             ERR_NOSUCHCHANNEL, ERR_TOOMANYCHANNELS,
                             ERR_TOOMANYTARGETS, ERR_UNAVAILRESOURCE, NULL))
    { close_connection (fd);
      exit (1);
    }
    free_message_rec (reply);
  }
}

/*
 * irc_pong: send a ping reply ("pong")
 * Exported function
 */
void irc_pong (int fd, char *server)
{ char message [MAXMSGLEN + 1];
  message_rec *reply;
  sprintf (message, "PONG %s\n", server);
  put_string (fd, message);
  while (data_ready (fd))
  { reply = irc_get_message (fd);
    if (irc_is_error (reply, ERR_NOORIGIN, ERR_NOSUCHSERVER, NULL))
    { close_connection (fd);
      exit (1);
    }
    free_message_rec (reply);
  }
}

/*
 * irc_send_message: send an IRC PRIVMSG
 * Exported function
 */
void irc_send_message (int fd, char *dest, char *msg)
{ char message [MAXMSGLEN + 1];
  message_rec *reply;
  sprintf (message, "PRIVMSG %s :%s\n", dest, msg);
  put_string (fd, message);
  while (data_ready (fd))
  { reply = irc_get_message (fd);
    free_message_rec (reply);
  }
}

/*
 * irc_quit: close connection
 * Exported function
 */
void irc_quit (int fd, char *reason)
{ char message [MAXMSGLEN + 1];
  message_rec *reply;
  sprintf (message, "QUIT :%s\n", reason);
  put_string (fd, message);
  while (data_ready (fd))
  { reply = irc_get_message (fd);
    if (irc_is_error (reply, ERR_NOORIGIN, ERR_NOSUCHSERVER, NULL))
    { close_connection (fd);
      exit (1);
    }
    else if (irc_is_closing (reply))
    { fprintf (stderr, "Signing off.\n");
      close_connection (fd);
      exit (0);
    }
    free_message_rec (reply);
  }
}
