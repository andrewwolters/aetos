/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * modules -- support for modules in aetos and event handling
 *
 * $Id: modules.c,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */


#include "aetos.h"

#include <dlfcn.h>
#include <pth.h>
#include <csp/list.h>
#include <csp/slist.h>

#include "system.h"
#include "error.h"
#include "mem.h"
#include "modules.h"
#include "event.h"
#include "efuns.h"


/* Some local defines and setup of structures */
list_t(module_list, module_st) mod_list = LIST_NIL_LIST;
slist_t(callback_list, callback_st);

private unsigned int next_id = 1;
private pth_key_t mod_key = PTH_KEY_INIT;
intern pth_key_t callback_key = PTH_KEY_INIT;

intern efun_tbl efuns = &_efun_table;
intern struct load_tbl
{	void *(*bootfunc)();
	void *so_handle;
};

/* Create a unique id */
private unsigned int unique_id(void)
{	unsigned int ret;
	while (mod_search(next_id) != NIL_MODULE)
	{	next_id++;
	}
	ret = next_id;
	next_id++;
	return ret;
}

/* Destroy thread local callback list */
private void destroy_callback_list(void *arg)
{	struct callback_list *cblist = (struct callback_list *) arg;
	slist_delete(cblist, callback_st, tfree);
	tfree(cblist);
}

/* Initialize ACS */
intern void init_modules (void)
{	module_t core;
	pth_key_create(&callback_key, destroy_callback_list);
	pth_key_create(&mod_key, NULL);

	/* Install the core in the module list */
	core = tmalloc(sizeof(struct module_st));
	core -> id = unique_id();
	core -> thread_hnd = pth_self();
	list_insert_head(&mod_list, core);
	pth_key_setdata(mod_key, (const void *) core);
	mod_initialize("core", 0, 303);
	notice("Modules: initialized");
}

/* Initialisation function for a module */
export int mod_initialize (char *name, int major, int minor)
{	struct callback_list *cblist;
	module_t mod;

	/* The key must be set for each thread */
	if ((struct callback_list *) pth_key_getdata(callback_key) == NULL)
	{	cblist = tmalloc(sizeof(struct callback_list));
		slist_instantiate(cblist);
		pth_key_setdata(callback_key, (const void *) cblist);
	}
	mod = (module_t) pth_key_getdata(mod_key);
	strcpy(mod -> name, name);
	mod -> major = major;
	mod -> minor = minor;
	return 1;
}

/* Exit from a module's thread */
export void mod_exit (void)
{	pth_exit(NULL);
}

/* Search for a module with id */
export module_t mod_search (int id)
{	module_t m;
	list_foreach(&mod_list, m)
	{	if (m -> id == id)
			return m;
	}
	return NIL_MODULE;
}

/* Return own mod handle */
export module_t mod_self (void)
{	return ((module_t) pth_key_getdata(mod_key));
}

/* Return own id */
export int mod_self_id (void)
{	module_t mod = ((module_t) pth_key_getdata(mod_key));
	return mod -> id;
}

/* Second stage module loader */
private void *mod_load_2 (void *arg)
{	module_t mod;
	struct load_tbl *load_info = (struct load_tbl *) arg;
	
	/* Insert into module list and thread local data */
	mod = tmalloc(sizeof(struct module_st));
	mod -> id = unique_id();
	mod -> so_hnd = load_info -> so_handle;
	mod -> thread_hnd = pth_self();
	list_insert_head(&mod_list, mod);
	if ((module_t) pth_key_getdata(mod_key) == NULL)
		pth_key_setdata(mod_key, (const void *) mod);
	load_info -> bootfunc(efuns, 0, NULL);

	/* NOTREACHED */
}

/* Open the shared library and bootstraps the module
 *  the entry point in a module is <modname>_init
 */
intern int mod_load (char *name, int argc, char *argv[])
{	struct load_tbl *load_info;
	const char *error;
	pth_attr_t attr;
	char buf[MAXPATHLEN];
	void *(*bootfunc)();
	void *so_handle;

	/* Administrative setup and load/resolv of the module */
	sprintf (buf, "%s/%s%s/%s%s", MOD_DIR, MOD_DIR_PREFIX, name, name, MOD_EXT);
	notice("Opening module ");
	notice(buf);
	dlerror();
 	if (! (so_handle = dlopen (buf, RTLD_LAZY)) )
	{	error = dlerror();
		fprintf (stderr, "dlopen() - %s\n", error);
		tfree((void *) error);
		return 0;
	}
#ifdef NEED_USCORE
	sprintf (buf, "_%s_init", name);
#else
	sprintf (buf, "%s_init", name);
#endif
	if (! (bootfunc = (void *(*)()) dlsym (so_handle, buf)) )
	{	error = dlerror();
		fprintf (stderr, "dlsym() - %s\n", error);
		dlclose (so_handle);
		tfree((void *) error);
		return 0;
	}

	/* Construct the table with loading info for pth_spawn */
	load_info = tmalloc(sizeof(struct load_tbl));
	load_info -> bootfunc = bootfunc;
	load_info -> so_handle = so_handle;

	/* Bootstrap the module */
	attr = pth_attr_new();
	pth_attr_set(attr, PTH_ATTR_NAME, name);
	pth_attr_set(attr, PTH_ATTR_STACK_SIZE, 64*1024);
	pth_attr_set(attr, PTH_ATTR_JOINABLE, FALSE);
	pth_spawn(attr, mod_load_2, (void *) load_info);
	pth_attr_destroy(attr);

	notice ("Module booted.");

	return 1;
}

/* Unload the module */
intern int mod_unload (int id)
{	const char *error;
	module_t mod;

	/* Get handle from module list and remove */
	if (! (mod = mod_search(id)) )
		return 0;
	list_remove(mod);
	tfree(mod);

	/* Close the object */
	dlerror();
	if (dlclose(mod -> so_hnd))
	{	error = dlerror();
		fprintf (stderr, "dlclose() - %s", error);
		tfree((void *) error);
		return 0;
	}

	return 1;
}
