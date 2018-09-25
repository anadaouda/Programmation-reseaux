#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



int do_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock == -1) {
    perror("Socket invalide");
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
    perror("bind invalide");
    exit(EXIT_FAILURE);
  }
}

void do_listen(int sock) {
  if (listen(sock, SOMAXCONN) == -1) {
    perror("listen invalide");
    exit(EXIT_FAILURE);
  }
}

int do_accept(int sock, struct sockaddr_in sock_addr) {
  int addrlen = sizeof(struct sockaddr);
  int readSocket = accept(sock, (struct sockaddr *) &sock_addr, (socklen_t *)&addrlen);
  if ( readSocket == -1) {
    perror("accept invalide");
    exit(EXIT_FAILURE);
  }
  return readSocket;
}


void do_write(int sock, char * msg, size_t len) {
  if (send(sock,msg,len,0) == -1) {
    perror("send invalide");
    exit(EXIT_FAILURE);
  }
}

void do_read(int sock, char * buf) {
  int reception = recv(sock, buf, 10240, 0);
    if (reception == -1) {
      perror("Read marche pas");
      exit(EXIT_FAILURE);
    }
    if (!strcmp(buf,"/quit")) {
      printf("Au revoir\n");
      do_write(sock,"/quit", 10);
      close(sock);
      exit(1);
    }
    printf("Le serveur a recu : %s\n", buf);
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
    int readSocket;
    readSocket = do_accept(sock, sock_addr);

    char * buf = malloc(1000);
    for (;;)
    {


        //read what the client has to say
        do_read(readSocket, buf);

        //we write back to the client
        do_write(readSocket, buf, (size_t)sizeof(buf)/sizeof(char));

        //clean up client socket
    }

    //clean up server socket

    return 0;
}
