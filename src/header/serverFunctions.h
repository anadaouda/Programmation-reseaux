#ifndef SERVERFUNCTIONS_H_
#define SERVERFUNCTIONS_H_

int spacePollFd(struct pollfd structPollFd[]);
int nbOpenFd(struct pollfd structPollFd[]);
void quit(char * buffer, struct pollfd * structPollFd, int i, struct userInfo * users);
void msgall(char * buffer, struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], int channelIndex, char * channelName);
void msg(struct userInfo * sender, struct userInfo * users, struct pollfd structPollFd[], char * buffer);

#endif /* SERVERFUNCTIONS_H_ */
