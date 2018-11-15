#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <poll.h>
#include <time.h>
#include <netdb.h>

#include "header/users.h"
#include "header/constant.h"
#include "header/channels.h"

// creates the listening socket
int do_socket() {
    int sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int yes = 1;

    if (sockServer == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockServer, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("ERROR setting socket options");
    }

    return sockServer;
}

// initiates the server's address
struct sockaddr_in init_serv_addr(char ** argv) {
    struct sockaddr_in sockAddr;

    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(argv[1]));
    inet_aton("127.0.0.1", &sockAddr.sin_addr);

    return sockAddr;
}

void do_bind(int sockServer, struct sockaddr_in sockAddr) {
    if (bind(sockServer, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) == -1) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
}

void do_listen(int sockServer) {
    if (listen(sockServer, MAX_FD) == -1) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

int do_accept(int sockServer, struct sockaddr_in sockAddr) {
    int addrLen = sizeof(struct sockaddr);
    int rdwrSock = accept(sockServer, (struct sockaddr *) &sockAddr, (socklen_t *)&addrLen);
    if (rdwrSock == -1) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    return rdwrSock;
}


void do_send(int rdwrSock, char * buffer, char * who) {
    int strSent;
    int messageLen = MAX_BUFFER_SIZE;

    char * sender = malloc(MAX_BUFFER_SIZE);
    sprintf(sender, "[%s] : ", who);
    strcat(sender, buffer);
    buffer = sender;

    do {
        strSent = send(rdwrSock,&messageLen,sizeof(int),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != sizeof(int));

    do {
        strSent = send(rdwrSock,buffer,MAX_BUFFER_SIZE,0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != MAX_BUFFER_SIZE);
}



void do_receive(int rdwrSock, int sockServer, char * buffer, struct pollfd structPollFd[]) {
    memset(buffer, '\0', MAX_BUFFER_SIZE);

    int strReceived, strSizeToReceive;

    do {
        strReceived = recv(rdwrSock, &strSizeToReceive, sizeof(int), 0);
        if (strReceived == -1) {
            perror("Receive");
            exit(EXIT_FAILURE);
        }
    } while (strReceived != sizeof(int));

    do {
        strReceived = recv(rdwrSock, buffer, SSIZE_MAX, 0);
        if (strReceived == -1) {
            perror("Receive");
            exit(EXIT_FAILURE);
        }
    } while (strReceived != strSizeToReceive);

    printf("[%i] : %s\n",rdwrSock, buffer);
}

// returns the index of an availabla space in the pollfd table
int spacePollFd(struct pollfd structPollFd[]) {
    int i = 1;

    while ((i < MAX_FD) && (structPollFd[i].events == POLLIN)) {
        i++;
    }
    return i;
}

// returns the nuber of active fds in the pollfd table
int nbOpenFd(struct pollfd structPollFd[]) {
    int i;
    int openFd = 0;

    for (i = 1; i < MAX_FD + 1; i++) {
        if (structPollFd[i].events == POLLIN) {
            openFd++;
        }
    }
    return openFd;
}

void quit(char * buffer, struct pollfd * structPollFd, int i, struct userInfo * users) {
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    buffer = "You will be terminated\n";
    do_send(structPollFd->fd ,"You will be terminated", "SERVER");
    close(structPollFd->fd);
    memset(structPollFd, '\0', sizeof(struct pollfd));
    deleteUser(i, users);
}

void msgall(struct userInfo * sender, char * message, struct userInfo * users, struct pollfd structPollFd[], int channelIndex) {
    struct userInfo * current = users;

    if (channelIndex == -1) {
        while(current != NULL) {
            if ((getLoggedIn(current) == 1)&&(getIndex(current) != getIndex(sender))) {
                do_send(structPollFd[getIndex(current)].fd, message, getUsername(sender));
            }
            current = getNext(current);
        }
    } else {
        while(current != NULL) {
            if ((getLoggedIn(current) == 1)&&(getIndex(current) != getIndex(sender))&&(isInChannel(current) == channelIndex)) {
                do_send(structPollFd[getIndex(current)].fd, message, getUsername(sender));
            }
            current = getNext(current);
        }
    }
}

int msg(struct userInfo * sender, char * username, char * message, struct userInfo * users, struct pollfd structPollFd[], char * buffer) {
    int err = 0;
    struct userInfo * receiver = searchByUsername(users, username);
    if (receiver != NULL) {
        do_send(structPollFd[getIndex(receiver)].fd, message, getUsername(sender));
        err++;
    } else {
        sprintf(buffer, "L'utilisateur '%s' n'existe pas ou n'est pas connecte\n",username);
    }
    return err;
}

void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, char * username, struct userInfo * sender, int portP2P) {
    struct userInfo * exist = searchByUsername(users, username);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    if ((exist != NULL)&&(exist != sender)) {
        setP2P(exist, getIndex(sender));
        setPortP2P(sender,portP2P);
        do_send(structPollFd[getIndex(exist)].fd, "user1 wants you to accept the transfer of the file named 'file.txt'. Do you accept? [Y/n]", "SERVER");
        sprintf(buffer,"%s", "/sendCheck ok");
    } else {
        sprintf(buffer, "%s", "/sendCheck error");
    }
}
