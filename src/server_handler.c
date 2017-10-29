#include "server_handler.h"
#include "server.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle_multi_threaded(const struct server *server, void (*const callback)(struct connection*)) {
  while (1) {
    // Wait for next connection
    struct connection *connection = create_connection(server);

    // When we've got a connection, start a new thread to handle it
    pthread_t current_thread;
    if (pthread_create(&current_thread, NULL, (void * (*)(void *))callback, connection) != 0) {
      perror("Couldn't create thread");
      exit(1);
    }
  }
}

void handle_single_threaded(const struct server *server, void (*const callback)(struct connection*)) {
  while (1) {
    // Wait for a connection and handle it in this thread
    struct connection *connection = create_connection(server);
    callback(connection);
  }
}

