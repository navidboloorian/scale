#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include "backend.h"
#include "socket.h"

char *addresses[1] = {"http://localhost:8080"};
Backend backends[1];
int num_backends = 1;

int initialize_backends() {
  int i;

  int num_addresses = 1;

  for (i = 0; i < num_addresses; i++) {

    Backend backend = {
      .address = "http://localhost",
      .port = "8080",
      .connections = 0,
      .is_up = true
    };

    backends[i] = backend;
  }

  return 0;
}

void request_backend(char* request, char* response) {
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *selected_socket;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char port[16];

  if (getaddrinfo(NULL, backends[0].port, &hints, &servinfo) == -1) {
    perror("getaddrinfo");
    return;
  }

  int sockfd = find_valid_socket(servinfo, false);

  if (sockfd == -1) {
    perror("sockfd");
    return;
  }
  
  int bytes_sent = send(sockfd, request, strlen(request), 0); 

  if(bytes_sent == -1) {
    perror("send");
  }

  if(recv(sockfd, response, 1024, 0) == -1) 
    perror("recv");


  close(sockfd);
}