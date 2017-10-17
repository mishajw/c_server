struct arguments {
  int port_number;
  enum {
    SINGLE_THREADED,
    MULTI_THREADED
  } mode;
  char *root_directory;
}

struct arguments *create_arguments(int argc, char *argv[]);

void destroy_arguments(struct arguments *arguments);

