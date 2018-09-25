#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
void get_addr_info(const char* address, const char* port, struct addrinfo** res){

    int status;
    struct addrinfo hints;

    memset(&hints,0,sizeof(hints));

    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    status = getaddrinfo(address,port,&hints,res);

}
*/

struct sockaddr_in get_addr_info() {
    struct sockaddr_in  sock_server;

    memset(&sock_server, '\0', sizeof(sock_server));
    sock_server.sin_family = AF_INET;
    sock_server.sin_port = htons(33000);
    inet_aton("127.0.0.1", &sock_server.sin_addr);

    return sock_server;
}

int do_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    perror("socket invalide");
    exit(EXIT_FAILURE);
  }
  return sock;
}

void do_connect(int sock, struct sockaddr * sock_host) {
  //printf("%i", resConnect);
  if (connect(sock, sock_host, sizeof(struct sockaddr_in)) == -1) {
    perror("connect invalide");
    exit(EXIT_FAILURE);
  }
}


char* do_readline() {
  printf("Bonjour, donner le message à envoyer\n");
  char* chaine = malloc(100*sizeof(char));
  scanf("%s", chaine);
  return chaine;
}

void do_send(int sock, char * msg, size_t len ,int flags) {

  if (send(sock,msg,len,flags) == -1) {
    perror("send invalide");
    exit(EXIT_FAILURE);
  }
}

void do_read(int sock, void * buf) {
  int reception = recv(sock, buf, 1024, 0);
    if (reception == -1) {
      perror("Read marche pas");
      exit(EXIT_FAILURE);
    }
    if (!strcmp(buf,"/quit")) {
      printf("Au revoir\n");
      close(sock);
      free(buf);
      exit(1);
    }
}

int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        //return 1;
    }

//get address info from the server
/*
struct addrinfo* res = malloc(sizeof(struct addrinfo));
get_addr_info("127.1.0.0", "33000", &res);
struct sockaddr * sock_host = malloc(sizeof(struct sockaddr));
sock_host = res->ai_addr;
*/

struct sockaddr_in sock_host = get_addr_info();
//get the socket
int sock = do_socket();

//connect to remote socket
do_connect(sock, (struct sockaddr * )&sock_host);

char * message = malloc(1000);
char * buf = malloc(1000);
int * len = malloc(sizeof(int));
//get user input
while(1){
  message = do_readline();
  *len = sizeof(message)/sizeof(char);
  //send message to the server
  do_send(sock,message,(size_t)len ,0);
  do_read(sock, buf);
  printf("Le serveur à renvoyé : %s\n",buf);
  //handle_client_message()

}



    return 0;


}
