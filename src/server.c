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
    char * path = malloc(MAX_BUFFER_SIZE);
    int portP2P;

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
                    if ((getLoggedIn(currentUser) == 1)&&(isInChannel(currentUser) == -1)&&(confirmedP2P(currentUser)==0)) {
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
                        else if (!strncmp(buffer, "/send ", 6)) {
                            memset(username, '0', MAX_USERNAME);
                            sscanf(buffer, "/send %s %i", username, &portP2P);
                            //creer une fonction checkArgs(buffer, nbArgs);
                            sendCheck(structPollFd, buffer, users, username, currentUser, portP2P);
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
                    else if (confirmedP2P(currentUser) != 0) {
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd);
                        if((!strcmp(buffer, "y\n"))||(!strcmp(buffer, "Y\n"))) {
                            struct userInfo * sender = searchByIndex(users, confirmedP2P(currentUser));
                            do_send(structPollFd[confirmedP2P(currentUser)].fd, "machin accepted the transfer", "SERVER");
                            sprintf(buffer, "/recvFile %s %i", getIP(sender), getPortP2P(sender));
                            do_send(structPollFd[i].fd, buffer, "SERVER");
                        } else if((!strcmp(buffer, "n\n"))||(!strcmp(buffer, "N\n"))) {
                            do_send(structPollFd[confirmedP2P(currentUser)].fd, "machin said fuck you", "SERVER");
                        }
                        setP2P(currentUser, 0);
                    }

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
