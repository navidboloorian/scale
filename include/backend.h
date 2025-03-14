#ifndef _BACKEND_H_
#define _BACKEND_H_

#include <stdbool.h>

typedef struct Backend {
  char *address;
  char *port;
  int connections;
  bool is_up;
} Backend;

int initialize_backends();
void request_backend(char*, int, Backend);

extern Backend backends[3];
extern int num_backends;

#endif