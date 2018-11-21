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

<<<<<<< HEAD
char * getFilename(char * path) {
    int i, j;
    char * result = malloc(strlen(path));
    int position = 0;
    for (i = 0 ; i < strlen(path); i++){
        if(path[i] == '/'){
            position = i+1;
        }
    }

    strcpy(result, path + position);
    return result;
}


struct threadArgs {
=======

struct args {
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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

<<<<<<< HEAD
struct sockaddr_in get_addr_info(char* IP, int port) {
=======
struct sockaddr_in get_addr_info(char** argv) {
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
    struct sockaddr_in sockServerAddr;

    memset(&sockServerAddr, '\0', sizeof(sockServerAddr));
    sockServerAddr.sin_family = AF_INET;
<<<<<<< HEAD
    sockServerAddr.sin_port = htons(port);
    inet_aton(IP, &sockServerAddr.sin_addr);
=======
    sockServerAddr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &sockServerAddr.sin_addr);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82

    return sockServerAddr;
}

<<<<<<< HEAD
int do_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
=======
int do_socket(int something) {
    int sock = socket(AF_INET, SOCK_STREAM, something);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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
<<<<<<< HEAD
    printf("Connecting to client...");
=======
    printf("Connecting to server...");
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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
<<<<<<< HEAD
    fgets(input, MAX_BUFFER_SIZE*sizeof(char), stdin);
=======

    fgets(input,MAX_BUFFER_SIZE*sizeof(char),stdin);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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
<<<<<<< HEAD
    struct threadArgs arguments = *(struct threadArgs *)args;
=======
    struct args arguments = *(struct args *)args;
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
    char * buffer = arguments.buffer;
    int sock = arguments.sock;
    int * canSend = arguments.canSend;
    pthread_cond_t * cond = arguments.cond;
    pthread_mutex_t * lock = arguments.lock;

    while(1) {
        message_to_send(buffer);
<<<<<<< HEAD

        if (!strncmp(buffer, "/send ", 6)) {
            char * path = malloc(MAX_BUFFER_SIZE);
            char * username = malloc(MAX_USERNAME);

            sscanf(buffer, "/send %s %s", username, path);
            FILE * toSend = fopen(path, "r");

            if (toSend != NULL) {
                int sockP2P = do_socket();
                struct sockaddr_in sockAddr = init_P2P_addr();
                do_bind(sockP2P, sockAddr);
                do_listen(sockP2P);
                sprintf(buffer, "%s %i", buffer, ntohs(sockAddr.sin_port));
=======
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
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
                do_send(buffer, sock);

                pthread_mutex_lock(lock);
                while(*canSend == 0) {
                    pthread_cond_wait(cond, lock);
                }
<<<<<<< HEAD

                if (*canSend == 1) {
                    int wrSockP2P = do_accept(sockP2P, sockAddr);

                    char * filename = getFilename(path);
                    fseek(toSend, 0, SEEK_END);
                    int fileSize = ftell(toSend);
                    rewind(toSend);

                    char * file2Send = malloc((fileSize + strlen(filename))*sizeof(char)+2);
                    char * file = malloc(fileSize*sizeof(char));

                    fread(file, sizeof(char), fileSize, toSend);

                    sprintf(file2Send, "%s %s", filename, file);

                    do_send(file2Send, wrSockP2P);

                    free(file2Send);

                }
=======
                /*
                if (*canSend == 1) {
                    do_accept()
                    do_send(fileName);
                    do_send(file);
                }
                */
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
                *canSend = 0;
                pthread_mutex_unlock(lock);

                close(sockP2P);
<<<<<<< HEAD
            } else {
                printf("The file does not exist\n");
                fflush(stdout);
=======
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
            }
        } else {
            do_send(buffer, sock);
        }

        if (!strcmp(buffer, "/quit\n")) {
<<<<<<< HEAD
            return NULL;
        }
=======
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

>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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
<<<<<<< HEAD
    struct threadArgs arguments = *(struct threadArgs *)args;
=======
    struct args arguments = *(struct args *)args;
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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
<<<<<<< HEAD
            //strcmp(desc, "ok")==0 ? *canSend = YES : *canSend = NO;
            if(!strcmp(desc, "ok")) {
=======
            if(!strcmp(buffer, "ok")) {
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
                *canSend = 1;
            }
            else {
                *canSend = -1;
            }
            pthread_cond_signal(cond);
            pthread_mutex_unlock(lock);
<<<<<<< HEAD
        }
        else if (!strncmp(buffer, "[SERVER] : /recvFile ", 20)) {

            char * IP = malloc(15*sizeof(char));
            int portP2P;

            sscanf(buffer,"[SERVER] : /recvFile %s %i", IP, &portP2P);
            struct sockaddr_in sockAddrP2P = get_addr_info(IP, portP2P);
            int sockP2P = do_socket();
            do_connect(sockP2P, (struct sockaddr * )&sockAddrP2P);


            char * fileReceived = malloc(200);
            do_receive(sockP2P, fileReceived); //nom du fichier

            char * filename = malloc(100);
            sscanf(fileReceived, "%s ", filename);
            char * file = &fileReceived[strlen(filename)+1];

            printf("filename %s\n", filename);
            fflush(stdout);
            printf("file %s\n", file);
            fflush(stdout);


            char * path = malloc((strlen(INBOX_DIR)+strlen(filename) + 1)*sizeof(char));
            sprintf(path, "%s/%s", INBOX_DIR, filename);
            FILE * toSave = fopen(path, "a");
            fprintf(toSave, "%s", file);
            fclose(toSave);
            /*
            save_file();
            */


            printf("%s saved in %s\n", filename, INBOX_DIR);
            fflush(stdout);

            close(sockP2P);
=======
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
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
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

<<<<<<< HEAD
    struct sockaddr_in sockServerAddr = get_addr_info(argv[1], atoi(argv[2]));
    int sock = do_socket();
=======
    struct sockaddr_in sockServerAddr = get_addr_info(argv);
    int sock = do_socket(0);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
    do_connect(sock, (struct sockaddr * )&sockServerAddr);
    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));

    pthread_mutex_t lock;
    pthread_cond_t cond;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&lock, NULL);

    int canSend = 0;

    pthread_t rdThread;
    pthread_t wrThread;

<<<<<<< HEAD
    struct threadArgs args = {sock, buffer, &canSend, &cond, &lock};

    pthread_create(&rdThread, NULL, readThread, (void *)&args);
    pthread_create(&wrThread, NULL, writeThread, (void *)&args);
=======
    struct args arguments = {sock, buffer, &canSend, &cond, &lock};

    pthread_create(&rdThread, NULL, readThread, (void *)&arguments);
    pthread_create(&wrThread, NULL, writeThread, (void *)&arguments);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82

    pthread_join(rdThread, NULL);

    return 0;
}
<<<<<<< HEAD
=======

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
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
