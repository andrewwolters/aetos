/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * efuns - All the exported functions
 *
 * $Id: efuns.h,v 1.3 2002/09/10 13:40:46 andrewwo Exp $
 */

#ifndef AETOS_EFUNS_H
#define AETOS_EFUNS_H

/*
 * Create a script to autogenerate this file (and /mod/mod.h) from
 * a 'grep export *.c'
 */

/* External function table */
typedef struct efun_st *efun_tbl;
struct efun_st
{	/* Mod related */
	int (*mod_initialize) (char *, int, int);
	void (*mod_mainloop) (void);
	void (*mod_exit) (void);

	/* Message/event related */
	void (*get_from_event) (event_t, void **);
	void (*send_message) (int, char *, char *);
	char *(*source_privmsg) (ircmsg_evt);
	int (*is_command) (char *, char *);

	/* Callbacks */
	void (*add_callback) (event_mask, callback_proc);
	void (*remove_callback) (event_mask, callback_proc);

	/* String utility functions */
	void (*tokenize_string) (char *, char **, int);
	char *(*duplicate_string) (char *);

	/* Memory utilisation */
	void *(*tmalloc) (size_t);
	void *(*trealloc) (void *, size_t);
	void (*tfree) (void *);
	void *(*tcalloc) (int, size_t);

	/* Misc */
	gs_table (*get_gst) (void);
};


#endif /* AETOS_EFUNS_H */
