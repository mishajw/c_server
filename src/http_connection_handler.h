#include "server.h"

void handle_connection(struct connection *connection);

struct request_header *create_request_header(char *message);

void handle_get_request(struct connection *connection, struct request_header *request_header);

