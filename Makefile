# KPaint Makefile for GBDK
# Path to GBDK
GBDK_PATH = /home/kurizu/Documents/GitHub/gbdk

# Compiler and linker
CC = $(GBDK_PATH)/bin/lcc
CFLAGS = -Wa-l -Wl-m -Wl-j -I$(GBDK_PATH)/include -I.

# Project directories
SRC_DIR = src
BUILD_DIR = build
ASSETS_DIR = assets

# ROM name
PROJECT_NAME = kpaint

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Output ROM
ROM = $(BUILD_DIR)/$(PROJECT_NAME).gb

# Default target
all: $(ROM)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the ROM
$(ROM): $(BUILD_DIR) $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o $(ROM) $(SRC_DIR)/main.c

# Clean the build directory
clean:
	rm -rf $(BUILD_DIR)

# Run the ROM in BGB emulator (if you have it installed)
run:
	bgb $(ROM)

.PHONY: all clean run
