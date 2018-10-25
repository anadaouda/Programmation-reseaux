#ifndef USERS_H_
#define USERS_H_

struct userInfo;

char * getUsername(struct userInfo * user);
int getYear(struct userInfo * user);
int getMonth(struct userInfo * user);
int getDay(struct userInfo * user);
int getHour(struct userInfo * user);
int getMinute(struct userInfo * user);
int getSecond(struct userInfo * user);
char * getIP(struct userInfo * user);
int getPort(struct userInfo * user);
int getIndex(struct userInfo * user);
int getLoggedIn(struct userInfo * user);
struct tm * getConTime(struct userInfo * user);

void setLoggedIn(struct userInfo * user, int status);
void setConTime(struct userInfo * user, struct tm * conTime);
void setNext(struct userInfo * user, struct userInfo * next);

struct userInfo * getNext(struct userInfo * user);
struct userInfo * createUser();
void addUser(struct userInfo * user, struct userinfo * users);
void newUser(struct userInfo * users, int index, char * IP, int port);
struct userInfo * searchByUsername(struct userInfo * users, char * username);
struct userInfo * searchByIndex(struct userInfo * users, int index);
void deleteUser(int index, struct userInfo * users);

int nbUsers (struct userInfo * users);

#endif /* USERS_H_ */
