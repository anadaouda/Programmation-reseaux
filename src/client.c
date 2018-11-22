#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#include "header/constant.h"
#include "header/P2P.h"
#include "header/socketSetup.h"


struct threadArgs {
    int sock;
    char * buffer;
    int * canSend;
    pthread_cond_t * cond;
    pthread_mutex_t * lock;
};

//get user input
void message_to_send(char * input) {
    memset(input, '\0', strlen(input));
    fgets(input, MAX_BUFFER_SIZE*sizeof(char), stdin);
}

void * writeThread(void * args) {

    struct threadArgs arguments = *(struct threadArgs *)args;

    char * buffer = arguments.buffer;
    int sock = arguments.sock;
    int * canSend = arguments.canSend;
    pthread_cond_t * cond = arguments.cond;
    pthread_mutex_t * lock = arguments.lock;

    while(1) {
        message_to_send(buffer);

        if (!strncmp(buffer, "/send ", 6)) {
            sendFile(buffer,sock,lock, cond,canSend);

        } else {
            do_send(buffer, sock, NULL);
        }

        if (!strcmp(buffer, "/quit\n")) {
            return NULL;
        }

    }
    return NULL;
}


void * readThread(void * args) {

    struct threadArgs arguments = *(struct threadArgs *)args;

    char * buffer = arguments.buffer;
    int sock = arguments.sock;
    int * canSend = arguments.canSend;
    pthread_cond_t * cond = arguments.cond;
    pthread_mutex_t * lock = arguments.lock;

    char * desc = malloc(22);

    while(1) {
        do_receive(sock, buffer, CLIENT);



        if (!strcmp(buffer, "[SERVER] : /serverOverload")) {
            printf("Server cannot accept incoming connections anymore. Try again later\n");
            close(sock);
            free(buffer);
            exit(1);
        }
        else if (!strncmp(buffer,"[SERVER] : You will be terminated",33)) {
            printf("%s\n",buffer);
            fflush(stdout);
            close(sock);
            free(buffer);
            printf("Disconnected\n");
            fflush(stdout);
            exit(1);

        } else if (!strncmp(buffer, "[SERVER] : /sendCheck ", 17)) {
            memset(desc, '\0', strlen(desc));
            sscanf(buffer,"[SERVER] : /sendCheck %s", desc);

            pthread_mutex_lock(lock);
            strcmp(desc, "ok") == 0 ? (*canSend = YES) : (*canSend = NO);
            pthread_cond_signal(cond);
            pthread_mutex_unlock(lock);

        } else if (!strncmp(buffer, "[SERVER] : /recvFile ", 20)) {
            recvFile(buffer);
            do_send(buffer, sock, NULL);

        } else {
            printf("%s\n",buffer);
            fflush(stdout);
            
        }
    }
    return NULL;
}


int main(int argc,char** argv) {

    if (argc != 3) {
        fprintf(stderr,"Veuillez indiquer un numero une adresse IP et un numero de port\n\n");
        return 1;
    } else {
        printf("usage: RE216_CLIENT %s %s\n", argv[1], argv[2]);
        fflush(stdout);
    }


    struct sockaddr_in sockServerAddr = get_addr_info(argv[1], atoi(argv[2]));
    int sock = do_socket();

    do_connect(sock, (struct sockaddr * )&sockServerAddr);
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));

    pthread_mutex_t lock;
    pthread_cond_t cond;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&lock, NULL);

    int canSend = 0;

    pthread_t rdThread;
    pthread_t wrThread;


    struct threadArgs args = {sock, buffer, &canSend, &cond, &lock};

    pthread_create(&rdThread, NULL, readThread, (void *)&args);
    pthread_create(&wrThread, NULL, writeThread, (void *)&args);


    pthread_join(rdThread, NULL);

    return 0;
}
