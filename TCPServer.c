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
#include <fcntl.h>
#include <sys/sendfile.h>
#define BUFFER_SIZE 65536
#define SA struct sockaddr
#define PORT 8080


void handle_request(int sockfd){
    char request[BUFFER_SIZE];
    char request_type[8];       //GET
    char request_path[1024];    //info
    char request_protocol[128]; //HTTP/1.1

    char response[BUFFER_SIZE];
    int response_length = sprintf(response, "HTTP/1.1 200 OK\n"
                                            "Connection: close\n"
                                            "Content-Type: image/jpeg\n"
                                            "Content_Length: 6051\n"
                                            "\n");
    int img;

    int bytes_recv = recv(sockfd, request, BUFFER_SIZE - 1, 0);

    if(bytes_recv < 0){
        perror("recv");
        return;
    }

    request[bytes_recv] = '\0';

    sscanf(request, "%s %s %s", request_type, request_path, request_protocol);
    printf("Request: %s %s %s\n", request_type, request_path, request_protocol);

    img = open(request_path, O_RDONLY);
    write(sockfd, response, response_length);
    sendfile(sockfd, img, NULL, 6051);
    close(img);

    close(sockfd);

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


    }

    close(server_fd);
    return 0;
}