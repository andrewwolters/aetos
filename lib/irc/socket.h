/*
 * Aetos - the most aesthetically correct IRC bot ever
 *
 * socket functions header file 
 * $Id: socket.h,v 1.1 2002/08/30 16:04:11 andrewwo Exp $
 */

#ifndef aetos_socket_h
#define aetos_socket_h

#include <stdio.h>

int open_connection (char *hostname, unsigned short port);
int data_ready (int fd);
void close_connection (int fd);
void put_string (int fd, char *str);
char get_char (int fd);
void get_string (int fd, char *str, size_t maxlen);
void get_line (int fd, char *str, size_t maxlen);

#endif /* aetos_socket_h */
