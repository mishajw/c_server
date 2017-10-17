#include "server_handler.h"
#include "server.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handle_multi_threaded(struct server *server, void (*callback)(struct connection*)) {
  while (1) {
    printf("Waiting for next connection\n");
    struct connection *connection = create_connection(server);

    pthread_t current_thread;
    if (pthread_create(&current_thread, NULL, (void * (*)(void *))callback, connection) != 0) {
      perror("Couldn't create thread");
      exit(1);
    } else {
      printf("Successfully created thread\n");
    }

    destroy_connection(connection);
  }
}

void handle_single_threaded(struct server *server, void (*callback)(struct connection*)) {
  while (1) {
    struct connection *connection = create_connection(server);
    callback(connection);
  }
}

