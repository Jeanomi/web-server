/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "server.h"
#include "mime.h"

void launch(struct Server *svr){
    char buffer[2048];
    char file[30] = "index.html";
    //char resourcePath[] = "index.html";
    //char resourceAddr[] = "127.0.0.1:4000";
    //char *hello = "HTTP/1.1 200 OK\n Date: Thurs, 18 Mar 2021 09:27:00 GMT+7\nServer: Apache/2.4.41 (Ubuntu)\nLast-Modified:Thurs 18 Mar 2021 10:00:00\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>Hello, World!</h1></body></html>";
    //char *hello = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:4000\r\nContent-Type: text/plain\r\n\r\n";
    char *webpage = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html\r\n\r\n"
                  "<!DOCTYPE html>\r\n"
                  "<html><head><title>Home Page</title></head>\r\n"
                  "<body><center><h1>Hello World!</h1><br>\r\n"
                  "<img src=\"favicon.ico\"></center></body></html>\r\n";
    int addr_len = sizeof(svr->address);
    int client;
    int img;
    while(1){
        printf("====WAITING FOR CONNECTION====\n");
        client = accept(svr->socket, (struct sockaddr *)&svr->address, (socklen_t *)&addr_len);

        if(!fork()){
            memset(buffer, 0, 2048);
            read(client, buffer, 2047);
            printf("%s\n", buffer);

            if(!strncmp(buffer, "GET /index.html", 16)){
                img = open("index.html", O_RDONLY);
                char* header = "HTTP/1.1 200 OK\n"
                                "Content-Type: text/html\n"
                                "Content-Length: 161\n"
                                "\n";
                write(client, header, strlen(header));
                sendfile(client, img, NULL, 161);
                close(img);
            }else{
                write(client, webpage, strlen(webpage));
            }
        }

        close(client);
        printf("Closing ...\n");
    }
}

int main(){
    struct Server svr = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 4000, 10, launch);
    svr.launch(&svr);
}
