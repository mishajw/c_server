#include "server.h"

#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const uint32_t BUFFER_SIZE = 32;

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

size_t get_message(const struct connection *connection, char **message) {
  *message = calloc(1, BUFFER_SIZE);
  char buffer[BUFFER_SIZE];
  size_t cumulative_size = 0;

  int recv_size = 0;
  while ((recv_size = recv(connection->client_fd, &buffer, BUFFER_SIZE, 0)) > 0) {
    // Resize the cumulative buffer to fit new data
    char *realloced_message = realloc(*message, cumulative_size + recv_size + 1);
    if (!realloced_message) {
      fprintf(stderr, "Can't realloc memory\n");
      exit(1);
    }
    *message = realloced_message;

    // Copy data into the cumulative buffer
    memcpy(&(*message)[cumulative_size], buffer, recv_size);
    // Set the new size
    cumulative_size += recv_size;

    if (recv_size < BUFFER_SIZE) {
      break;
    }
  }

  // Append NULL to message
  (*message)[cumulative_size - 1] = '\0';

  if (recv_size < 0) {
    perror("Failed reading message");
    exit(1);
  }

  return cumulative_size;
}

void send_message(const struct connection *connection, const char *message, const size_t message_length) {
  if (write(connection->client_fd, message, message_length) < 0) {
    perror("Couldn't send message");
    exit(1);
  }
}

void send_file(const struct connection *connection, int file_descriptor, off_t start, off_t end) {
  off_t total_size = end - start;
  if (sendfile(connection->client_fd, file_descriptor, &start, end) != total_size) {
    perror("Couldn't send file");
    exit(1);
  }
}

void destroy_server(struct server *server) {
  free(server);
}

void destroy_connection(struct connection *connection) {
  close(connection->client_fd);
  free(connection);
}

