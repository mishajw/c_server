#include "http_connection_handler.h"

#include <stdio.h>

void handle_connection(struct connection *connection) {
  char *message = NULL;
  if (get_message(connection, &message) > 0) {
    printf("Got message %s\n", message);
  } else {
    perror("Couldn't get message");
  }

  destroy_connection(connection);
}

