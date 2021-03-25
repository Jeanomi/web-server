all: test
    
clean:
    rm -rf *.o
    rm -rf server

test: test.o server.o
    gcc test.o server.o -o test

test.o: test.c
    gcc -c test.c -o test.o

server.o: server.c server.h
    gcc -c server.c -o server.o
