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

#define MAX_BUFFER_SIZE 100

struct sockaddr_in get_addr_info(char** argv) {
    struct sockaddr_in sock_server;

    memset(&sock_server, '\0', sizeof(sock_server));
    sock_server.sin_family = AF_INET;
    sock_server.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &sock_server.sin_addr);

    return sock_server;
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

void do_connect(int sock, struct sockaddr * sock_server) {
  if (connect(sock, sock_server, sizeof(struct sockaddr_in)) == -1) {
    perror("Connect");
    exit(EXIT_FAILURE);
  }
}

//get user input
void message_to_send(char * buffer) {
  memset(buffer, '\0', strlen(buffer));
  printf("Message à envoyer :\n");
  scanf("%s", buffer);
}

void do_send(int sock, char * buffer) {
  message_to_send(buffer);

  if (send(sock, buffer, strlen(buffer), 0) == -1) {
    perror("Send");
    exit(EXIT_FAILURE);
  }
}

void do_receive(int sock, char * buffer) {
  int reception = recv(sock, buffer, SSIZE_MAX, 0);
    if (reception == -1) {
      perror("Recieve");
      exit(EXIT_FAILURE);
    }
}

void handle_client_message(int sock, char * buffer) {
  do_receive(sock, buffer);
  printf("[SERVER] : %s\n\n",buffer);
  fflush(stdout);

  if (!strcmp(buffer, "/serverOverload")) {
    printf("Server cannot accept incoming connections anymore. Try again later\n");
    close(sock);
    free(buffer);
    exit(1);
  }
  if (!strcmp(buffer,"You will be terminated")) {
    close(sock);
    free(buffer);
    printf("Disconnected\n");
    fflush(stdout);
    exit(1);
  }

}

int main(int argc,char** argv)
{

  if (argc != 3)
  {
      fprintf(stderr,"Veuillez indiquer un numero une adresse IP et un numero de port\n\n");
      return 1;
  } else {
    printf("usage: RE216_CLIENT %s %s\n", argv[1], argv[2]);
    fflush(stdout);
  }


struct sockaddr_in sock_server = get_addr_info(argv);
//get the socket
int sock = do_socket();

//connect to remote socket
do_connect(sock, (struct sockaddr * )&sock_server);

char * buffer = malloc(MAX_BUFFER_SIZE);

while(1) {
  //send message to the server
  handle_client_message(sock, buffer);

  do_send(sock, buffer);


}

    return 0;


}
