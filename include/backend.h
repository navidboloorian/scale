#ifndef _BACKEND_H_
#define _BACKEND_H_

#include <stdbool.h>

typedef struct Backend {
  char *addr;
  int connections;
  int sockfd;
  bool is_up;
} Backend;

Backend backends[1];

#endif