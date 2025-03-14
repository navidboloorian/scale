#ifndef THREADING_H_
#define THREADING_H_

#include "backend.h"

typedef struct Request {
  void (*request_function)(char *, int, Backend);
  char *value;
  int sockfd;
  Backend backend;
} Request;

void create_thread_pool();
void destroy_thread_pool();
void add_request(Request);

#endif