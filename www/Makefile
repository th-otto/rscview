CC = gcc
CFLAGS = -O2 -fomit-frame-pointer -Wall -W
LDFLAGS = -s

all: mkhtml


mkhtml: mkhtml.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
