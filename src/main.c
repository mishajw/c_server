#include <stdio.h>

#include "server.h"
#include "server_handler.h"
#include "http_connection_handler.h"

int main(int argc, char *argv[]) {
  struct server *server = create_server(12612);
  printf("Made server\n");

  handle_multi_threaded(server, &handle_connection);

  destroy_server(server);
}

