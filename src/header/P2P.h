#ifndef P2P_H_
#define P2P_H_

#include <poll.h>

void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, struct userInfo * sender);
char * getFilename(char * path);
void sendFile(char * buffer, int sock, pthread_mutex_t * lock, pthread_cond_t * cond, int * canSend);
void recvFile(char * buffer);
void confirmP2P(char * buffer, struct userInfo * recipient, struct userInfo * users, struct pollfd structPollFd[]);
void endP2P(struct userInfo * recipient, struct pollfd structPollFd[]);

#endif /* P2P_H_ */
