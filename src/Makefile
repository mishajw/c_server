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
	gcc -Wall -o $(TARGET_PATH) $(SRCS) $(INCS)

run: $(TARGET_PATH)
	@$(TARGET_PATH)

clean:
	@rm -rf $(BIN)

