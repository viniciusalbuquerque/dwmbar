# Makefile

TARGETS = dwmbar

CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -Werror -pthread#-std=c99 

all: clean $(TARGETS)

$(TARGETS):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(TARGETS)
