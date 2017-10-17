#include "http_connection_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct request_header {
  enum {
    GET, POST, HEAD
  } type;
  char *path;
  char *version;
};

void handle_connection(struct connection *connection) {
  char *message = NULL;
  if (get_message(connection, &message) < 0) {
    perror("Couldn't get message");
    exit(1);
  }

  struct request_header *request_header = create_request_header(message);
  free(message);

  destroy_connection(connection);
}

struct request_header *create_request_header(char *message) {
  char *strtok_saveptr;
  const char *delimiter = " ";

  const char *request_type = strtok_r(message, delimiter, &strtok_saveptr);
  const char *path = strtok_r(NULL, delimiter, &strtok_saveptr);
  const char *version = strtok_r(NULL, delimiter, &strtok_saveptr);

  struct request_header *request_header = calloc(1, sizeof(struct request_header));  

  if (strcmp(request_type, "GET") == 0) {
    request_header->type = GET;
  } else if (strcmp(request_type, "POST") == 0) {
    request_header->type = POST;
  } else if (strcmp(request_type, "HEAD") == 0) {
    request_header->type = HEAD;
  } else {
    fprintf(stderr, "Couldn't parse header type");
    return NULL;
  }

  // TODO: DRY
  size_t path_length = strlen(path);
  request_header->path = calloc(1, path_length);
  memcpy(request_header->path, path, path_length);

  size_t version_length = strlen(version);
  request_header->version = calloc(1, version_length);
  memcpy(request_header->version, version, version_length);

  return request_header;
}

