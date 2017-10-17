#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "server.h"

struct server {
  int socket_fd;
  struct sockaddr_in server_addr;
};

struct connection {
  int client_fd;
  struct sockaddr_in client_addr;
};

struct server* create_server(const uint16_t port_number) {
  struct server *server = calloc(1, sizeof(struct server));

  // Open the socket
  server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server->socket_fd < 0) {
    perror("Couldn't open server");
    exit(1);
  }

  int val = 1;
  setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&val, sizeof(val));

  // Set up server address
  server->server_addr.sin_family = AF_INET;
  server->server_addr.sin_addr.s_addr = INADDR_ANY;
  server->server_addr.sin_port = htons(port_number);

  if (bind(server->socket_fd, (struct sockaddr*) &server->server_addr, sizeof(server->server_addr)) < 0) {
    perror("Couldn't bind socket");
    exit(1);
  }

  listen(server->socket_fd, 3);

  return server;
}

struct connection* create_connection(const struct server *server) {
  struct connection *connection = calloc(1, sizeof(struct connection));

  int client_addr_size = sizeof(connection->client_addr);

  connection->client_fd = accept(
      server->socket_fd, (struct sockaddr*) &connection->client_addr, (socklen_t*) &client_addr_size);

  if (connection->client_fd < 0) {
    perror("Couldn't accept connection");
    exit(1);
  }

  return connection;
}

size_t get_message(const struct connection *connection, char *message, const size_t message_size) {
  return recv(connection->client_fd, message, message_size, 0);
}

void destroy_server(struct server *server) {
  free(server);
}

void destroy_connection(struct connection *connection) {
  free(connection);
}

