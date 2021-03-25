/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "server.h"
#include <stdio.h>
#include <stdlib.h>

struct Server server_constructor(int domain, int service, int protocol, u_long interface,
                                int port, int backlog, void (*launch) (struct Server *svr)){
    struct Server svr;

    svr.domain = domain;
    svr.service = service;
    svr.protocol = protocol;
    svr.interface = interface;
    svr.port = port;
    svr.backlog = backlog;
    
    svr.address.sin_family = domain;
    svr.address.sin_port = htons(port);
    svr.address.sin_addr.s_addr = htonl(interface);

    svr.socket = socket(domain, service, protocol);
    if(svr.socket == 0){
        perror("Failed to connect socket ...\n");
        exit(1);
    }

    if(bind(svr.socket, (struct sockaddr *)&svr.address, sizeof(svr.address)) < 0){
        perror("Failed to bind socket ...\n");
        exit(1);
    }

    if(listen(svr.socket, svr.backlog) < 0){
        perror("Failed to start listening ...\n");
        exit(1);
    }

    svr.launch = launch;

    return svr;
}