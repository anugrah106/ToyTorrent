NAME := root
CFLAGS ?= -Wall
LDFLAGS += -pthread
BUILD_DIR := ./build
SOURCES   := $(wildcard *.c)
OBJECTS   := $(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))
BINARIES  := ../$(NAME)_client.app

all: build_dir $(BINARIES)

build_dir:
	@mkdir -p $(BUILD_DIR)

../$(NAME)_client.app: $(OBJECTS)
	$(CC) -g $(LDFLAGS) $(OBJECTS) -o $@

build/%.o: %.c
	$(CC) -c $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	@rm -f $(BUILD_DIR)/*
	@rm -f $(BINARIES)
