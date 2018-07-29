NAME := tracker
CXXLAGS ?= -Wall 
CXXFLAGS += --std=c++11 -I.. 
LDFLAGS += -pthread
BUILD_DIR := ./build
SOURCES   := $(wildcard *.cpp)
OBJECTS   := $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
BINARIES  := ../$(NAME).app

all: $(BINARIES)

build_dir:
	@mkdir -p build

../$(NAME).app: build_dir $(OBJECTS)
	$(CXX) -g $(OBJECTS) -o $@ $(LDFLAGS)

build/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(BINARIES)

.PHONY: build_dir
