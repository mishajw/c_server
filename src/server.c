#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "server.h"

void create_server(uint16_t port_number) {
  // Open the socket
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("Couldn't open server");
    exit(1);
  }

  int val = 1;
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&val, sizeof(val));

  // Set up server address
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_number);

  if (bind(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    perror("Couldn't bind socket");
    exit(1);
  }

  listen(sock_fd, 3);

  // Accept connection
  printf("Waiting for connection...\n");
  struct sockaddr_in client_addr;
  int client_addr_size = sizeof(client_addr);
  int client_sock = accept(sock_fd, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_size);
  if (client_sock < 0) {
    perror("Couldn't accept connection");
  }
  printf("Accepted connection\n");

  // Read message and send it back
  int read_size = 0;
  char client_message_buffer[2000];
  while ((read_size = recv(client_sock, client_message_buffer, 2000, 0)) > 0) {
    write(client_sock, client_message_buffer, strlen(client_message_buffer));
  }

  // Check for errors in read
  if (read_size == 0) {
    printf("Connection closed\n");
  } else if (read_size < 0) {
    perror("recv failed");
    exit(1);
  }
}

