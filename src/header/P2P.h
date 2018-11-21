#ifndef P2P_H_
#define P2P_H_

void sendCheck(struct pollfd structPollFd[], char * buffer, struct userInfo * users, char * username, struct userInfo * sender, int portP2P, char * filename)
char * getFilename(char * path);
void sendFile(char * buffer, int sock, pthread_mutex_t * lock, pthread_cond_t * cond, int * canSend);
void recvFile(char * buffer);
void confirmP2P(char * buffer, struct userInfo * recipient, struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], int index);

#endif /* P2P_H_ */
