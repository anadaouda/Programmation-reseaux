#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>



int do_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock == -1) {
    perror("Socket");
    exit(EXIT_FAILURE);
  }
  return sock;
}

struct sockaddr_in init_serv_addr() {
  struct sockaddr_in sock_addr;

  memset(&sock_addr, '\0', sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(33000);
  inet_aton("127.0.0.1", &sock_addr.sin_addr);

  return sock_addr;
}

void do_bind(int sock, struct sockaddr_in sock_addr) {
  if (bind(sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1) {
    perror("Bind");
    exit(EXIT_FAILURE);
  }
}

void do_listen(int sock) {
  if (listen(sock, SOMAXCONN) == -1) {
    perror("Listen");
    exit(EXIT_FAILURE);
  }
}

int do_accept(int sock, struct sockaddr_in sock_addr) {
  int addrlen = sizeof(struct sockaddr);
  int rdwr_socket = accept(sock, (struct sockaddr *) &sock_addr, (socklen_t *)&addrlen);
  if (rdwr_socket == -1) {
    perror("Accept");
    exit(EXIT_FAILURE);
  }
  return rdwr_socket;
}


void do_write(int sock, char * message) {
  if (send(sock,message,strlen(message),0) == -1) {
    perror("Send");
    exit(EXIT_FAILURE);
  }
}

void do_read(int sock, char * buffer) {
  memset(buffer, '\0', strlen(buffer));
  int reception = recv(sock, buffer, SSIZE_MAX, 0);
    if (reception == -1) {
      perror("Read");
      exit(EXIT_FAILURE);
    }
    if (!strcmp(buffer,"/quit")) {
      printf("Au revoir\n");
      do_write(sock,"/quit");
      close(sock);
      exit(1);
    }
    printf("Le serveur a recu : %s\n", buffer);
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
      fprintf(stderr, "usage: RE216_SERVER 33000\n");
      //return 1;
  }
    //create the socket, check for validity!
    int sock = do_socket();
    printf("Numéro de socket : %i\n", sock);

    //init the serv_add structure
    struct sockaddr_in sock_addr = init_serv_addr();

    //perform the binding
    //we bind on the tcp port specified
    do_bind(sock, sock_addr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(sock);
    //accept connection from client

    int rdwr_socket = do_accept(sock, sock_addr);

    char * buffer = malloc(100);

    for (;;)
    {

        //read what the client has to say
        do_read(rdwr_socket, buffer);

        //we write back to the client
        do_write(rdwr_socket, buffer);

        //clean up client socket

    }

    //clean up server socket

    return 0;
}
