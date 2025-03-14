#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include "backend.h"
#include "socket.h"

char *addresses[3] = {"8080", "8081", "8082"};
int num_backends = 3;
Backend backends[3] = {NULL, NULL, NULL};

int initialize_backends() {
  int i;

  for (i = 0; i < num_backends; i++) {

    Backend backend = {
      .address = NULL,
      .port = addresses[i],
      .connections = 0,
      .is_up = true
    };

    backends[i] = backend;
  }

  return 0;
}

void request_backend(char* request, int client_sockfd, Backend backend) {
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *selected_socket;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(backend.address, backend.port, &hints, &servinfo) == -1) {
    perror("getaddrinfo");
    return;
  }

  int server_sockfd = find_valid_socket(servinfo, false);

  if (server_sockfd == -1) {
    perror("sockfd");
    return;
  }

  char response[1024];

  if(send(server_sockfd, request, strlen(request), 0) == -1) {
    perror("send");
    return;
  }

  if(recv(server_sockfd, response, 1023, 0) == -1) { 
    perror("recv");
    return;
  }

  close(server_sockfd);

  if(send(client_sockfd, response, strlen(response), 0) == -1) {
    perror("send");
    return;
  }

  printf("successfully routed to server at port %s\n", backend.port);

  close(client_sockfd);
}