/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * modules -- support for modules in aetos
 *
 * $Id: modules.h,v 1.3 2002/09/09 19:27:22 andrewwo Exp $
 */

#ifndef AETOS_MODULES_H
#define AETOS_MODULES_H

#include <pth.h>
#include <csp/list.h>
#include <csp/slist.h>
#include "common.h"

/* Some defines */
#define NIL_MODULE		NULL
#define MOD_NAME_LEN	40
#define MOD_DIR			"../mod"
#define MOD_DIR_PREFIX	"m_"
#define MOD_EXT			".mod"

/* Module record */
typedef struct module_st *module_t;
struct module_st
{	LIST_LINK(module_st);	/* Link area */
	int id;					/* ID provided by the core */
	char name[MOD_NAME_LEN];/* Name */
	int major;				/* Major version number */
	int minor;				/* Minor version number */
	void *so_hnd;			/* Handle : pointer to the mod's shared object */
	pth_t thread_hnd;		/* Handle of the thread bound to mod */
};

/* Routines for module administration */
void init_modules (void);
int mod_load (char *file, int argc, char *argv[]);
int mod_unload (int id);
int mod_initialize (char *name, int major, int minor);
void mod_mainloop (void);
void mod_exit (void);
void mod_listing (char *dest);
module_t mod_search (int id); 
module_t mod_self (void); 
int mod_self_id (void);


#endif /* AETOS_MODULES_H */
