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

#include "header/constant.h"

struct userInfo {
    int index;
    int loggedIn;
    char * username;
    struct tm * conTime;
    char * IP;
    int port;
    struct userInfo * next;
};

struct userInfo * createUser() {
    struct userInfo * user = malloc(sizeof(struct userInfo));
    user->next = NULL;
    return user;
}

char * getUsername(struct userInfo * user) {
    return user->username;
}

int getYear(struct userInfo * user) {
    return 1900+user->conTime->tm_year;
}

int getMonth(struct userInfo * user) {
    return 1+user->conTime->tm_mon;
}

int getDay(struct userInfo * user) {
    return user->conTime->tm_mday;
}

int getHour(struct userInfo * user) {
    return user->conTime->tm_hour;
}

int getMinute(struct userInfo * user) {
    return user->conTime->tm_min;
}

int getSecond(struct userInfo * user) {
    return user->conTime->tm_sec;
}

char * getIP(struct userInfo * user) {
    return user->IP;
}

int getPort(struct userInfo * user) {
    return user->port;
}

int getIndex(struct userInfo * user) {
    return user->index;
}

int getLoggedIn(struct userInfo * user) {
    return user->loggedIn;
}

struct tm * getConTime(struct userInfo * user) {
    return user->conTime;
}

struct userInfo * getNext(struct userInfo * user) {
    return user->next;
}

void setLoggedIn(struct userInfo * user, int status) {
    user->loggedIn = status;
}
void setConTime(struct userInfo * user, struct tm * conTime) {
    user->conTime = conTime;
}
void setNext(struct userInfo * user, struct userInfo * next) {
    user->next = next;
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
    newUser->port = port;
    newUser->username = malloc(MAX_USERNAME);
    newUser->conTime = malloc(sizeof(struct tm));
    newUser->IP = malloc(100);
    strcpy(newUser->IP, IP);
}

void addUser(struct userInfo * user, struct userInfo * users) {
    struct userInfo * current = users;

    while(current->next != NULL) {
        current = current->next;
    }
    current->next = user;
    user->next = NULL;
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
