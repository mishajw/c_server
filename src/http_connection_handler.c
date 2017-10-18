#include "http_connection_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *DEFAULT_PATH = "/index.html";

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

  switch (request_header->type) {
    case GET:
      handle_get_request(connection, request_header);
      break;
    default:
      fprintf(stderr, "No support for request type\n");
      break;
  }

  destroy_connection(connection);
}

struct request_header *create_request_header(char *message) {
  const char *line_delimiter = "\n";
  const char *word_delimiter = " ";

  char *strtok_saveptr;
  char *request_line = strtok_r(message, line_delimiter, &strtok_saveptr);

  char *request_line_strtok_saveptr;
  const char *request_type = strtok_r(request_line, word_delimiter, &request_line_strtok_saveptr);
  const char *path = strtok_r(NULL, word_delimiter, &request_line_strtok_saveptr);
  const char *version = strtok_r(NULL, word_delimiter, &request_line_strtok_saveptr);

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

void handle_get_request(struct connection *connection, struct request_header *request_header) {
  const char *local_path = request_header->path;

  // If "/" requested, redirect to "index.html"
  if (strcmp(local_path, "/") == 0) {
    local_path = DEFAULT_PATH;
  }

  char buffer[1024];
  char *absolute_path = getcwd(buffer, sizeof(buffer));
  strcat(absolute_path, local_path);

  // Check if the file exists and if we can read it
  if (access(absolute_path, F_OK) != 0) {
    fprintf(stderr, "Couldn't find file %s\n", absolute_path);
    return;
  }

  // Open the file
  FILE *file = fopen(absolute_path, "r");

  // Get the stats for the size field
  struct stat stat_results;
  stat(absolute_path, &stat_results);

  // Read the file into memeory
  char file_buffer[stat_results.st_size];
  fread(file_buffer, stat_results.st_size, sizeof(char), file);

  const char *response_header = "HTTP/1.1 200 OK\n\n";
  
  size_t full_response_length = strlen(response_header) + stat_results.st_size;
  char full_response[full_response_length];
  strcpy(full_response, response_header);
  strcat(full_response, file_buffer);

  // Send the file to the client
  send_message(connection, full_response, full_response_length);
}

