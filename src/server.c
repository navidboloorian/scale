#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include "threading.h"
#include "http.h"

/**
 * Returns the socket file descriptor the server is bound to if successful and -1 if not.
 */
int find_valid_socket(struct addrinfo *sockets, struct addrinfo *selected_socket) {
  int sockfd;
  int yes = 1;

  for (selected_socket = sockets; selected_socket != NULL; selected_socket = selected_socket->ai_next) {
    sockfd = socket(selected_socket->ai_family, selected_socket->ai_socktype, selected_socket->ai_protocol);

    if (sockfd == -1) {
      perror("socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      return -1;
    }

    if (bind(sockfd, selected_socket->ai_addr, selected_socket->ai_addrlen) == -1) {
      perror("bind");
      continue;
    }

    break;
  }

  freeaddrinfo(sockets);

  if (selected_socket == NULL) {
    fprintf(stderr, "server failed to bind\n");
    return -1;
  }

  return sockfd;
}

void handle_connections(int sockfd, struct addrinfo *selected_socket, char *port) {
  struct sockaddr_storage incoming_addr;
  socklen_t incoming_addr_size;
  char buf[100];
  
  printf("server waiting for connections on port %s...\n", port);

  while(1) {
    incoming_addr_size = sizeof incoming_addr;

    int new_sockfd = accept(sockfd, (struct sockaddr *)&incoming_addr, &incoming_addr_size);

    if (new_sockfd == -1) {
      perror("accept");
      continue;
    }

    if ((recv(new_sockfd, buf, 99, 0)) == -1) {
      perror("recv");
      continue;
    }

    char *type = strtok(buf, " ");
    char *path = strtok(NULL, " ");
    char *protocol = strtok(NULL, " ");

    char *response;

    Request req = {
      .request_function = &parse_request,
      .path = path,
      .type = type,
      .protocol = protocol,
      .response = &response,
      .sockfd = new_sockfd
    };

    add_request(req);
  }
}

bool is_number(char string[]) {
  int string_length = strlen(string);

  int i = 0;

  for(; i < string_length; i++) {
    if (!isdigit(string[i])) {
      return false;
    }
  }

  return true;
}

int main(int argc, char **argv) {
  if (argc > 2) {
    fprintf(stderr, "usage: ./scale [port number]\n");
    return 0;
  }

  int port_num = 8080;

  if (argc == 2) {
    if (!is_number(argv[1])) {
      fprintf(stderr, "port must be a number\n");
      return 0;
    }

    port_num = atoi(argv[1]);

    if (port_num < 1024 || port_num > 65535) {
      fprintf(stderr, "port must be between 1024 and 65535\n");
      return 0;
    }
  }

  char port[100]; 
  sprintf(port, "%d", port_num);

  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *selected_socket;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(NULL, port, &hints, &servinfo) == -1) {
    perror("getaddrinfo");
    exit(1);
  }

  int sockfd = find_valid_socket(servinfo, selected_socket);

  if (sockfd == -1) {
    fprintf(stderr, "server could not find valid socket\n");
    exit(1);
  }

  if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  create_thread_pool();
  handle_connections(sockfd, selected_socket, port);
  destroy_thread_pool();

  return 0;
}