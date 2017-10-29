TARGET_NAME = c_server

SRC = .
BIN = ../bin
TARGET_PATH = $(BIN)/$(TARGET_NAME)

SRCS := $(shell find $(SRC) -type f -name "*.c")
INCS := $(shell find $(SRC) -type f -name "*.h")

$(BIN):
	@mkdir -p $(BIN)

.PHONY: build
build: $(TARGET_PATH)

$(TARGET_PATH): $(SRCS) $(INCS) $(BIN)
	gcc -O2 -g -Wall -Wpedantic -pthread -o $(TARGET_PATH) $(SRCS) $(INCS)

.PHONY: run
run: $(TARGET_PATH)
	@$(TARGET_PATH)

.PHONY: clean
clean:
	@rm -rf $(BIN)

.PHONY: all
all: $(TARGET_PATH)

