#include "http_connection_handler.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *DEFAULT_PATH = "/index.html";
static const char *HTTP_VERSION = "HTTP/1.1";

struct byte_range {
  off_t start;
  off_t end;
};

struct request_header {
  enum {
    GET, POST, HEAD
  } type;
  char *path;
  char *version;
  struct byte_range *byte_range;
};

// Create a `request_header` from the message from the clinet
struct request_header *create_request_header(char *message);

// Handle a GET request from a connection
void handle_request_header(struct connection *connection, struct request_header *request_header);

// Get the absolute path from a request header, defaults to index.html if empty path
void get_path_from_request_header(struct request_header *request_header, char *path, size_t path_size);

// Send a response header to the connection
void send_response_header(struct connection *connection, int response_code, bool include_newlines);

// Parse the byte range field from a request
struct byte_range *parse_byte_range(char *byte_range_string);

// Free resources used by a `request_header`
void destroy_request_header(struct request_header *request_header);

void handle_connection(struct connection *connection) {
  char *message = NULL;
  get_message(connection, &message);
  struct request_header *request_header = create_request_header(message);
  free(message);

  if (!request_header) {
    send_response_header(connection, 400, true);
    goto CLEANUP;
  }

  printf(
      "Got request header with type %d, version %s, and path %s\n",
      request_header->type,
      request_header->version,
      request_header->path);

  if (strcmp(request_header->version, HTTP_VERSION) != 0) {
    send_response_header(connection, 505, true);
    goto CLEANUP;
  }

  handle_request_header(connection, request_header);

CLEANUP:
  destroy_request_header(request_header);
  destroy_connection(connection);
}

struct request_header *create_request_header(char *message) {
  const char *line_delimiter = "\n\r";
  const char *word_delimiter = " ";

  char *strtok_saveptr;
  char *request_line = strtok_r(message, line_delimiter, &strtok_saveptr);

  if (!request_line) {
    fprintf(stderr, "Couldn't get request header from message: %s\n", message);
    return NULL;
  }

  char *request_line_strtok_saveptr;
  const char *request_type = strtok_r(request_line, word_delimiter, &request_line_strtok_saveptr);
  const char *path = strtok_r(NULL, word_delimiter, &request_line_strtok_saveptr);
  const char *version = strtok_r(NULL, word_delimiter, &request_line_strtok_saveptr);

  if (!request_type || !path || !version) {
    fprintf(stderr, "Couldn't parse request header: %s\n", request_line);
    return NULL;
  }

  struct request_header *request_header = calloc(1, sizeof(struct request_header));  

  if (strcmp(request_type, "GET") == 0) {
    request_header->type = GET;
  } else if (strcmp(request_type, "POST") == 0) {
    request_header->type = POST;
  } else if (strcmp(request_type, "HEAD") == 0) {
    request_header->type = HEAD;
  } else {
    fprintf(stderr, "Couldn't parse header type\n");
    free(request_header);
    return NULL;
  }

  // TODO: DRY
  size_t path_length = strlen(path);
  request_header->path = calloc(1, path_length + 1);
  memcpy(request_header->path, path, path_length);

  size_t version_length = strlen(version);
  request_header->version = calloc(1, version_length + 1);
  memcpy(request_header->version, version, version_length);

  while (true) {
    const char *current_line = strtok_r(NULL, line_delimiter, &strtok_saveptr);

    if (!current_line) {
      break;
    }

    char *colon = strchr(current_line, ':');

    if (!colon || strlen(colon) < 2 || colon[0] != ':' || colon[1] != ' ') {
      continue;
    }

    char *parameter = colon + 2;
    colon[0] = '\0';
    const char *parameter_name = current_line;

    if (strcmp(parameter_name, "Range") == 0) {
      struct byte_range *byte_range = parse_byte_range(parameter);
      request_header->byte_range = byte_range;
    }
  }

  return request_header;
}

void handle_request_header(struct connection *connection, struct request_header *request_header) {
  // TODO: Remove magic number for path size
  char absolute_path[1024];
  get_path_from_request_header(request_header, absolute_path, 1024);

  // Check if the file exists and if we can read it
  if (access(absolute_path, F_OK) != 0) {
    send_response_header(connection, 404, true);
    return;
  }

  // Send the header to the client
  send_response_header(connection, 200, true);

  // Open the file for stats and reading
  int file = open(absolute_path, O_RDONLY);

  // Get the stats for the size field
  struct stat stat_results;
  if (fstat(file, &stat_results) != 0) {
    perror("Couldn't stat file");
    exit(1);
  }
  off_t file_size = stat_results.st_size;

  if (request_header->type != HEAD) {
    // Send the file to the client
    if (!request_header->byte_range) {
      send_file(connection, file, 0, file_size);
    } else {
      if (request_header->byte_range->end > file_size) {
        send_response_header(connection, 400, true);
        return;
      }

      send_file(
          connection,
          file,
          request_header->byte_range->start,
          request_header->byte_range->end);
    }
  }
}

void get_path_from_request_header(struct request_header *request_header, char *path, size_t path_size) {
  getcwd(path, path_size);

  if (strcmp(request_header->path, "/") == 0) {
    strcat(path, DEFAULT_PATH);
  } else {
    strcat(path, request_header->path);
  }
}

void send_response_header(struct connection *connection, int response_code, bool include_newlines) {
  char *response_string = NULL;

  switch (response_code) {
    case 200:
      response_string = "OK";
      break;
    case 400:
      response_string = "Bad Request";
      break;
    case 404:
      response_string = "Not Found";
      break;
    case 505:
      response_string = "HTTP Version Not Supported";
      break;
    default:
      fprintf(stderr, "Couldn't recognise response code, setting response string to \"Unknown code\"");
      response_string = "Unknown code";
      break;
  }

  // +3 for response code, +2 for spaces, +2 for possible newlines, +1 for null byte
  char response_header[strlen(HTTP_VERSION) + strlen(response_string) + 8];
  char *format_string;
  if (include_newlines) {
    format_string = "%s %d %s\n\n";
  } else {
    format_string = "%s %d %s\n";
  }
  sprintf(response_header, format_string, HTTP_VERSION, response_code, response_string);

  printf("Sending response: \"%s\"\n", response_header);
  send_message(connection, response_header, strlen(response_header));
}

// Parse the byte range field from a request
struct byte_range *parse_byte_range(char *byte_range_string) {
  struct byte_range *byte_range = calloc(1, sizeof(struct byte_range));

  const char *expected_beginning = "bytes=";

  if (strlen(byte_range_string) < strlen(expected_beginning)
      || strncmp(byte_range_string, expected_beginning, strlen(expected_beginning) - 1) != 0) {

    fprintf(stderr, "Couldn't parse byte range: %s\n", byte_range_string);
    free(byte_range);
    return NULL;
  }

  char *range_string = byte_range_string + strlen(expected_beginning);

  char *strtok_saveptr;
  const char *start = strtok_r(range_string, "-", &strtok_saveptr);
  const char *end = strtok_r(NULL, "-", &strtok_saveptr);

  byte_range->start = atoi(start);
  byte_range->end = atoi(end);

  printf(
      "byte range %ld to %ld\n",
      byte_range->start,
      byte_range->end);

  return byte_range;
}

void destroy_request_header(struct request_header *request_header) {
  if (!request_header) {
    return;
  }

  free(request_header->path);
  free(request_header->version);

  if (request_header->byte_range) {
    free(request_header->byte_range);
  }

  free(request_header);
}

