#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "header/constant.h"
#include "header/users.h"

struct channelInfo {
    int index;
    struct userInfo * channelUsers;
    char * name;
    struct channelInfo * next;
};

struct channelInfo * createChannel() {
    struct channelInfo * channel = malloc(sizeof(struct channelInfo));
    channel->next = NULL;
    return channel;
}

char * getChannelName(struct channelInfo * channel) {
    return channel->name;
}

int getChannelIndex(struct channelInfo * channel) {
    return channel->index;
}

struct channelInfo * getChannelNext(struct channelInfo * channel) {
    return channel->next;
}

void setChannelNext(struct channelInfo * channel, struct channelInfo * next) {
    channel->next = next;
}

void newChannel(struct channelInfo * channels, int index, char * name, char * buffer) {
    struct channelInfo * verification = searchByChannelName(channels, name);

    if (verification == NULL) {
        struct channelInfo * current = channel;
        struct channelInfo * newChannel = malloc(sizeof(struct channelInfo));

        while(current->next != NULL) {
            current = current->next;
        }

        current->next = newChannel;
        newChannel->next = NULL;
        newChannel->index = index;
        newChannel->name = malloc(MAX_USERNAME);
        strcpy(newChannel->name, name);
        newChannel->users = createUser();
        sprintf(buffer, "%s channel has been created\n", name);
    }
    else {
        sprintf(buffer,"%s", "channel name already exists\n");
    }

}

struct channelInfo * searchByChannelName(struct channelInfo * channel, char * name) {
    struct channelInfo * current = channel->next;

    while((current != NULL)&&(strcmp(current->name,name))) {
        current = current->next;
    }

    return current;
}

struct channelInfo * searchChannelByIndex(struct channelInfo * channel, int index) {
    struct channelInfo * current = channel->next;

    while((current != NULL)&&(current->index != index)) {
        current = current->next;
    }
    return current;
}

int channelNbUser(struct channelInfo * channel){
    return nbUsers(channel->channelUsers);
}

void deleteChannel(int index, struct channelInfo * channel) {
    struct channelInfo * current = channel;
    struct channelInfo * toDelete = malloc(sizeof(struct channelInfo));

    while((current->next != NULL)&&(current->next->index != index)) {
        current = current->next;
    }
    if (current != NULL) {
        toDelete = current->next;
        current->next = current->next->next;
        free(toDelete);
    }
}

void join(struct channelInfo * channels, char * name, struct userInfo * user, char * buffer) {
    struct channelInfo * channel = searchByChannelName(channels, name);

    if (channel != NULL) {
        addUser(user, channel->channelUsers);
        sprintf(buffer, "You have joined %s\n", name);
    }
    else {
        sprintf(buffer, "the channel doesnt exist\n");
    }
}
