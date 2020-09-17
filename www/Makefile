CC = gcc
CFLAGS = -O2 -fomit-frame-pointer -Wall -W -DNO_MAIN -DWITH_CHECKS=1
LDFLAGS = -s

all: mkhtml


mkhtml: mkhtml.o mktbl.o unitable.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

mktbl.o: mktbl.c mktbl.h country.h unitable.h
mkhtml.o: mkhtml.c mktbl.h unitable.h
unitable.o: unitable.c unitable.h

clean::
	$(RM) mkhtml *.o
