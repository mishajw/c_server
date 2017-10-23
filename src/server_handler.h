#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "server.h"

void handle_multi_threaded(const struct server *server, void (*const callback)(struct connection*));

void handle_single_threaded(const struct server *server, const void (*callback)(struct connection*));

#endif

