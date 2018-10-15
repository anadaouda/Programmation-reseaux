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


#define MAX_BUFFER_SIZE 100
#define MAX_FD 2
#define MAX_USERNAME 50

struct userInfo {
    //int index
    int loggedIn;
    char * username;
    char * conTime;
    char * IP;
    int port;
    //struct userInfo * next
};

/*
void newUser(struct userInfo * users) {
    struct userInfo * current = users;
    struct userInfo * newUser = malloc(sizeof(struct userInfo));
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = newUser;
    newUser->next = NULL;
}

struct userInfo * searchByUsername(struct userInfo * users, username) {
    struct userInfo * current = users;
    while((current != NULL)&&(strcmp(current->username, username))) {
        current = current->next;
    }
    return current;
}


void deleteUser(int index, struct userInfo * users) {
    struct userInfo * current = users;
    while((current != NULL)&&(current->index == index)) {
        current = current->next;
    }
    if (current == NULL) {
        return;
    }
}
*/

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
    int messageLen = strlen(buffer);

    do {
        strSent = send(rdwrSock,&messageLen,sizeof(int),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != sizeof(int));

    do {
        strSent = send(rdwrSock,buffer,strlen(buffer),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != strlen(buffer));
}

void deleteUser(struct userInfo * user) {
    memset(user->username, '\0', MAX_USERNAME);
    user->port = 0;
    user->loggedIn = -1;
}

int searchByUsername (char * username, struct userInfo users[]) {
    int i=0;

    while((i<MAX_FD)&&strcmp(users[i].username, username)) {
        i++;
    }
    return i;
}

void do_receive(int rdwrSock, int sockServer, char * buffer, struct pollfd structPollFd[], struct userInfo users[]) {
    memset(buffer, '\0', strlen(buffer));

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

void whois (char * buffer, char * username, struct userInfo users[]) {
    int i = searchByUsername(username, users);
    if (i < MAX_FD) {
        sprintf(buffer, "%s connected since 2014/09/29@19:23 with IP address %s and port number %i\n", users[i].username, users[i].IP, users[i].port);

    } else {
        sprintf(buffer, "%s", "The user does not exist or is not connceted\n");
    }
}

void who (char * buffer, struct userInfo users[]) {
    int i;
    char * sentence = malloc(200*sizeof(char));
    strcat(sentence, "Online users are : ");
    for (i = 0; i < MAX_FD; i++) {
        if(users[i].port != 0) {
            strcat(sentence, "\n\t- ");
            strcat(sentence, users[i].username);
        }
    }
    strcat(sentence, "\n");
    sprintf(buffer, "%s", sentence);
}

void nick(char * buffer, struct userInfo users[], char * username, int userIndex) {
    int i = searchByUsername(username, users);
    if (i >= MAX_FD) {
        strcpy(users[userIndex].username, username);
        sprintf(buffer, "Your new username is %s. Welcome back !\n", users[userIndex].username);
    } else {
        sprintf(buffer, "%s", "The username is already taken.\n");
    }
}

void loggedIn (char * buffer, struct userInfo users[], int rdwrSock, int i) {
    char * username = malloc(MAX_USERNAME*sizeof(char));
    if (!strncmp(buffer, "/nick ", 6)) {
        sscanf(buffer, "/nick %s", username);
        if (searchByUsername(username, users) >= MAX_FD) {
            strcpy(users[i].username, username);
            users[i].loggedIn = 1;
            sprintf(buffer, "Welcome on the chat %s", username);
            do_send(rdwrSock, buffer);
        }
        else {
            buffer = "This user is already connected\n";
            do_send(rdwrSock, buffer);
        }
    } else {
        buffer = "Please logon with /nick <your pseudo>\n";
        do_send(rdwrSock, buffer);
    }
}

void getPortAndAddr(const char* address, const char* port) {
    struct addrinfo * res = malloc(sizeof(struct addrinfo **));
    int status;
    struct addrinfo hints;

    memset(&hints,0,sizeof(hints));

    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    status = getaddrinfo(address,port,&hints,&res);
    freeaddrinfo(res);
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

    struct userInfo users[MAX_FD];
    int j;
    for (j = 0; j < 10; j++) {
        users[j].username = malloc(MAX_USERNAME);
        users[j].IP = malloc(16);
        users[j].port = 0;
    }
    //memset(users, '\0', sizeof(struct userInfo)*MAX_FD);


    int rdwrSock;
    int i;
    int resPoll;
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));

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
                //do receive ip + port
                structPollFd[i].fd = rdwrSock;
                printf("rdwrSock = %i\n", rdwrSock);
                fflush(stdout);
                structPollFd[i].events = POLLIN;
                users[i-1].loggedIn = -1;
                users[i-1].port = sockAddr.sin_port;
                strcpy(users[i-1].IP, inet_ntoa(sockAddr.sin_addr));
                do_send(structPollFd[i].fd, "Please logon with /nick <your pseudo>");

            }
        }
        else {
            //goes through the pollfd table to send and receive data
            for (i = 1; i < MAX_FD + 1; i++) {
                if (structPollFd[i].revents == POLLIN) {
                    if (users[i-1].loggedIn == 1) {
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd, users);
                        if (!strncmp(buffer, "/whois ", 7)) {
                            char * username = malloc(MAX_USERNAME);
                            sscanf(buffer, "/whois %s", username);
                            whois(buffer, username, users);
                        }
                        else if (!strncmp(buffer, "/who\n", 4)) {
                            who(buffer, users);
                        }
                        else if (!strncmp(buffer, "/nick ", 6)) {
                            char * username = malloc(MAX_USERNAME);
                            sscanf(buffer, "/nick %s", username);
                            nick(buffer, users, username, i-1);
                        }
                        else if (!strcmp(buffer, "/quit\n")) {
                            do_send(structPollFd[i].fd ,"You will be terminated");
                            close(structPollFd[i].fd);
                            memset(&structPollFd[i], '\0', sizeof(struct pollfd));
                            deleteUser(&users[i-1]);
                            break;
                        }
                        if (structPollFd[i].fd != 0) {
                            do_send(structPollFd[i].fd, buffer);
                        }
                    } else if (users[i-1].loggedIn==-1) {
                        do_receive(structPollFd[i].fd, sockServer, buffer, structPollFd, users);
                        if (!strcmp(buffer, "/quit\n")) {
                            do_send(structPollFd[i].fd ,"You will be terminated");
                            close(structPollFd[i].fd);
                            memset(&structPollFd[i], '\0', sizeof(struct pollfd));
                            memset(&users[i-1], '\0', sizeof(struct userInfo));
                            break;
                        }
                        loggedIn(buffer, users, structPollFd[i].fd, i-1);
                    }
                }
        }
    }
}
    return 0;
}
