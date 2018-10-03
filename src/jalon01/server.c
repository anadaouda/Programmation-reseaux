#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <poll.h>

#define MAX_BUFFER_SIZE 100
#define MAX_FD 5


int do_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

struct sockaddr_in init_serv_addr(char ** argv) {
  struct sockaddr_in sock_addr;

  memset(&sock_addr, '\0', sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(atoi(argv[1]));
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


void do_write(int rdwr_sock, char * message) {
  printf("Socket send : %i\n\n", rdwr_sock);
  if (send(rdwr_sock,message,strlen(message),0) == -1) {
    perror("Send");
    exit(EXIT_FAILURE);
  }
}

void closeInPoll (int rdwr_sock, struct pollfd structPoll[]) {
  int i = 1;

  while ((i < MAX_FD) && (structPoll[i].fd != rdwr_sock)) {
    i++;
  }

  structPoll[i].events = 0;
}

void do_read(int rdwr_sock, int sock, char * buffer, struct pollfd structPoll[]) {
  memset(buffer, '\0', strlen(buffer));
  int reception = recv(rdwr_sock, buffer, SSIZE_MAX, 0);
    if (reception == -1) {
      perror("Read");
      exit(EXIT_FAILURE);
    }
    if (!strcmp(buffer,"/quit")) {
      closeInPoll(rdwr_sock, structPoll);
      do_write(rdwr_sock,"You will be terminated");
      //free(buffer);
      close(rdwr_sock);
    }
    printf("[%i] : %s\n",rdwr_sock, buffer);
}

int searchPollIn(struct pollfd structPoll[]) {
  int i = 1;

  while ((i < MAX_FD) && (structPoll[i].events == POLLIN)) {
    i++;
  }
  return i;
}


int main(int argc, char** argv) {

  if (argc != 2)
  {
      fprintf(stderr,"Veuillez indiquer un numero de port\n\n");
      return 1;
  } else {
    printf("usage: RE216_SERVER %s\n", argv[1]);
    fflush(stdout);
  }
    //create the socket, check for validity!
    int sock = do_socket();

    //init the serv_add structure
    struct sockaddr_in sock_addr = init_serv_addr(argv);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(sock, sock_addr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(sock);
    //accept connection from client


    struct pollfd structPoll[MAX_FD];
    structPoll[0].fd = sock;
    structPoll[0].events = POLLIN;



    int rdwr_sock;
    int i;
    int resPoll;

    char * buffer = malloc(MAX_BUFFER_SIZE);

    while(1)
    {

      resPoll = poll(structPoll, MAX_FD, -1);

      if (structPoll[0].revents == POLLIN) {
        if (resPoll >= MAX_FD) {
          rdwr_sock = do_accept(sock, sock_addr);
          do_write(rdwr_sock, "/serverOverload");
          close(rdwr_sock);
        }
        else if (resPoll == -1) {
          perror("Poll");
          exit(EXIT_FAILURE);
        }
        else {
           rdwr_sock = do_accept(sock, sock_addr);
           i = searchPollIn(structPoll);
           structPoll[i].fd = rdwr_sock;
           structPoll[i].events = POLLIN;
           do_write(structPoll[i].fd, "connecte");

        }
      }
//      else {
        for (i = 1; i < MAX_FD; i++) {
          if (structPoll[i].revents == POLLIN) {
            do_read(structPoll[i].fd, sock, buffer, structPoll);
              do_write(structPoll[i].fd, buffer);
//      }
    }
  //   else if (structPoll[i].revents == POLLHUP) {
  //     while(1);
  //     //do_write(structPoll[0].fd, buffer);
  // }

        //clean up client socket

    }

    //clean up server socket
}
    return 0;

}
