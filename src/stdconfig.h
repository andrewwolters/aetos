/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * Configuration header file for default values
 *
 * $Id: stdconfig.h,v 1.3 2002/09/09 19:27:23 andrewwo Exp $
 */

#ifndef AETOS_STDCONFIG_H
#define AETOS_STDCONFIG_H

#define MAXNICKLEN 9
#define MAXMSGLEN  512
#include <sys/param.h>
#ifndef MAXHOSTNAMELEN
#include <netdb.h>
#endif /* MAXHOSTNAMELEN */

#define DEFAULT_CHANNEL "#aetos"
#define DEFAULT_SERVER "irc.kerguelen.org"
#define DEFAULT_PORT   6667
#define DEFAULT_NICK   "aetos"
#define DEFAULT_USER   "vaguebot"
#define DEFAULT_REAL   "Aetos 0.1a"

#define CONNECTION_PASSWORD "geheim"
#define COMMAND_PREFIX  '.'

#endif /* AETOS_STDCONFIG_H */
