#include <inttypes.h>
#include <stddef.h>

// Server being used to serve clients
struct server;

// A connection between a server and a single client
struct connection;

// Initialise a server on the port `port_number`
struct server* create_server(uint16_t port_number);

// Wait for a connection to be made the server, and return the connection struct for the new connection
struct connection* create_connection(struct server *server);

// Get the next message from a connection
size_t get_message(struct connection *connection, char *message, size_t message_size);

// Deallocate resources used by the server
void destroy_server(struct server *server);

// Deallocate resources used by the connection
void destroy_connection(struct connection *connection);

