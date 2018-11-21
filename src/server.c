#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>

#include "header/users.h"
#include "header/constant.h"
#include "header/channels.h"
#include "header/serverFunctions.h"


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr,"Veuillez indiquer un numero de port\n\n");
        return 1;
    } else {
        printf("usage: RE216_SERVER %s\n", argv[1]);
        fflush(stdout);
    }

    //Setting up the socket
    int sockServer = do_socket();
    struct sockaddr_in sockAddr = init_serv_addr(argv);
    do_bind(sockServer, sockAddr);
    do_listen(sockServer);

    //Setting up the pollfd structure
    struct pollfd structPollFd[MAX_FD + 1];
    memset(structPollFd, '\0', sizeof(struct pollfd)*(MAX_FD + 1));
    structPollFd[0].fd = sockServer;
    structPollFd[0].events = POLLIN;

    //Setting up variables
    struct userInfo * users = createUsers();
    struct channelInfo * channels = createChannel();
    int rdwrSock, i, resPoll;
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));
    struct userInfo * currentUser = createUsers();
    struct channelInfo * currentChannel = createChannel();
    char * username = malloc(MAX_USERNAME);
    char * channelName = malloc(MAX_CHANNEL_NAME);
    char * message = malloc(MAX_BUFFER_SIZE);
    char * path = malloc(MAX_BUFFER_SIZE);
    char * command = malloc(50);

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
            else { //accept connection from client
                rdwrSock = do_accept(sockServer, sockAddr);
                i = spacePollFd(structPollFd);
                structPollFd[i].fd = rdwrSock;
                structPollFd[i].events = POLLIN;
                newUser(users, i, inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
                do_send(structPollFd[i].fd, "Please logon with /nick <your pseudo>", "SERVER");

            }
        }
        else {
            for (i = 1; i < MAX_FD + 1; i++) {
                if (structPollFd[i].revents == POLLIN) {
                    currentUser = searchByIndex(users, i);
                    if ((isLoggedIn(currentUser) == YES) && (isInChannel(currentUser) == NO) && (isInP2P(currentUser) == NO)) {
                        do_receive(structPollFd[i].fd, sockServer, buffer);
                        sscanf(buffer, "%s", command);
                        if (!strcmp(command, "/whois" )) { whois(buffer, users);}
                        else if (!strcmp(command, "/who" )) { who(buffer, users,-1);}
                        else if (!strcmp(command, "/channellist" )) { channelList(buffer, channels);}
                        else if (!strcmp(command, "/nick" )) { nick(buffer, users, currentUser);}
                        else if (!strcmp(command, "/msgall" )) { msgall(buffer, currentUser, users, structPollFd, -1);}
                        else if (!strcmp(command, "/msg" )) { msg(currentUser, users, structPollFd, buffer);}
                        else if (!strcmp(command, "/createchannel" )) { newChannel(channels, buffer);}
                        else if (!strcmp(command, "/join" )) { join(channels, currentUser, users,buffer);}
                        else if (!strcmp(command, "/quit" )) { quit(buffer, &structPollFd[i], i, users);}
                        else if (!strcmp(command, "/sendCheck" )) { sendCheck(structPollFd, buffer, users, currentUser);}
                        if (strcmp(buffer, "")) {
                            do_send(structPollFd[i].fd, buffer, "SERVER");
                        }

                    } else if (isLoggedIn(currentUser)==-1) { //if client is not connected
                        do_receive(structPollFd[i].fd, sockServer, buffer);
                        if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users);
                            break;
                        }
                        loggedIn(buffer, users, structPollFd[i].fd, currentUser);
                        do_send(structPollFd[i].fd, buffer, "SERVER");
                    }
                    else if (isInP2P(currentUser) != NO) {
                        do_receive(structPollFd[i].fd, sockServer, buffer);
                        char * confirm = malloc(100);
                        if((!strcmp(buffer, "y\n"))||(!strcmp(buffer, "Y\n"))) {
                            struct userInfo * sender = searchByIndex(users, isInP2P(currentUser));
                            sprintf(confirm, "%s accepted the transfert", getUsername(currentUser));
                            do_send(structPollFd[isInP2P(currentUser)].fd, confirm, "SERVER");
                            sprintf(buffer, "/recvFile %s %i", getIP(sender), getPortP2P(sender));
                            do_send(structPollFd[i].fd, buffer, "SERVER");
                        } else if((!strcmp(buffer, "n\n"))||(!strcmp(buffer, "N\n"))) {
                            sprintf(confirm, "%s cancelled file transfert", getUsername(currentUser));
                            do_send(structPollFd[isInP2P(currentUser)].fd, confirm, "SERVER");
                        }
                        setP2P(currentUser, NO);
                        free(confirm);
                    }

                    else if (isInChannel(currentUser) != NO) { //if client is in a channel
                        currentChannel = searchChannelByIndex(channels,isInChannel(currentUser));
                        do_receive(structPollFd[i].fd, sockServer, buffer);

                        if (!strncmp(buffer, "/quit ", 6)) {
                            quitChannel(channels,currentUser, buffer);
                        }
                        else if (!strncmp(buffer, "/who\n", 5)) {
                            who(buffer, users, isInChannel(currentUser));
                        }
                        else if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users);
                            break;
                        }
                        else {

                            msgall(buffer,currentUser, users, structPollFd, isInChannel(currentUser));
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
