/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * option handling header file
 * $Id: options.h,v 1.2 2002/09/14 23:14:03 semprini Exp $
 */

#ifndef aetos_options_h
#define aetos_options_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

void get_options (int argc, char **argv, gs_table *t);

#endif /* aetos_options_h */
