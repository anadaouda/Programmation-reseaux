#ifndef USERS_H_
#define USERS_H_

// faire des énumartions
struct userInfo;

int getLoggedIn(struct userInfo * user);
char * getUsername(struct userInfo * user);
int getIndex(struct userInfo * user);
struct userInfo * getNext(struct userInfo * user);
int isInChannel(struct userInfo * user);
void setChannel(struct userInfo * user, int channelIndex);
int confirmedP2P(struct userInfo * user);
void setP2P(struct userInfo * user, int status);
int getPortP2P(struct userInfo * user);
char * getIP(struct userInfo * user);
void setPortP2P(struct userInfo * user, int status);

struct userInfo * createUsers();
void newUser(struct userInfo * users, int index, char * IP, int port);
struct userInfo * searchByUsername(struct userInfo * users, char * username);
struct userInfo * searchByIndex(struct userInfo * users, int index);
void deleteUser(int index, struct userInfo * users);
int nbUsers (struct userInfo * users);

void whois (char * buffer, char * username, struct userInfo * users);
void who (char * buffer, struct userInfo * users, int channelIndex);
void nick(char * buffer, struct userInfo * users, char * username, struct userInfo * currentUser);
void loggedIn (char * buffer, struct userInfo * users, int rdwrSock, struct userInfo * currentUser);

#endif /* USERS_H_ */
