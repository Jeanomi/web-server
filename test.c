/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>

#include "server.h"
#include "mime.h"
#include "file.h"

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./serverroot"

int send_response(int fd, char *header, char *content_type, void *body, int content_length){
    const int max_response_size = 65536;
    char response[max_response_size];

    time_t rawtime;
    struct tm *info;

    info = localtime(&rawtime);

    int response_length = sprintf(response, "%s\n"
                                            "Date: %s\n"
                                            "Connection: close\n"
                                            "Content-Length: %d\n"
                                            "Content-Type: %s\n"
                                            "\n",
                                  header, asctime(info), content_length, content_type);

    memcpy(response + response_length, body, content_length);

    int rv = send(fd, response, response_length + content_length, 0);

    if(rv < 0){
        perror("send");
    }

    return rv;
}

void resp_404(int fd){
    char filepath[4096];
    struct file_data *filedata;
    char *mime_type;

    //Fetch the 404.html file
    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    filedata = file_load(filepath);

    if(filedata == NULL){
        fprintf(stderr, "Cannot find system 404 file\n");
        _exit(3);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

void get_d20(int fd){
    srand(time(NULL) + getpid());

    char str[8];

    int random = rand() % 20 + 1;
    int length = sprintf(str, "%d\n", random);

    send_response(fd, "HTTP/1.1 200 OK", "text/plain", str, length);
}

void get_file(int fd, char *request_path){
    char file_path[65536];
    struct file_data *filedata;
    char *mime_type;

    snprintf(file_path, sizeof(file_path), "%s%s", SERVER_ROOT, request_path);
    filedata = file_load(file_path);

    if (filedata == NULL){
        snprintf(file_path, sizeof(file_path), "%s%s/index.hmtl", SERVER_ROOT,request_path);
        filedata = file_load(file_path);

        if(filedata == NULL){
            resp_404(fd);
            return;
        }
    }

    mime_type = mime_type_get(file_path);
    send_response(fd, "HTTP/1.1 200 OK", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

void handle_http_request(int fd){
    const int request_buffer_size = 65536;
    char request[request_buffer_size];
    char request_type[8];       //GET
    char request_path[1024];    //info
    char request_protocol[128]; //HTTP/1.1

    int bytes_recvd = recv(fd, request, request_buffer_size -1, 0);

    if(bytes_recvd < 0){
        perror("recv");
        return;
    }

    //NULL terminate request string
    request[bytes_recvd] = '\0';

    sscanf(request, "%s %s %s", request_type, request_path, request_protocol);
    printf("Request: %s %s %s", request_type, request_path, request_protocol);

    if(strcmp(request_type, "GET") == 0){
        if (strcmp(request_path, "/d20") == 0){
            get_d20(fd);
        }else{
            get_file(fd,request_path);
        }
    }else{
        fprintf(stderr, "Unknown request type \"%s\"\n", request_type);
        return;
    }
}

void launch(struct Server *svr){
    char buffer[2048];
    //char resourcePath[] = "index.html";
    //char resourceAddr[] = "127.0.0.1:4000";
    //char *hello = "HTTP/1.1 200 OK\n Date: Thurs, 18 Mar 2021 09:27:00 GMT+7\nServer: Apache/2.4.41 (Ubuntu)\nLast-Modified:Thurs 18 Mar 2021 10:00:00\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>Hello, World!</h1></body></html>";
    //char *hello = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1:4000\r\nContent-Type: text/plain\r\n\r\n";
    char *webpage = "GET /index.html HTTP/1.1\r\n\r\n";
//                  "Content-Type: text/html\r\n"
//                  "<!DOCTYPE html>\r\n"
//                  "<html><head><title>Home Page</title></head>\r\n"
//                  "<body><center><h1>Hello World!</h1><br>\r\n"
//                  "<img src=\"favicon.ico\"></center></body></html>\r\n";
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
//        write(client, webpage, strlen(webpage));
//        handle_http_request(client);

        close(client);
    }
}

int main(){
    struct Server svr = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 4000, 10, launch);
    svr.launch(&svr);
}
