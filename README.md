# web-server
rebuild another server

Compile file.c for TCPServer reading from existing files:
  gcc -c file.c -o file.o
  
TCPServer.o:
  gcc TCPServer.c - o TCPServer.o
  
server:
  gcc file.o TCPServer.o -o server
  
To run server:
  ./server
  
client:
  gcc TCPClient.c -o client
  
To run client connecting to server:
  ./client
