/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * resources - interface to the resource file(s)
 *
 * $Id: resources.h,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#ifndef AETOS_RESOURCES_H
#define AETOS_RESOURCES_H

#include "common.h"

	/* Some structures and stuff */

	/* Interface functions */
extern int hash_resource_file PROTO((const char *filename));
extern int query_resource PROTO((const char *query, ...));
extern int insert_resource PROTO((const char *speci, ...));

#endif /* AETOS_RESOURCES_H */
