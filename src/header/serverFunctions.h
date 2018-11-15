#ifndef SERVERFUNCTIONS_H_
#define SERVERFUNCTIONS_H_

int do_socket();
struct sockaddr_in init_serv_addr(char ** argv);
void do_bind(int sockServer, struct sockaddr_in sockAddr);
void do_listen(int sockServer);
int do_accept(int sockServer, struct sockaddr_in sockAddr);
void do_send(int rdwrSock, char * buffer, char * who);
void do_receive(int rdwrSock, int sockServer, char * buffer, struct pollfd structPollFd[]);
int spacePollFd(struct pollfd structPollFd[]);
int nbOpenFd(struct pollfd structPollFd[]);
void quit(char * buffer, struct pollfd * structPollFd, int i, struct userInfo * users);
void msgall(struct userInfo * sender, char * message, struct userInfo * users, struct pollfd structPollFd[], int channelIndex);
int msg(struct userInfo * sender, char * username, char * message, struct userInfo * users, struct pollfd structPollFd[], char * buffer);
void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, char * username, struct userInfo * sender, int portP2P);

#endif /* SERVERFUNCTIONS_H_ */
