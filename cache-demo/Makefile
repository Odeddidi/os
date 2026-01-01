#* 211913587 oded didi *#
CC = gcc
CFLAGS = -Wall -Wextra -g

all: cache

cache: cache.o
	$(CC) $(CFLAGS) -o cache cache.o

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c

clean:
	rm -f cache cache.o
