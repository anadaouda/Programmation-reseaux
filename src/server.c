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

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr,"Veuillez indiquer un numero de port\n\n");
        return 1;
    } else {
        printf("usage: RE216_SERVER %s\n", argv[1]);
        fflush(stdout);
    }
    //create the socket, check for validity!
    int sockServer = do_socket();

    //init the serv_add structure
    struct sockaddr_in sockAddr = init_serv_addr(argv);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(sockServer, sockAddr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(sockServer);

    struct pollfd structPollFd[MAX_FD + 1];
    memset(structPollFd, '\0', sizeof(struct pollfd)*(MAX_FD + 1));
    structPollFd[0].fd = sockServer;
    structPollFd[0].events = POLLIN;

    struct userInfo * users = createUsers();
    struct channelInfo * channels = createChannel();

    int rdwrSock;
    int i;
    int resPoll;
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));
    struct userInfo * currentUser = createUsers();
    struct channelInfo * currentChannel = createChannel();
    char * username = malloc(MAX_USERNAME);
    char * channelName = malloc(MAX_CHANNEL_NAME);
    char * message = malloc(MAX_BUFFER_SIZE);

    while(1) {
        resPoll = poll(structPollFd, MAX_FD + 1, -1);

        if (structPollFd[0].revents == POLLIN) {
            if (nbOpenFd(structPollFd) >= MAX_FD) {
                rdwrSock = do_accept(sockServer, sockAddr);
                do_send(rdwrSock, "/serverOverload", "SERVER");
                close(rdwrSock);
            }
            else if (resPoll == -1) {
                perror("Poll");
                exit(EXIT_FAILURE);
            }
            else {
                //accept connection from client
                rdwrSock = do_accept(sockServer, sockAddr);
                i = spacePollFd(structPollFd);
                structPollFd[i].fd = rdwrSock;
                structPollFd[i].events = POLLIN;
                newUser(users, i, inet_ntoa(sockAddr.sin_addr), sockAddr.sin_port);
                do_send(structPollFd[i].fd, "Please logon with /nick <your pseudo>", "SERVER");

            }
        }
        else {
            //goes through the pollfd table to send and receive data
            for (i = 1; i < MAX_FD + 1; i++) {
                if (structPollFd[i].revents == POLLIN) {
                    currentUser = searchByIndex(users, i);
                    if ((getLoggedIn(currentUser) == 1)&&(isInChannel(currentUser) == -1)) {
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd);
                        if (!strncmp(buffer, "/whois ", 7)) {
                            memset(username, '0', MAX_USERNAME);
                            sscanf(buffer, "/whois %s", username);
                            whois(buffer, username, users);

                        }
                        else if (!strncmp(buffer, "/who\n", 5)) {
                            who(buffer, users,-1);
                        }
                        else if (!strncmp(buffer, "/channellist\n", 13)) {
                            channelList(buffer, channels);
                        }
                        else if (!strncmp(buffer, "/nick ", 6)) {
                            memset(username, '0', MAX_USERNAME);
                            sscanf(buffer, "/nick %s", username);
                            nick(buffer, users, username, currentUser);
                        }
                        else if (!strncmp(buffer, "/msgall ", 8)) {
                            memset(message, '0', MAX_BUFFER_SIZE);
                            sscanf(buffer, "/msgall %[^\n]s", message);
                            msgall(currentUser, message, users, structPollFd, -1);
                            break;
                        }
                        else if (!strncmp(buffer, "/msg ", 5)) {
                            memset(username, '0', MAX_USERNAME);
                            memset(message, '0', MAX_BUFFER_SIZE);
                            sscanf(buffer, "/msg %s %[^\n]s", username, message);
                            if (msg(currentUser,username, message, users, structPollFd, buffer)) {
                                break;
                            }
                        }
                        else if (!strncmp(buffer, "/createchannel ", 15)) {
                            memset(channelName, '0', MAX_CHANNEL_NAME);
                            sscanf(buffer, "/createchannel %s", channelName);
                            newChannel(channels, channelName, buffer);
                        }
                        else if (!strncmp(buffer, "/join ", 6)) {
                            memset(channelName, '0', MAX_CHANNEL_NAME);
                            sscanf(buffer, "/join %s", channelName);
                            join(channels, channelName, currentUser, users,buffer);
                        }
                        else if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users);
                            break;
                        }
                        /*
                        else if cest une commande send il faut verifier que le user existe
                            si oui envouer /send userClear et changer la variable accepted share du recepteur à l'index de l'expediteur?
                            mettre @IP et port
                            envoyer au recepteur y ou n
                            sinon envoyer error
                            break
                        */
                        do_send(structPollFd[i].fd, buffer, "SERVER");

                    } else if (getLoggedIn(currentUser)==-1) { //if client is not connected
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd);
                        if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users);
                            break;
                        }
                        loggedIn(buffer, users, structPollFd[i].fd, currentUser);
                        do_send(structPollFd[i].fd, buffer, "SERVER");
                    }
                    /*
                    else if (acceptedShare(currentUser) != 0) {
                        if yes envoyer ip et sin_port au currentUser
                        if no renvoyer send error au numero du accepted acceptedShare
                        dans les deux cas changer le acceptedShare a 0;

                    }
                    */
                    else if (isInChannel(currentUser) != -1) { //if client is in a channel
                        currentChannel = searchChannelByIndex(channels,isInChannel(currentUser));
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd);

                        if (!strncmp(buffer, "/quit ", 6)) {
                            memset(channelName, '0', MAX_CHANNEL_NAME);
                            sscanf(buffer, "/quit %s", channelName);
                            quitChannel(channels, channelName, currentUser, buffer);
                        }
                        else if (!strncmp(buffer, "/who\n", 5)) {
                            who(buffer, users, isInChannel(currentUser));
                        }
                        else {
                            msgall(currentUser, buffer, users, structPollFd, isInChannel(currentUser));
                            break;
                        }

                        do_send(structPollFd[i].fd, buffer, "SERVER");
                    }
                }
        }
    }
}
    return 0;
}
