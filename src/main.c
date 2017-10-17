#include <stdio.h>

#include "server.h"
#include "server_handler.h"

void connection_callback(struct connection *connection) {
  char message[1000];
  if (get_message(connection, message, 1000) > 0) {
    printf("Got message: %s", message);
  } else {
    perror("Couldn't get message");
  }
}

int main(int argc, char *argv[]) {
  printf("Hello, world!\n");

  struct server *server = create_server(12612);
  printf("Made server\n");

  handle_multi_threaded(server, &connection_callback);

  destroy_server(server);
}

