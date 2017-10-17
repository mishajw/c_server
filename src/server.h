#include <inttypes.h>
#include <stddef.h>

struct server;
struct connection;

struct server* create_server(uint16_t port_number);

struct connection* create_connection(struct server *server);

size_t get_message(struct connection *connection, char *message, size_t message_size);

void destroy_server(struct server *server);

