//
// Created by cuong on 27/03/2021.
//

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <fcntl.h>
//#include <sys/sendfile.h>
#include "file.h"
#include "mime.h"

#define BUFFER_SIZE 2000000
#define SA struct sockaddr
#define PORT 8080
#define SERVER_ROOT "./serverroot"
#define SERVER_FILES "./serverfiles"


void handle_request(int sockfd);
void get_file(int fd, char *request_path);
void send_response(int fd, char *header,void *body, char *content_type, int content_length);
void send_resp_404(int fd);

int main(){
    int server_fd, conn_fd, len;
    struct sockaddr_in servaddr, cli;

    server_fd = socket(AF_INET, SOCK_STREAM,0);
    if (server_fd == -1){
        printf("Socket creation failed...\n");
        exit(0);
    }else{
        printf("Successfully created socket...\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        printf("Socket bind failed...\n");
        exit(0);
    }else{
        printf("Socket successfully binded...\n");
    }

    if (listen(server_fd, 5) != 0){
        printf("Listen failed...\n");
        exit(0);
    }else{
        printf("Server listening...\n");
    }
    len = sizeof(cli);

    while (1) {
        conn_fd = accept(server_fd, (struct sockaddr *) &cli, (socklen_t *) &len);

        if (conn_fd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } else {
            printf("Server accept client...\n");
        }
        if (!fork()) {
            handle_request(conn_fd);
        }
        close(conn_fd);
    }

    //close(server_fd);
    return 0;
}


void send_response(int fd, char *header, void *body, char *content_type, int content_length){
    printf("2.0\n");
    char response[BUFFER_SIZE];
    printf("2.1\n");
    int response_length = sprintf(response, "%s\n"
                                            "Connection: close\n"
                                            "Content-Type: %s\n"
                                            "Content_Length: %d\n"
                                            "\n",
                                            header, content_type, content_length);
    printf("2.2\n");
    memcpy(response + response_length, body, content_length);
    printf("2.3\n");
    int rv = write(fd, response, response_length + content_length);
    printf("2.4\n");
    if (rv < 0){
        perror("send");
    }

    //return rv;
}

void get_file(int fd, char *request_path){
    char filepath[BUFFER_SIZE];
    struct file_data * fileData;
    char *mime_type;

    snprintf(filepath, sizeof(filepath), "%s%s", SERVER_ROOT, request_path);
    fileData = file_load(filepath);

//    if (fileData == NULL){
//        snprintf(filepath, sizeof(filepath), "%s/index.html", SERVER_ROOT);
//    }
    mime_type = mime_type_get(filepath);

    if (fileData == NULL){
        send_resp_404(fd);
        return;
    }
    printf("2\n");
    send_response(fd, "HTTP/1.1 200 OK", fileData->data, mime_type, fileData->size);

    printf("3\n");
    file_free(fileData);
}

void handle_request(int sockfd){
    char request[BUFFER_SIZE];
    char request_type[8];       //GET
    char request_path[1024];    //info
    char request_protocol[128]; //HTTP/1.1

//    char response[BUFFER_SIZE];
//    int response_length = sprintf(response, "HTTP/1.1 200 OK\n"
//                                            "Connection: close\n"
//                                            "Content-Type: text/html\n"
//                                            "Content_Length: 161\n"
//                                            "\n");

    int bytes_recv = recv(sockfd, request, BUFFER_SIZE - 1, 0);

    if(bytes_recv < 0){
        perror("recv");
        return;
    }

    request[bytes_recv] = '\0';

    sscanf(request, "%s %s %s", request_type, request_path, request_protocol);
    printf("Request: %s %s %s\n", request_type, request_path, request_protocol);

    printf("1\n");
    get_file(sockfd, request_path);
    //if (strcmp(request_type == "GET") == 0) {

    //}

//    for (;;){
//        bzero(buff, sizeof(buff));
//
//        //read message from client
//        read(sockfd, buff, sizeof(buff));
//
//        bzero(buff, sizeof(buff));
//        n = 0;
//        //copy server message in the buffer
//        while ((buff[n++] = getchar()) != '\n')
//            ;
//        //send the buffer to client
//        write(sockfd, buff, sizeof(buff));
//
//        //exit to end chat
//        if((strncmp(buff, "exit", 4)) == 0){
//            printf("Server Exit ...\n");
//            break;
//        }
//    }
}

void send_resp_404(int fd){
    char filepath[BUFFER_SIZE];
    struct file_data * fileData;
    char *mime_type;

    snprintf(filepath, sizeof(filepath), "%s/404.html", SERVER_FILES);
    fileData = file_load(filepath);

    if (fileData == NULL) {
        fprintf(stderr, "Cannot find system 404 file\n");
        exit(3);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 404 NOT FOUND", fileData->data, mime_type, fileData->size);

    file_free(fileData);
}