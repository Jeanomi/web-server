CC=gcc
CFLAGS=-Wall -Wextra

OBJS=TCPServer.o file.o mime.o

all: server

server: $(OBJS)
	gcc -o $@ $^

TCPServer.o: TCPServer.c

file.o: file.c file.h

mime.o: mime.c mime.h



