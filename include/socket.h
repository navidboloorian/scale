#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <netdb.h>
#include <stdbool.h>

int find_valid_socket(struct addrinfo *, bool);

#endif