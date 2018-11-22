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
#include <pthread.h>

#include "header/users.h"
#include "header/constant.h"
#include "header/channels.h"
#include "header/socketSetup.h"

char * getFilename(char * path) {
    int i, j;
    char * result = malloc(strlen(path));
    int position = 0;

    for (i = 0 ; i < strlen(path); i++) {
        if (path[i] == '/') {
            position = i+1;
        }
    }

    strcpy(result, path + position);
    return result;
}

void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, struct userInfo * sender) {
    char * username = malloc(MAX_USERNAME);
    char * path = malloc(MAX_BUFFER_SIZE);
    int portP2P;
    sscanf(buffer, "/send %s %s %i", username, path, &portP2P);
    struct userInfo * recipient = searchByUsername(users, username);

    memset(buffer, '\0', MAX_BUFFER_SIZE);

    if ((recipient != NULL)&&(recipient != sender)) {
        char * confirm = malloc(100);

        setP2P(recipient, getIndex(sender)); //setP2Ppeer
        setPortP2P(sender,portP2P);

        sprintf(confirm, "%s wants you to accept the transfer of the file named '%s'. Do you accept? [Y/n]", getUsername(sender), getFilename(path));
        sprintf(buffer,"%s", "/sendCheck ok");

        do_send(confirm, structPollFd[getIndex(recipient)].fd, "SERVER");

    } else {
        sprintf(buffer, "%s", "/sendCheck error");

    }
}


void sendFile(char * buffer, int sock, pthread_mutex_t * lock, pthread_cond_t * cond, int * canSend) {
    char * path = malloc(MAX_BUFFER_SIZE);
    char * username = malloc(MAX_USERNAME);

    sscanf(buffer, "/send %s %s", username, path);
    FILE * fileToSend = fopen(path, "r");

    if (fileToSend != NULL) {
        int sockP2P = do_socket();
        struct sockaddr_in sockP2PAddr = init_addr("127.0.0.1", 33001);

        do_bind(sockP2P, sockP2PAddr);
        do_listen(sockP2P);
        sprintf(buffer, "%s %i", buffer, ntohs(sockP2PAddr.sin_port));
        do_send(buffer, sock, NULL);

        pthread_mutex_lock(lock);
        while(*canSend == UNCLEAR) {
            pthread_cond_wait(cond, lock);
        }

        if (*canSend == YES) {
            int wrSockP2P = do_accept(sockP2P, sockP2PAddr);
            char * filename = getFilename(path);
            fseek(fileToSend, 0, SEEK_END);
            int fileSize = ftell(fileToSend);
            rewind(fileToSend);

            char * fileToSendStr = malloc((fileSize + strlen(filename)+1)*sizeof(char));
            char * file = malloc(fileSize*sizeof(char));

            fread(file, sizeof(char), fileSize, fileToSend);
            sprintf(fileToSendStr, "%s %s", filename, file);

            do_send(fileToSendStr, wrSockP2P, NULL);
        }
        *canSend = UNCLEAR;
        pthread_mutex_unlock(lock);
        close(sockP2P);

    } else {
        printf("The file does not exist\n");
        fflush(stdout);
    }
}

void recvFile(char * buffer) {
    char * IP = malloc(15*sizeof(char));
    int portP2P;
    char * fileReceived = malloc(200);
    char * filename = malloc(100);

    sscanf(buffer,"[SERVER] : /recvFile %s %i", IP, &portP2P);
    struct sockaddr_in sockAddrP2P = get_addr_info(IP, portP2P);
    int sockP2P = do_socket();
    do_connect(sockP2P, (struct sockaddr * )&sockAddrP2P);

    do_receive(sockP2P, fileReceived, CLIENT);
    sscanf(fileReceived, "%s", filename);
    char * file = fileReceived + strlen(filename)+1;

    char * path = malloc((strlen(INBOX_DIR)+strlen(filename) + 1)*sizeof(char));
    sprintf(path, "%s/%s", INBOX_DIR, filename);
    FILE * fileToSave = fopen(path, "a");
    fprintf(fileToSave, "%s", file);
    fclose(fileToSave);

    printf("%s saved in %s\n", filename, INBOX_DIR);
    sprintf(buffer, "%s", "/P2Pdone");
    close(sockP2P);
}

void confirmP2P(char * buffer, struct userInfo * recipient, struct userInfo * users, struct pollfd structPollFd[]) {
    char * confirm = malloc(100);

    if((!strcmp(buffer, "y\n"))||(!strcmp(buffer, "Y\n"))) {
        struct userInfo * sender = searchByIndex(users, isInP2P(recipient));

        sprintf(confirm, "%s accepted the transfert", getUsername(recipient));
        sprintf(buffer, "/recvFile %s %i", getIP(sender), getPortP2P(sender));

        do_send(confirm,structPollFd[isInP2P(recipient)].fd, "SERVER");

    } else if((!strcmp(buffer, "n\n"))||(!strcmp(buffer, "N\n"))) {
        sprintf(confirm, "%s cancelled file transfert", getUsername(recipient));
        do_send(confirm, structPollFd[isInP2P(recipient)].fd,  "SERVER");
    }

    free(confirm);
}


void endP2P(struct userInfo * recipient, struct pollfd structPollFd[]) {
    char * confirm = malloc(100);
    sprintf(confirm, "%s received file", getUsername(recipient));
    do_send(confirm,structPollFd[isInP2P(recipient)].fd, "SERVER");
    setP2P(recipient, NO);
    free(confirm);
}
