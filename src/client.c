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
    int * canSend;
    pthread_cond_t * cond;
    pthread_mutex_t * lock;
};

struct sockaddr_in init_P2P_addr() {
    struct sockaddr_in sockAddr;

    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(33001);
    inet_aton("127.0.0.1", &sockAddr.sin_addr);

    return sockAddr;
}

void do_bind(int sockP2P, struct sockaddr_in sockAddr) {
    if (bind(sockP2P, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) == -1) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
}

void do_listen(int sockP2P) {
    if (listen(sockP2P, MAX_FD) == -1) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

int do_accept(int sockP2P, struct sockaddr_in sockAddr) {
    int addrLen = sizeof(struct sockaddr);
    int rdwrSock = accept(sockP2P, (struct sockaddr *) &sockAddr, (socklen_t *)&addrLen);
    if (rdwrSock == -1) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    return rdwrSock;
}

struct sockaddr_in get_addr_info(char** argv) {
    struct sockaddr_in sockServerAddr;

    memset(&sockServerAddr, '\0', sizeof(sockServerAddr));
    sockServerAddr.sin_family = AF_INET;
    sockServerAddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &sockServerAddr.sin_addr);

    return sockServerAddr;
}

int do_socket(int something) {
    int sock = socket(AF_INET, SOCK_STREAM, something);
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
    int * canSend = arguments.canSend;
    pthread_cond_t * cond = arguments.cond;
    pthread_mutex_t * lock = arguments.lock;

    while(1) {
        message_to_send(buffer);
        if (!strncmp(buffer, "/send ", 6)) {

            char * path = malloc(MAX_BUFFER_SIZE);
            //sscanf(buffer, "/send %s %s", NULL, path);
            FILE * toSend = fopen("fichier.txt", "r");
            if (toSend != NULL) {
                int sockP2P = do_socket(IPPROTO_TCP);
                struct sockaddr_in sockAddr = init_P2P_addr();
                do_bind(sockP2P, sockAddr);
                do_listen(sockP2P);
                sprintf(buffer, "%s %i", buffer, sockAddr.sin_port);
                do_send(buffer, sock);

                pthread_mutex_lock(lock);
                while(*canSend == 0) {
                    pthread_cond_wait(cond, lock);
                }
                /*
                if (*canSend == 1) {
                    do_accept()
                    do_send(fileName);
                    do_send(file);
                }
                */
                *canSend = 0;
                pthread_mutex_unlock(lock);

                close(sockP2P);
            }
        } else {
            do_send(buffer, sock);
        }

        if (!strcmp(buffer, "/quit\n")) {
            //pthread_join(sdFile, NULL);
            return NULL;
        }

            //verifier que le fichier existe
            //creer la socket
            //bind et listen
            //envoyer la requete send pour verification plus adresse et IP
            //attendre que le serveur dise que c'est bon (avec une condition puis remettre la cond a 0)
            //si oui envoyer le port et l'adresse IP
            //faire un accept
            //envoyer la fichier et fermer la socket

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
    int * canSend = arguments.canSend;
    pthread_cond_t * cond = arguments.cond;
    pthread_mutex_t * lock = arguments.lock;

    char * desc = malloc(22);

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
        }
        else if (!strncmp(buffer, "[SERVER] : /sendCheck ", 17)) {
            memset(desc, '\0', strlen(desc));
            sscanf(buffer,"[SERVER] : /sendCheck %s", desc);

            pthread_mutex_lock(lock);
            if(!strcmp(buffer, "ok")) {
                *canSend = 1;
            }
            else {
                *canSend = -1;
            }
            pthread_cond_signal(cond);
            pthread_mutex_unlock(lock);
            // c'est là qu'il faut modifier la variable de condition
            //recuperer desc
            //modifier la variable peut envoyer et le signaler
        }
        else if (!strncmp(buffer, "[SERVER] : /rcvFile ", 20)) {
            memset(desc, '\0', strlen(desc));
            /*
            sscanf(desc,"%s %i", IP, portP2P);

            get_addr_info();
            int sockP2P = do_socket();
            do_connect();
            do_receive(fileName); //nom du fichier
            do_receive(file);// fichier
            save_file();
            close(sockP2P);
            */
            //recuperer desc
            //ouvrir une socket
            //se connecter
            //recevoir et enregistrer le fichier
            //fermer la socket
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

    struct sockaddr_in sockServerAddr = get_addr_info(argv);
    int sock = do_socket(0);
    do_connect(sock, (struct sockaddr * )&sockServerAddr);
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));

    pthread_mutex_t lock;
    pthread_cond_t cond;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&lock, NULL);

    int canSend = 0;

    pthread_t rdThread;
    pthread_t wrThread;

    struct args arguments = {sock, buffer, &canSend, &cond, &lock};

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
