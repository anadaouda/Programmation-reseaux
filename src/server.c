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


void do_send(int rdwrSock, char * buffer) {
    int strSent;
    int messageLen = MAX_BUFFER_SIZE;

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

void whois (char * buffer, char * username, struct userInfo * users) {
    struct userInfo * user = searchByUsername(users, username);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    if (user != NULL) {
        sprintf(buffer, "%s is connected since %04d-%02d-%02d %02d:%02d:%02d with IP address %s and port number %i\n", getUsername(user),getYear(user),getMonth(user),getDay(user),getHour(user),getMinute(user),getSecond(user), getIP(user), getPort(user));

    } else {
        sprintf(buffer, "%s", "The user does not exist or is not connected\n");
    }
}

void who (char * buffer, struct userInfo * users) {
    char * sentence = malloc(200*sizeof(char));
    struct userInfo * current = getNext(users);

    sprintf(sentence, "%s", "Online users are : ");
    while(current != NULL) {
            strcat(sentence, "\n\t- ");
            strcat(sentence, getUsername(current));
        current = getNext(current);
    }
    strcat(sentence, "\n");
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    sprintf(buffer, "%s", sentence);
}

void nick(char * buffer, struct userInfo * users, char * username, struct userInfo * currentUser) {
    struct userInfo * available = searchByUsername(users, username);
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    if (available == NULL) {
        strcpy(getUsername(currentUser), username);
        sprintf(buffer, "Your new username is %s. Welcome back !\n", getUsername(currentUser));
    } else {
        sprintf(buffer, "%s", "The username is already taken.\n");
    }
}

void loggedIn (char * buffer, struct userInfo * users, int rdwrSock, struct userInfo * currentUser) {
    char * username = malloc(MAX_USERNAME*sizeof(char));
    struct userInfo * available = createUser();
    time_t seconds = time(NULL);

    if (!strncmp(buffer, "/nick ", 6)) {
        sscanf(buffer, "/nick %s", username);
         available = searchByUsername(users, username);
        if (available == NULL) {
            strcpy(getUsername(currentUser), username);
            setLoggedIn(currentUser, 1);
            setConTime(currentUser,localtime(&seconds));
            memset(buffer, '\0', MAX_BUFFER_SIZE);
            sprintf(buffer, "Welcome on the chat %s", username);
            do_send(rdwrSock, buffer);
        }
        else {
            memset(buffer, '\0', MAX_BUFFER_SIZE);
            buffer = "This user is already connected\n";
            do_send(rdwrSock, buffer);
        }
    } else {
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        buffer = "Please logon with /nick <your pseudo>\n";
        do_send(rdwrSock, buffer);
    }
}

void quit(char * buffer, struct pollfd * structPollFd, int i, struct userInfo * users) {
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    buffer = "You will be terminated\n";
    do_send(structPollFd->fd ,"You will be terminated");
    close(structPollFd->fd);
    memset(structPollFd, '\0', sizeof(struct pollfd));
    deleteUser(i, users);
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

    struct userInfo * users = createUser();
    setNext(users, NULL);

    int rdwrSock;
    int i;
    int resPoll;
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));
    struct userInfo * currentUser = createUser();

    while(1) {
        resPoll = poll(structPollFd, MAX_FD + 1, -1);

        if (structPollFd[0].revents == POLLIN) {
            if (nbOpenFd(structPollFd) >= MAX_FD) {
                rdwrSock = do_accept(sockServer, sockAddr);
                do_send(rdwrSock, "/serverOverload");
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
                do_send(structPollFd[i].fd, "Please logon with /nick <your pseudo>");

            }
        }
        else {
            //goes through the pollfd table to send and receive data
            for (i = 1; i < MAX_FD + 1; i++) {
                if (structPollFd[i].revents == POLLIN) {
                    currentUser = searchByIndex(users, i);
                    if (getLoggedIn(currentUser) == 1) {
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd);
                        if (!strncmp(buffer, "/whois ", 7)) {
                            char * username = malloc(MAX_USERNAME);
                            sscanf(buffer, "/whois %s", username);
                            whois(buffer, username, users);
                        }
                        else if (!strncmp(buffer, "/who\n", 5)) {
                            who(buffer, users);
                        }
                        else if (!strncmp(buffer, "/nick ", 6)) {
                            char * username = malloc(MAX_USERNAME);
                            sscanf(buffer, "/nick %s", username);
                            nick(buffer, users, username, currentUser);
                        }
                        else if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users);
                            break;
                        }
                        if (structPollFd[i].fd != 0) {
                            do_send(structPollFd[i].fd, buffer);
                        }
                    } else if (getLoggedIn(currentUser)==-1) {
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd);
                        if (!strcmp(buffer, "/quit\n")) {
                            quit(buffer, &structPollFd[i], i, users);
                        }
                        loggedIn(buffer, users, structPollFd[i].fd, currentUser);
                    }
                }
        }
    }
}
    return 0;
}
