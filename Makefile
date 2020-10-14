# Makefile

APP_NAME = dwmbar
CC = $(CROSS_TOOL)gcc
CFLAGS = -Wall -g -Werror -pthread
DIRS = generated bin
SRC_DIR = $(CURDIR)/src
OBJ_DIR = $(CURDIR)/generated
BIN_DIR = $(CURDIR)/bin

all: $(APP_NAME)

$(APP_NAME): $(OBJ_DIR)/*.o
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(BIN_DIR)/$(APP_NAME)
