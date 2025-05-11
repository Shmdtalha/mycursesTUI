CC=gcc
NCURSES_PATH=$(shell pwd)/build/install
CFLAGS=-I$(NCURSES_PATH)/include
LDFLAGS=-L$(NCURSES_PATH)/lib -Wl,-rpath,$(NCURSES_PATH)/lib
LDLIBS=-lform -lncurses

all: test

test: test.c
	$(CC) $(CFLAGS) -o test test.c $(LDFLAGS) $(LDLIBS)

clean:
	rm -f test

.PHONY: all clean