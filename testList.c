#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct userInfo {
    int index;
    int loggedIn;
    char * username;
    char * conTime;
    char * IP;
    int port;
    struct userInfo * next;
};

void newUser(struct userInfo * users, int index, char * username, char * conTime, char * IP, int port) {
    struct userInfo * current = users;
    struct userInfo * newUser = malloc(sizeof(struct userInfo));
    
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = newUser;
    newUser->next = NULL;
    newUser->index = index;
    newUser->port = port;
    newUser->username = malloc(100);
    strcpy(newUser->username, username);
    newUser->conTime = malloc(100);
    strcpy(newUser->conTime, conTime);
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

void affiche(struct userInfo * users) {
    struct userInfo * current = users->next;
    while(current != NULL) {
        printf("index = %i\n", current->index);
        current = current->next;
    }
}

int main(int argc, char const *argv[]) {
    struct userInfo * users = malloc(sizeof(struct userInfo));
    users->next = NULL;

    newUser(users, 1, "username","conTime", "IP", 12);
    newUser(users, 2, "username","conTime", "IP", 12);
    newUser(users, 3, "username","conTime", "IP", 12);
    newUser(users, 4, "username","conTime", "IP", 12);

    affiche(users);

    deleteUser(3, users);

    affiche(users);

    if (searchByUsername(users, "usernae") != NULL) {
        printf("Il est dedans");
    } else {
        printf("Il est pas dedans");
    }

    return 0;
}
