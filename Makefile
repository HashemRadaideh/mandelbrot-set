# Project name and version
PROJECT = mandelbrot
VERSION = 0.0.1

# Define variables for the version, compiler and flags
CC = cc
CFLAGS = -std=c17 $(INCS) $(MACROS) $(WARNINGS)
LDFLAGS  = $(LIBS)

# Warnings and Macros
WARNINGS = -Wall -Wpedantic -Wextra -Wno-deprecated-declarations
MACROS   = -DVERSION=\"$(VERSION)\"

# Includes and Libs
INCS = -Iinclude/
LIBS = -lm -lpthread -ldl -lraylib -lrt -lX11

# Define the build, source and include directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release

# List of C source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# List header files
HEADERS = $(wildcard $(INC_DIR)/*.h)

# Locate object files
DEBUG_OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(DEBUG_DIR)/%.o)
RELEASE_OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(RELEASE_DIR)/%.o)

# Target binaries
DEBUG_TARGET = $(DEBUG_DIR)/$(PROJECT)
RELEASE_TARGET = $(RELEASE_DIR)/$(PROJECT)

all: release

options:
	@echo "Build options:"
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"
	@echo "CC       = $(CC)"

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(DEBUG_DIR)
	@echo "Compiling debug $@"
	$(CC) $(CFLAGS) -g -c $< -o $@
	@echo "Done."

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(RELEASE_DIR)
	@echo "Compiling release $@"
	$(CC) $(CFLAGS) -O3 -c $< -o $@
	@echo "Done."

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	@mkdir -p $(DEBUG_DIR)
	@echo "Linking debug $@"
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "Finished!"

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	@mkdir -p $(RELEASE_DIR)
	@echo "Linking release $@"
	$(CC) $(LDFLAGS) $^ -o $@
	@echo "Finished!"

debug: $(DEBUG_TARGET)

debugger: $(DEBUG_TARGET)
	@gdb $(DEBUG_TARGET)

release: $(RELEASE_TARGET)

run: $(RELEASE_TARGET)
	@$(RELEASE_TARGET)

clean:
	@rm -rf $(BUILD_DIR)

image:
	@docker build -t $(PROJECT) .

container:
	@docker run --rm -e DISPLAY=$(DISPLAY) -v /tmp/.X11-unix:/tmp/.X11-unix --shm-size=1024m -it $(PROJECT)

cmake-run:
	@cmake -B ./build && cmake --build ./build -t run

meson-run:
	@meson setup ./build && meson compile -C ./build run

.PHONY: all options debug debugger release run clean image container cmake-run meson-run
