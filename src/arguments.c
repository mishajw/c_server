#include "arguments.h"

struct arguments *create_arguments(int argc, char *argv[]) {
  struct arguments *arguments = calloc(1, sizeof(arguments));

  for (size_t i; i < argc; i++) {
    if (strcmp("--port-number", argv[i])) {
      if (i == argc - 1) {
        printf("--port-number needs argument");
      }

      value = atoi(argv[++i]);
      arguments.port_number = value;
    }
  }
}

void destroy_arguments(struct arguments *arguments) {
  free(arguments);
}

