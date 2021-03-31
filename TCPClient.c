//
// Created by cuong on 27/03/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048
#define SA struct sockaddr
#define PORT 8080

//void func(int sockfd){
//    char buff[BUFFER_SIZE];
//    int n;
//    for (;;){
//        bzero(buff, sizeof(buff));
//        printf("Enter the string: ");
//        n = 0;
//        while ((buff[n++] = getchar()) != '\n')
//            ;
//        write(sockfd, buff, sizeof(buff));
//        bzero(buff, sizeof(buff));
//        read(sockfd, buff, sizeof(buff));
//        printf("From Server: %s", buff);
//        if((strncmp(buff, "exit", 4)) == 0){
//            printf("Client Exit ...\n");
//            break;
//        }
//    }
//}

void send_request(int fd){
    char buff[BUFFER_SIZE];
    char request[BUFFER_SIZE];
    int request_length = sprintf(request, "GET /serverroot/index.html HTTP/1.1\r\n");

    //write(fd, "GET /serverroot/index.html HTTP/1.1\r\n", strlen("GET /serverroot/index.html HTTP/1.1\r\n"));
    printf("1\n");
    //bzero(buff, BUFFER_SIZE);
    int sn = send(fd, request, request_length, 0);

    printf("2\n");
    if (sn < 0){
        perror("send");
    }

    printf("3\n");
    while(read(fd, buff, BUFFER_SIZE - 1) != 0){
        fprintf(stderr, "%s", buff);
        bzero(buff, BUFFER_SIZE);
    }

    printf("4\n");
    close(fd);
}

int main(){
    int sockfd;
    struct sockaddr_in serv_addr;

    //socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        printf("Socket creation failed\n");
        exit(0);
    }else{
        printf("Socket successfully created\n");
    }
    bzero(&serv_addr, sizeof(serv_addr));

    //assign IP PORT
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    //Convert IPv4 IPv6 addresses from text to binary form
//    if (inet_pton(AF_INET, "127.0.0.1",&serv_addr.sin_addr) <= 0){
//        printf("\nInvalid address/ Address not supported\n");
//        return -1;
//    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
        printf("Connection with server failed...\n");
        exit(0);
    }else{
        printf("Connected to the server...\n");
    }

    send_request(sockfd);

    close(sockfd);

    return 0;
}
