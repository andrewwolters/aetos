/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * These functions handle memory allocation and garbage collection
 *
 * $Id: mem.h,v 1.1 2002/08/30 15:55:50 andrewwo Exp $
 */

#ifndef AETOS_MEM_H
#define AETOS_MEM_H

#include "common.h"

void *tmalloc PROTO((size_t size));
void *tcalloc PROTO((int num, size_t size));
void tfree PROTO((void *ptr));
void *trealloc PROTO((void *ptr, size_t size));
int collect_garbage PROTO((void));


#endif /* AETOS_MEM_H */
