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

#define BUFFER_SIZE 65536
#define SA struct sockaddr
#define PORT 8080
#define SERVER_ROOT "./serverroot"
#define SERVER_FILES "./serverfiles"


void handle_request(int sockfd);
void get_file(int fd, char *request_path);
//void send_response(int fd, char *header,void *body, char *content_type, int content_length, char *file_path);
void send_response(int fd, char *header, char *content_type, int content_length, char *file_path);
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
}


void send_response(int fd, char *header, char *content_type, int content_length, char *file_path){
    printf("2.0\n");
//    char *response;
    char response_header[BUFFER_SIZE];
    //char *ptr, *ptr2;

    //int total_length;
    FILE *f = fopen(file_path, "rb");

    printf("2.1\n");

    if(f == NULL){
        printf("File open error\n");
    }

    int response_header_length = sprintf(response_header, "%s\n"
                                            "Connection: close\n"
                                            "Content-Type: %s\n"
                                            "Content_Length: %d\n"
                                            "\n",
                                            header, content_type, content_length);

//    response = calloc(response_header_length + content_length + 1, sizeof(char));
    printf("2.2\n");
//    memcpy(response, response_header, response_header_length);
//    memcpy(response + response_header_length, body, content_length);
    write(fd, response_header, response_header_length);
    printf("2.3\n");

//    ptr = response;
//    ptr2 = response;
//    int rv = write(fd, response, response_header_length + content_length);

//    total_length = response_header_length + content_length;
//    part_size = total_length / 4000;

    while (1){
        unsigned char buff[1024] = {0};
        int nread = fread(buff, 1, 1024, f);

        if(nread > 0){
            write(fd, buff, nread);
        }
        if(nread < 1024){
            if(feof(f)){
                printf("End of file\n");
            }
            if(ferror(f)){
                printf("Error reading\n");
            }
            break;
        }
    }
/*
    while (total_length > 0) {
        if (total_length < 4000){
//            memcpy(ptr2, ptr, 1500);
            memcpy(buff, response, total_length);
            int rv = write(fd, buff, strlen(buff));
            if (rv < 0) {
                perror("send");
            }
//            int rf = fputs(ptr2, f);
//            if(rf < 0){
//                perror("write");
//            }
//            ptr2 = ptr;
            total_length -= total_length;
        }else {
//            memcpy(ptr2, ptr, 4000);
            //int rv = send(fd, ptr, 1500, 0);
            while ( part_size >= 0) {
                memcpy(buff, ptr, total_length);
                int rv = write(fd, buff, strlen(buff));
                if (rv < 0) {
                    perror("send");
                }
//            int rf = fputs(ptr2, f);
//            if(rf < 0){
//                perror("write");
//            }
                *ptr = *(ptr + rv);
                total_length -= rv;
                part_size--;
            }
        }
    }
    */

    printf("2.4\n");
//    if (rv < 0) {
//        perror("send");
//    }
//    free(response);
    fclose(f);
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
    send_response(fd, "HTTP/1.1 200 OK", mime_type, fileData->size, filepath);

    printf("3\n");
    file_free(fileData);
}

void handle_request(int sockfd){
    char request[BUFFER_SIZE];
    char request_type[8];       //GET
    char request_path[1024];    //info
    char request_protocol[128]; //HTTP/1.1

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

    send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, fileData->size, filepath);

    file_free(fileData);
}