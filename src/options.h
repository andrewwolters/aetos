/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * option handling header file
 * $Id: options.h,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#ifndef aetos_options_h
#define aetos_options_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

struct _options_rec
{ char *server_name,
       *nickname,
       *username,
       *realname,
       *channel;
  unsigned short port;
};
typedef struct _options_rec options_rec;

options_rec *get_options (int argc, char **argv);
options_rec *make_options_rec (char *server, char *nick, char *user, char *real,
                               char *channel, unsigned short port);
void free_options_rec (options_rec *opt);

#endif /* aetos_options_h */
