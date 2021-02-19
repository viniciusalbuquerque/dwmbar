# Makefile

APP_NAME = dwmbar
CC = $(CROSS_TOOL)gcc
CFLAGS = -Wall -g -Werror -pthread
DIRS = generated bin

SRC_DIR = $(CURDIR)/src
OBJ_DIR = $(CURDIR)/generated
BIN_DIR = $(CURDIR)/bin

all: $(APP_NAME)

$(APP_NAME): $(OBJ_DIR)/dwmbar.o $(OBJ_DIR)/program.o
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$@ 

$(OBJ_DIR)/dwmbar.o: $(SRC_DIR)/dwmbar.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ_DIR)/program.o: $(SRC_DIR)/program.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

run:
	$(BIN_DIR)/$(APP_NAME)
