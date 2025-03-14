#include <stdlib.h>
#include <stdio.h> 
#include "socket.h"

/**
 * Returns the socket file descriptor the server is bound to if successful and -1 if not.
 */
int find_valid_socket(struct addrinfo *sockets, bool is_binding) {
  int sockfd;
  int yes = 1;

  struct addrinfo *selected_socket;

  for (selected_socket = sockets; selected_socket != NULL; selected_socket = selected_socket->ai_next) {
    sockfd = socket(selected_socket->ai_family, selected_socket->ai_socktype, selected_socket->ai_protocol);

    if (sockfd == -1) {
      perror("socket");
      continue;
    }

    if (is_binding) {
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return -1;
      }
      
      if (bind(sockfd, selected_socket->ai_addr, selected_socket->ai_addrlen) == -1) {
        perror("bind");
        continue;
      }
    }
    else {
      if (connect(sockfd, selected_socket->ai_addr, selected_socket->ai_addrlen) == -1) {
        perror("connect");
        continue;
      }
    }

    break;
  }

  freeaddrinfo(sockets);

  if (is_binding && selected_socket == NULL) {
    fprintf(stderr, "server failed to bind\n");
    return -1;
  } 
  
  if (!is_binding && selected_socket == NULL) {
    fprintf(stderr, "server failed to connect\n");
    return -1;
  }

  return sockfd;
}
