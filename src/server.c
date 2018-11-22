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
#include "header/P2P.h"
#include "header/socketSetup.h"

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
    struct sockaddr_in sockAddr = init_addr("127.0.0.1", atoi(argv[1]));
    do_bind(sockServer, sockAddr);
    do_listen(sockServer);

    //Setting up the pollfd structure

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

    char * command = malloc(50);

    while(1) {
        resPoll = poll(structPollFd, MAX_FD + 1, -1);

        if (structPollFd[0].revents == POLLIN) {
            if (nbOpenFd(structPollFd) >= MAX_FD) {
                rdwrSock = do_accept(sockServer, sockAddr);
                do_send( "/serverOverload",rdwrSock, "SERVER");
                close(rdwrSock);

            } else if (resPoll == -1) {
                perror("Poll");
                exit(EXIT_FAILURE);

            } else { //accept connection from client
                rdwrSock = do_accept(sockServer, sockAddr);
                i = spacePollFd(structPollFd);
                structPollFd[i].fd = rdwrSock;
                structPollFd[i].events = POLLIN;

                newUser(users, i, inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
                do_send("Please logon with /nick <your pseudo>",structPollFd[i].fd,  "SERVER");

            }
        } else {
            for (i = 1; i < MAX_FD + 1; i++) {
                if (structPollFd[i].revents == POLLIN) {
                    currentUser = searchByIndex(users, i);
                    do_receive(structPollFd[i].fd, buffer, SERVER);
                    memset(command, '\0', 50);
                    sscanf(buffer, "%s", command);

                    if ((isLoggedIn(currentUser) == YES) && (isInChannel(currentUser) == NO) && (isInP2P(currentUser) == NO)) {
                        if (!strcmp(command, "/whois" )) { whois(buffer, users);}
                        else if (!strcmp(command, "/who" )) { who(buffer, users,-1);}
                        else if (!strcmp(command, "/channellist" )) { channelList(buffer, channels);}
                        else if (!strcmp(command, "/nick" )) { nick(buffer, users, currentUser);}
                        else if (!strcmp(command, "/msgall" )) { msgall(buffer, currentUser, users, structPollFd, -1, NULL);}
                        else if (!strcmp(command, "/msg" )) { msg(currentUser, users, structPollFd, buffer);}
                        else if (!strcmp(command, "/createchannel" )) { newChannel(channels, buffer);}
                        else if (!strcmp(command, "/join" )) { join(channels, currentUser, users,buffer);}
                        else if (!strcmp(command, "/quit" )) { quit(buffer, &structPollFd[i], i, users, NULL, NULL, NULL);}
                        else if (!strcmp(command, "/send" )) { sendCheck(structPollFd, buffer, users, currentUser);}

                    } else if (isLoggedIn(currentUser) == NO) { //if client is not connected
                        if (!strcmp(command, "/quit")) {
                            quit(buffer, &structPollFd[i], i, users, NULL, NULL, NULL);
                            break;
                        }
                        loggedIn(buffer, users, structPollFd[i].fd, currentUser);

                    } else if (isInP2P(currentUser) != NO) {
                        if (!strcmp(command, "/P2Pdone")) {
                            endP2P(currentUser, structPollFd);
                            break;
                        }

                        confirmP2P(buffer, currentUser, users,structPollFd);

                    } else if (isInChannel(currentUser) != NO) { //if client is in a channel
                        currentChannel = searchChannelByIndex(channels,isInChannel(currentUser));

                        if (!strncmp(buffer, "/quit ", 6)) { quitChannel(channels,currentUser, buffer);}
                        else if (!strcmp(command, "/who")) { who(buffer, users, isInChannel(currentUser));}
                        else if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users, currentChannel, currentUser, channels);
                            break;

                        } else {
                            msgall(buffer,currentUser, users, structPollFd, isInChannel(currentUser), getChannelName(currentChannel));
                            break;

                        }
                    }

                    if (strcmp(buffer, "")) {

                        do_send(buffer,structPollFd[i].fd,  "SERVER");

                    }
                }
            }
        }
    }
    return 0;

}
