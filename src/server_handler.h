#include "server.h"

void handle_multi_threaded(const struct server *server, const void (*callback)(struct connection*));

void handle_single_threaded(const struct server *server, const void (*callback)(struct connection*));

