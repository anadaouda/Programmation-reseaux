#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "header/constant.h"

struct userInfo {
    int index;
    int loggedIn;
    char * username;
    struct tm * conTime;
    char * IP;
    int port;
    struct userInfo * next;
    int inChannel;
};

struct userInfo * createUsers() {
    struct userInfo * user = malloc(sizeof(struct userInfo));
    user->next = NULL;
    return user;
}

int getLoggedIn(struct userInfo * user) {
    return user->loggedIn;
}

char * getUsername(struct userInfo * user) {
    return user->username;
}

struct userInfo * getNext(struct userInfo * user) {
    return user->next;
}

int getIndex(struct userInfo * user) {
    return user->index;
}

int isInChannel(struct userInfo * user) {
    return user->inChannel;
}

void setChannel(struct userInfo * user, int channelIndex) {
    user->inChannel = channelIndex;
}

void newUser(struct userInfo * users, int index, char * IP, int port) {
    struct userInfo * current = users;
    struct userInfo * newUser = malloc(sizeof(struct userInfo));

    while(current->next != NULL) {
        current = current->next;
    }
    current->next = newUser;
    newUser->next = NULL;
    newUser->index = index;
    newUser->loggedIn = -1;
    newUser->inChannel = -1;
    newUser->port = port;
    newUser->username = malloc(MAX_USERNAME);
    newUser->conTime = malloc(sizeof(struct tm));
    newUser->IP = malloc(100);
    strcpy(newUser->IP, IP);
}


struct userInfo * searchByUsername(struct userInfo * users, char * username) {
    struct userInfo * current = users->next;

    while((current != NULL)&&(strcmp(current->username, username))) {
        current = current->next;
    }

    return current;
}

struct userInfo * searchByIndex(struct userInfo * users, int index) {
    struct userInfo * current = users->next;

    while((current != NULL)&&(current->index != index)) {
        current = current->next;
    }

    return current;
}

void deleteUser(int index, struct userInfo * users) {
    struct userInfo * current = users;
    struct userInfo * toDelete = malloc(sizeof(struct userInfo));

    while((current->next != NULL)&&(current->next->index != index)) {
        current = current->next;
    }
    if (current != NULL) {
        toDelete = current->next;
        current->next = current->next->next;
        free(toDelete);
    }
}

int nbUsers (struct userInfo * users) {
    int nbUsers = 0;
    struct userInfo * current = users;

    while(current->next != NULL) {
        current = current->next;
        nbUsers++;
    }
    return nbUsers;
}

void whois (char * buffer, char * username, struct userInfo * users) {
    printf("Dedans\n");
    fflush(stdout);
    struct userInfo * user = searchByUsername(users, username);

    memset(buffer, '\0', MAX_BUFFER_SIZE);

    if (user != NULL) {
        sprintf(buffer, "%s connected since %04d-%02d-%02d %02d:%02d:%02d with IP address %s and port number %i in %i\n", user->username,1900+user->conTime->tm_year,1+user->conTime->tm_mon,user->conTime->tm_mday,user->conTime->tm_hour,user->conTime->tm_min,user->conTime->tm_sec, user->IP, user->port, user->inChannel);
    } else {
        sprintf(buffer, "%s", "The user does not exist or is not connected\n");
    }
}

void who (char * buffer, struct userInfo * users, int channelIndex) {
    char * sentence = malloc(200*sizeof(char));
    struct userInfo * current = users->next;

    sprintf(sentence, "%s", "Online users are : ");

    while(current != NULL) {
        if(channelIndex == -1){
            strcat(sentence, "\n\t- ");
            strcat(sentence, current->username);
        }
        else if (isInChannel(current)==channelIndex){
            strcat(sentence, "\n\t- ");
            strcat(sentence, current->username);
        }

        current = current->next;
    }
    strcat(sentence, "\n");
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    sprintf(buffer, "%s", sentence);
}

void nick(char * buffer, struct userInfo * users, char * username, struct userInfo * currentUser) {
    struct userInfo * available = searchByUsername(users, username);
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    if (available == NULL) {
        strcpy(currentUser->username, username);
        sprintf(buffer, "Your new username is %s. Welcome back !\n", currentUser->username);
    } else {
        sprintf(buffer, "%s", "The username is already taken.\n");
    }
}

void loggedIn (char * buffer, struct userInfo * users, int rdwrSock, struct userInfo * currentUser) {
    char * username = malloc(MAX_USERNAME*sizeof(char));
    struct userInfo * available = malloc(sizeof(struct userInfo));
    time_t seconds = time(NULL);

    if (!strncmp(buffer, "/nick ", 6)) {
        sscanf(buffer, "/nick %s", username);
         available = searchByUsername(users, username);

        if (available == NULL) {
            strcpy(currentUser->username, username);
            currentUser->loggedIn = 1;
            currentUser->conTime = localtime(&seconds);

            memset(buffer, '\0', MAX_BUFFER_SIZE);
            sprintf(buffer, "Welcome on the chat %s", username);
        }
        else {
            memset(buffer, '\0', MAX_BUFFER_SIZE);
            sprintf(buffer, "%s", "This user is already connected\n");
        }

    } else {
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        sprintf(buffer, "%s", "Please logon with /nick <your pseudo>\n");
    }
}
