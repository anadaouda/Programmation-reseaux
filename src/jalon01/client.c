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

struct sockaddr_in get_addr_info() {
    struct sockaddr_in sock_server;

    memset(&sock_server, '\0', sizeof(sock_server));
    sock_server.sin_family = AF_INET;
    sock_server.sin_port = htons(33000);
    inet_aton("127.0.0.1", &sock_server.sin_addr);

    return sock_server;
}

int do_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    perror("Socket");
    exit(EXIT_FAILURE);
  }
  return sock;
}

void do_connect(int sock, struct sockaddr * sock_server) {
  if (connect(sock, sock_server, sizeof(struct sockaddr_in)) == -1) {
    perror("Connect");
    exit(EXIT_FAILURE);
  }
}


char * message_to_send() {
  char * chaine = malloc(100*sizeof(char)); /// Changer un truc ici

  printf("Message à envoyer :\n");
  scanf("%s", chaine);

  return chaine;
}

void do_send(int sock) {
  char * buffer = message_to_send();
  if (send(sock, buffer, (size_t)sizeof(buffer)/sizeof(char), 0) == -1) {
    perror("Send");
    exit(EXIT_FAILURE);
  }
}

void do_receive(int sock, void * buffer) {
  int reception = recv(sock, buffer, SSIZE_MAX, 0);
    if (reception == -1) {
      perror("Recieve");
      exit(EXIT_FAILURE);
    }
    if (!strcmp(buffer,"/quit")) {
      printf("Au revoir\n");
      close(sock);
      free(buffer);
      exit(1);
    }
}

void handle_client_message(int sock) {
  char * buffer = malloc(100);
  do_receive(sock, buffer);
  printf("Le serveur à renvoyé : %s\n",buffer);

}

int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        //return 1;
    }


struct sockaddr_in sock_host = get_addr_info();
//get the socket
int sock = do_socket();

//connect to remote socket
do_connect(sock, (struct sockaddr * )&sock_host);

//get user input
while(1){
  //send message to the server
  do_send(sock);

  handle_client_message(sock);

}



    return 0;


}
