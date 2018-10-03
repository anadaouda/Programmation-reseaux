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
#define MAX_FD 20


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
  int strSend;
  int lenMessage = strlen(message);

  do {
    strSend = send(rdwr_sock,&lenMessage,sizeof(int),0);
    if (strSend == -1) {
      perror("Send");
      exit(EXIT_FAILURE);
    }
  } while (strSend != sizeof(int));

  do {
    strSend = send(rdwr_sock,message,strlen(message),0);
    if (strSend == -1) {
      perror("Send");
      exit(EXIT_FAILURE);
    }
  } while (strSend != strlen(message));
}

void closeInPoll (int rdwr_sock, struct pollfd structPoll[]) {
  int i = 1;

  while ((i < MAX_FD) && (structPoll[i].fd != rdwr_sock)) {
    i++;
  }

  memset(&structPoll[i], '\0', sizeof(struct pollfd));

}

void do_read(int rdwr_sock, int sock, char * buffer, struct pollfd structPoll[]) {
  memset(buffer, '\0', strlen(buffer));

  int strReceived, strSizeToReceive;
  do {
    strReceived = recv(rdwr_sock, &strSizeToReceive, sizeof(int), 0);
    if (strReceived == -1) {
      perror("Receive");
      exit(EXIT_FAILURE);
    }
  } while (strReceived != sizeof(int));

  do {
    strReceived = recv(rdwr_sock, buffer, SSIZE_MAX, 0);
    if (strReceived == -1) {
      perror("Receive");
      exit(EXIT_FAILURE);
    }
  } while (strReceived != strSizeToReceive);

    if (!strcmp(buffer,"/quit\n")) {
      do_write(rdwr_sock,"You will be terminated");
      closeInPoll(rdwr_sock, structPoll);
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

int numberPoll(struct pollfd structPoll[]) {
  int i;
  int nbPoll = 0;

  for (i = 1; i < MAX_FD + 1; i++) {
    if (structPoll[i].events == POLLIN) {
      nbPoll++;
    }
  }
  return nbPoll;
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


    struct pollfd structPoll[MAX_FD + 1];
    memset(structPoll, '\0', sizeof(struct pollfd)*(MAX_FD + 1));
    structPoll[0].fd = sock;
    structPoll[0].events = POLLIN;


    int rdwr_sock;
    int i;
    int resPoll;

    char * buffer = malloc(MAX_BUFFER_SIZE*sizeof(char));

    while(1)
    {

      resPoll = poll(structPoll, MAX_FD + 1, -1);

      if (structPoll[0].revents == POLLIN) {
        if (numberPoll(structPoll) >= MAX_FD) {
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
     else {
        for (i = 1; i < MAX_FD + 1; i++) {
          if (structPoll[i].revents == POLLIN) {
            do_read(structPoll[i].fd, sock, buffer, structPoll);
            if (structPoll[i].fd != 0) {
              do_write(structPoll[i].fd, buffer);
            }
     }
    }

        //clean up client socket

    }

    //clean up server socket
}
    return 0;

}
