CC=gcc
CFLAGS=-Wall -Wextra

OBJS=server.o file.o mime.o hashtable.o llist.o

all: test

test: $(OBJS)
	gcc -o $@ $^

test.o: test.c

server.o: server.c server.h

file.o: file.c file.h

mime.o: mime.c mime.h

hashtable.o: hashtable.c hashtable.h

llist.o: llist.c llist.h