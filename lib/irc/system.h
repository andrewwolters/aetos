/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * system functions  
 * $Id: system.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#ifndef aetos_sytem_h
#define aetos_sytem_h

void *ckmalloc (size_t size);
char *ckstrdup (char *str);
void *ckrealloc (void *ptr, size_t size);

#endif /* aetos_system_h */
