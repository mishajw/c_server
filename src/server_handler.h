#include "server.h"

void handle_multi_threaded(struct server *server, void (*callback)(struct connection*));

void handle_single_threaded(struct server *server, void (*callback)(struct connection*));

