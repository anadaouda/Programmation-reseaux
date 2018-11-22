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
#include "header/socketSetup.h"


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
    do_send("You will be terminated",structPollFd->fd , "SERVER");
    close(structPollFd->fd);
    memset(structPollFd, '\0', sizeof(struct pollfd));
    deleteUser(i, users);
}


void msgall(char * buffer, struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], int channelIndex, char * channelName) {
    char * message = malloc(MAX_BUFFER_SIZE);

    struct userInfo * current = users;

    if (channelIndex == -1) {
        sscanf(buffer, "/msgall %[^\n]s", message);
        while(current != NULL) {
            if ((isLoggedIn(current) == 1)&&(getIndex(current) != getIndex(sender))) {

                do_send( message,structPollFd[getIndex(current)].fd, getUsername(sender));
            }
            current = getNext(current);
        }
    } else {
        while(current != NULL) {

            if ((isLoggedIn(current) == 1)&&(getIndex(current) != getIndex(sender))&&(isInChannel(current) == channelIndex)) {
                char * who = malloc(MAX_USERNAME+ MAX_CHANNEL_NAME + 4);
                sprintf(who, "%s in %s", getUsername(sender), channelName);
                do_send(buffer,structPollFd[getIndex(current)].fd,  who);

            }
            current = getNext(current);
        }
    }
}


void msg(struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], char * buffer) {
    char * message = malloc(MAX_BUFFER_SIZE);
    char * username = malloc(MAX_USERNAME);
    sscanf(buffer, "/msg %s %[^\n]s", username, message);

    struct userInfo * receiver = searchByUsername(users, username);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    if (receiver != NULL) {
        do_send( message,structPollFd[getIndex(receiver)].fd, getUsername(sender));
        sprintf(buffer, "%s", "");
    } else {
        sprintf(buffer, "L'utilisateur '%s' n'existe pas ou n'est pas connecte\n",username);
    }
}
