/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * resources - interface to the resource file(s)
 *
 * $Id: resources.h,v 1.2 2002/09/09 19:27:23 andrewwo Exp $
 */

#ifndef AETOS_RESOURCES_H
#define AETOS_RESOURCES_H

#include "common.h"

/* Some structures and stuff */

/* Interface functions */
extern int hash_resource_file (const char *filename);
extern int query_resource (const char *query, ...);
extern int insert_resource (const char *speci, ...);
extern int delete_resource (const char *speci);
extern int delete_all_resources (void);

#endif /* AETOS_RESOURCES_H */
