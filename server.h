/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#ifndef server_h
#define server_h

#include <sys/socket.h>
#include <netinet/in.h>

struct Server{
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;

    void (*launch) (struct Server *svr);
};

struct Server server_constructor(int domain, int service, int protocol, u_long interface,
                                int port, int backlog, void (*launch)(struct Server *svr));

#endif 