#include "http_connection_handler.h"

#include <stdio.h>

void handle_connection(struct connection *connection) {
  char message[1000];
  if (get_message(connection, message, 1000) > 0) {
    printf("Got message: %s\n", message);
  } else {
    perror("Couldn't get message");
  }

  fflush(stdout);
}

