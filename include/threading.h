#ifndef THREADING_H_
#define THREADING_H_

typedef struct Request {
  void (*request_function)(char*, char*, char*, char**, int);
  char *type;
  char *path;
  char *protocol;
  char **response;
  int sockfd;
} Request;

void create_thread_pool();
void destroy_thread_pool();
void add_request(Request request);

#endif