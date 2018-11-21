#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#include "header/constant.h"

struct sockaddr_in init_addr(char * IP, int port) {
    struct sockaddr_in sockAddr;

    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    inet_aton(IP, &sockAddr.sin_addr);

    return sockAddr;
}

struct sockaddr_in get_addr_info(char* IP, int port) {
    struct sockaddr_in sockAddr;

    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);
    inet_aton(IP, &sockAddr.sin_addr);

    return sockAddr;
}

void do_bind(int sock, struct sockaddr_in sockAddr) {
    if (bind(sock, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) == -1) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
}

void do_listen(int sock) {
    if (listen(sock, MAX_FD) == -1) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

int do_accept(int sock, struct sockaddr_in sockAddr) {
    int addrLen = sizeof(struct sockaddr);
    int rdwrSock = accept(sock, (struct sockaddr *) &sockAddr, (socklen_t *)&addrLen);
    if (rdwrSock == -1) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    return rdwrSock;
}

int do_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int yes = 1;

    if (sock == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("ERROR setting socket options");
    }
    return sock;
}

void do_connect(int sock, struct sockaddr * sockAddr) {
    printf("Connecting ...");
    fflush(stdout);
    if (connect(sock, sockAddr, sizeof(struct sockaddr_in)) == -1) {
        perror("\nConnect");
        exit(EXIT_FAILURE);
    }
    printf("done !\n\n");
    fflush(stdout);
}

void do_send(char * buffer, int sock, char * who) {
    int strSent;
    int inputLen = strlen(buffer);

    if (who != NULL) {
        char * sender = malloc(MAX_BUFFER_SIZE);
        sprintf(sender, "[%s] : ", who);
        strcat(sender, buffer);
        buffer = sender;
    }

    do {
        strSent = send(sock,&inputLen,sizeof(int),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != sizeof(int));

    do {
        strSent = send(sock,buffer,strlen(buffer),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != strlen(buffer));
}

void do_receive(int sock, char * buffer, int type, int rdwrSock) {
        int strReceived, strSizeToReceive;
        do {
            strReceived = recv(sock, &strSizeToReceive, sizeof(int), 0);
            if (strReceived == -1) {
                perror("Receive");
                exit(EXIT_FAILURE);
            }
        } while (strReceived != sizeof(int));

        do {
            strReceived = recv(sock, buffer, SSIZE_MAX, 0);
            if (strReceived == -1) {
                perror("Receive");
                exit(EXIT_FAILURE);
            }
        } while (strReceived != strSizeToReceive);

        if (type == SERVER) {
            printf("[%i] : %s\n",rdwrSock, buffer);
            fflush(stdout);
        }
}
