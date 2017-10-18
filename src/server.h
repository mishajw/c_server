#include <inttypes.h>
#include <stddef.h>

// Server being used to serve clients
struct server;

// A connection between a server and a single client
struct connection;

// Initialise a server on the port `port_number`
struct server* create_server(const uint16_t port_number);

// Wait for a connection to be made the server, and return the connection struct for the new connection
struct connection* create_connection(const struct server *server);

// Get the next message from a connection
size_t get_message(const struct connection *connection, char **message);

// Send a message to the other end of a connection
void send_message(const struct connection *connection, const char *message, const size_t message_length);

// Send a file to the client
void send_file(const struct connection *connection, int file_descriptor);

// Deallocate resources used by the server
void destroy_server(struct server *server);

// Deallocate resources used by the connection
void destroy_connection(struct connection *connection);

