/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * Configuration header file for default values
 *
 * $Id: stdconfig.h,v 1.4 2003/12/05 21:43:07 semprini Exp $
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
#define DEFAULT_SERVER "irc.fluim.net"
#define DEFAULT_PORT   6667
#define DEFAULT_NICK   "aetos"
#define DEFAULT_USER   "vaguebot"
#define DEFAULT_REAL   "Aetos 0.1a"

#define CONNECTION_PASSWORD "geheim"
#define COMMAND_PREFIX  '.'

#endif /* AETOS_STDCONFIG_H */
