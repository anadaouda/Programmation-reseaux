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

struct args {
    int sock;
    char * buffer;
};

struct sockaddr_in get_addr_info(char** argv) {
    struct sockaddr_in sockServerAddr;

    memset(&sockServerAddr, '\0', sizeof(sockServerAddr));
    sockServerAddr.sin_family = AF_INET;
    sockServerAddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &sockServerAddr.sin_addr);

    return sockServerAddr;
}

int do_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;

    if (sock == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("ERROR setting socket options");
    }
    return sock;
}

void do_connect(int sock, struct sockaddr * sockServerAddr) {
    printf("Connecting to server...");
    fflush(stdout);
    if (connect(sock, sockServerAddr, sizeof(struct sockaddr_in)) == -1) {
        perror("\nConnect");
        exit(EXIT_FAILURE);
    }
    printf("done !\n\n");
    fflush(stdout);
}

//get user input
void message_to_send(char * input) {
    memset(input, '\0', strlen(input));

    fgets(input,MAX_BUFFER_SIZE*sizeof(char),stdin);
}

void do_send(char * buffer, int sock) {
    int strSent;
    int inputLen = strlen(buffer);

    do {
        strSent = send(sock,&inputLen,sizeof(int),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != sizeof(int));

    do {
        strSent = send(sock,buffer,strlen(buffer),0);
        if (strSent == -1) {
            perror("Send");
            exit(EXIT_FAILURE);
        }
    } while (strSent != strlen(buffer));
}

void * writeThread(void * args) {
    struct args arguments = *(struct args *)args;
    char * buffer = arguments.buffer;
    int sock = arguments.sock;

    while(1) {
        message_to_send(buffer);
        do_send(buffer, sock);

        if (!strcmp(buffer, "/quit\n")) {
            //pthread_join(sdFile, NULL);
            return NULL;
        }
        /*
        else if (strncmp(buffer, "/send ", 6)) {
            //verifier que le fichier existe
            //creer la socket
            //bind et listen
            //envoyer la requete send pour verification plus adresse et IP
            //attendre que le serveur dise que c'est bon (avec une condition puis remettre la cond a 0)
            //si oui envoyer le port et l'adresse IP
            //faire un accept
            //envoyer la fichier et fermer la socket
        }
        */
    }
    return NULL;
}

void do_receive(int sock, char * buffer) {
        int strReceived, strSizeToReceive;
        do {
            strReceived = recv(sock, &strSizeToReceive, sizeof(int), 0);
            if (strReceived == -1) {
                perror("Receive");
                exit(EXIT_FAILURE);
            }
        } while (strReceived != sizeof(int));

        do {
            strReceived = recv(sock, buffer, SSIZE_MAX, 0);
            if (strReceived == -1) {
                perror("Receive");
                exit(EXIT_FAILURE);
            }
        } while (strReceived != strSizeToReceive);
}

void * readThread(void * args) {
    struct args arguments = *(struct args *)args;
    char * buffer = arguments.buffer;
    int sock = arguments.sock;

    while(1) {
        do_receive(sock, buffer);
        printf("%s\n",buffer);
        fflush(stdout);

        if (!strcmp(buffer, "[SERVER] : /serverOverload")) {
            printf("Server cannot accept incoming connections anymore. Try again later\n");
            close(sock);
            free(buffer);
            exit(1);
        }
        else if (!strncmp(buffer,"[SERVER] : You will be terminated",33)) {
            close(sock);
            free(buffer);
            printf("Disconnected\n");
            fflush(stdout);
            exit(1);
        }/*
        else if (!strncmp(buffer, "[SERVER] : /send ", 17)) {
            //recuperer desc
            //modifier la variable peut envoyer et le signaler
        }
        else if (!strncmp(buffer, "[SERVER] : /rcvFile ", 20)) {
            //recuperer desc
            //ouvrir une socket
            //se connecter
            //recevoir et enregistrer le fichier
            //fermer la socket
        }
        */
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

    struct sockaddr_in sockServerAddr = get_addr_info(argv);
    int sock = do_socket();
    do_connect(sock, (struct sockaddr * )&sockServerAddr);
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));

    pthread_t rdThread;
    pthread_t wrThread;

    struct args arguments = {sock, buffer};

    pthread_create(&rdThread, NULL, readThread, (void *)&arguments);
    pthread_create(&wrThread, NULL, writeThread, (void *)&arguments);

    pthread_join(rdThread, NULL);

    return 0;
}

/*
void * sendFile(void * args) {
    struct args arguments = *(struct args2 *)args;

    char * buffer = arguments.buffer;
    int sock = arguments.sock;

    wrSock = accept(); //appel bloquant normalement
    fopen(file);
    read(file); dans le buffer
    do_send(buffer, wrSock)
    return NULL;
}
*/
