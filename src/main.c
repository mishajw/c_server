#include <stdio.h>

#include "server.h"

int main(int argc, char *argv[]) {
  printf("Hello, world!\n");

  struct server *server = create_server(12612);
  printf("Made server\n");

  struct connection *connection = create_connection(server);
  printf("Made connection\n");

  char message[1000];
  if (get_message(connection, message, 1000) > 0) {
    printf("Got message: %s", message);
  } else {
    perror("Couldn't get message");
  }

  destroy_connection(connection);
  destroy_server(server);
}

