#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "header/constant.h"
#include "header/users.h"

struct channelInfo {
    int index;
    int nbUsers;
    char * name;
    struct channelInfo * next;
};

struct channelInfo * createChannel() {
    struct channelInfo * channel = malloc(sizeof(struct channelInfo));
    channel->next = NULL;
    return channel;
}

int nbChannels (struct channelInfo * channels) {
    int nbChannels = 0;
    struct channelInfo * current = channels;

    while(current->next != NULL) {
        current = current->next;
        nbChannels++;
    }

    return nbChannels;
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

void newChannel(struct channelInfo * channels, char * name, char * buffer) {
    struct channelInfo * verification = searchByChannelName(channels, name);

    if (verification == NULL) {
        struct channelInfo * current = channels;
        struct channelInfo * newChannel = malloc(sizeof(struct channelInfo));

        while(current->next != NULL) {
            current = current->next;
        }

        current->next = newChannel;
        newChannel->next = NULL;
        newChannel->index = nbChannels(channels);
        newChannel->nbUsers = 0;
        newChannel->name = malloc(MAX_USERNAME);
        strcpy(newChannel->name, name);
        sprintf(buffer, "%s channel has been created\n", name);
    }
    else {
        sprintf(buffer,"%s", "channel name already exists\n");
    }

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

void join(struct channelInfo * channels, char * name, struct userInfo * user,struct userInfo * users, char * buffer) {
    struct channelInfo * channel = searchByChannelName(channels, name);
    if (channel != NULL) {
        setChannel(user, channel->index);
        channel->nbUsers++;
        sprintf(buffer, "You have joined %s\n", name);
    }
    else {
        sprintf(buffer, "the channel does'nt exist\n");
    }
}

void quitChannel(struct channelInfo * channels, char * name, struct userInfo * user, char * buffer) {
    struct channelInfo * channel = searchByChannelName(channels, name);
    if ((channel !=NULL)&&(getChannelIndex(channel) == isInChannel(user))) {
        setChannel(user, -1);
        channel->nbUsers--;
        sprintf(buffer, "You left %s\n", name);
        if(channel->nbUsers == 0) {
            deleteChannel(channel->index,channels);
        }
    }
    else if (channel == NULL) {
        sprintf(buffer, "The channel doesn't exist\n");
    }
    else {
        sprintf(buffer, "You are not in this channel\n");
    }
}

void channelList(char * buffer, struct channelInfo * channels) {
    char * sentence = malloc(200*sizeof(char));
    struct channelInfo * current = channels->next;
    if (!nbChannels(channels)) {
        sprintf(sentence, "%s", "No active channels. You can create one with command :\n/createchannel channel_name");
    } else {
        sprintf(sentence, "%s", "Active channels are : ");
    }
    while(current != NULL) {
        strcat(sentence, "\n\t- ");
        strcat(sentence, current->name);
        current = current->next;
    }
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    sprintf(buffer, "%s", sentence);
}
