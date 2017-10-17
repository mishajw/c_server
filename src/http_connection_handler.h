#include "server.h"

void handle_connection(struct connection *connection);

struct request_header *create_request_header(char *message);

