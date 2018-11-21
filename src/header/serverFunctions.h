#ifndef SERVERFUNCTIONS_H_
#define SERVERFUNCTIONS_H_

int do_socket();
struct sockaddr_in init_serv_addr(char ** argv);
void do_bind(int sockServer, struct sockaddr_in sockAddr);
void do_listen(int sockServer);
int do_accept(int sockServer, struct sockaddr_in sockAddr);
void do_send(int rdwrSock, char * buffer, char * who);
<<<<<<< HEAD
void do_receive(int rdwrSock, int sockServer, char * buffer);
int spacePollFd(struct pollfd structPollFd[]);
int nbOpenFd(struct pollfd structPollFd[]);
void quit(char * buffer, struct pollfd * structPollFd, int i, struct userInfo * users);
void msgall(char * buffer, struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], int channelIndex);
void msg(struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], char * buffer);
void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, struct userInfo * sender);
=======
void do_receive(int rdwrSock, int sockServer, char * buffer, struct pollfd structPollFd[]);
int spacePollFd(struct pollfd structPollFd[]);
int nbOpenFd(struct pollfd structPollFd[]);
void quit(char * buffer, struct pollfd * structPollFd, int i, struct userInfo * users);
void msgall(struct userInfo * sender, char * message, struct userInfo * users, struct pollfd structPollFd[], int channelIndex);
int msg(struct userInfo * sender, char * username, char * message, struct userInfo * users, struct pollfd structPollFd[], char * buffer);
void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, char * username, struct userInfo * sender, int portP2P);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82

#endif /* SERVERFUNCTIONS_H_ */
