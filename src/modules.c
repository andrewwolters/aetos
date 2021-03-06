/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * modules -- support for modules in aetos and event handling
 *
 * $Id: modules.c,v 1.4 2002/09/10 13:40:46 andrewwo Exp $
 */


#include "aetos.h"

#include <dlfcn.h>
#include <pth.h>
#include <csp/list.h>
#include <csp/slist.h>
#include <irc/irc.h>

#include "modules.h"
#include "event.h"
#include "mem.h"
#include "utility.h"
#include "error.h"
#include "efuns.h"


/* Some local defines of structures */
intern struct load_tbl
{	void *(*bootfunc)();
	void *so_handle;
};

/* Fill the efun table */
struct efun_st _efun_table =
{   /* Mod related */
    mod_initialize,
    mod_mainloop,
    mod_exit,

    /* Message/event related */
	get_from_event,
    irc_send_message,
    source_privmsg,
    is_command,

    /* Callbacks */
    add_callback,
    remove_callback,

    /* String utility functions */
    tokenize_string,
    duplicate_string,

    /* Memory utilisation */
    tmalloc,
    trealloc,
    tfree,
    tcalloc,

    /* Misc */
    get_gst
};

/* Setup the other structs */
intern list_t(module_list, module_st) mod_list = LIST_NIL_LIST;

private pth_key_t mod_key = PTH_KEY_INIT;

private unsigned int next_id = 1;

private efun_tbl efuns = &_efun_table;

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

/* Initialize ACS */
intern void init_modules (void)
{	module_t core;
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
{	module_t mod;
	mod = (module_t) pth_key_getdata(mod_key);
	strcpy(mod -> name, name);
	mod -> major = major;
	mod -> minor = minor;
	init_callback_list();
	return 1;
}

/* Exit from a module's thread */
export void mod_exit (void)
{	pth_exit(NULL);
}

/* Search for a module with id */
intern module_t mod_search (int id)
{	module_t m;
	list_foreach(&mod_list, m)
	{	if (m -> id == id)
			return m;
	}
	return NIL_MODULE;
}

/* Print a list of all the loaded modules */
intern void mod_listing (char *dest)
{	module_t m;
	char buf[100];
	bzero(buf, 100 * sizeof(char));
	irc_send_message (AETOS->serversocket, dest, "[CORE] Loaded modules:");
	list_foreach(&mod_list, m)
	{	sprintf(buf, "[CORE] %d : %s - %d.%d", m->id, m->name, m->major, m->minor);
		irc_send_message (AETOS->serversocket, dest, buf);
	}
}

/* Return own mod handle */
intern module_t mod_self (void)
{	return ((module_t) pth_key_getdata(mod_key));
}

/* Return own id */
intern int mod_self_id (void)
{	module_t mod = ((module_t) pth_key_getdata(mod_key));
	return mod -> id;
}

/* Module's main loop. This is where the events are grabbed
 * from the queue and dispatched to the apropriate callbacks
 */
export void mod_mainloop (void)
{	event_t event;
	while (1)
	{	next_event(&event);
		dispatch_event(event);
		destroy_event(event);
		pth_yield(NULL);
		pth_cancel_point();
	}
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
	init_event_queue();
	load_info -> bootfunc(efuns, 0, NULL);

	/* NOTREACHED */
	return NULL;
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
	notice("Opening module");
	notice(buf);
	dlerror();
 	if (! (so_handle = dlopen (buf, RTLD_LAZY)) )
	{	error = dlerror();
		warning(error);
		// tfree((void *) error);
		return 0;
	}
#ifdef NEED_USCORE
	sprintf (buf, "_%s_init", name);
#else
	sprintf (buf, "%s_init", name);
#endif
	if (! (bootfunc = (void *(*)()) dlsym (so_handle, buf)) )
	{	error = dlerror();
		warning(error);
		dlclose (so_handle);
		// tfree((void *) error);
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
	pth_cancel(mod -> thread_hnd);
	tfree(mod);

	/* Close the object */
	dlerror();
	if (dlclose(mod -> so_hnd))
	{	error = dlerror();
		warning(error);
		// tfree((void *) error);
		return 0;
	}

	return 1;
}
