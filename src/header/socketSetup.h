#ifndef SOCKET_SETUP_H_
#define SOCKET_SETUP_H_

struct sockaddr_in init_addr(char * IP, int port);
struct sockaddr_in get_addr_info(char* IP, int port);
void do_bind(int sock, struct sockaddr_in sockAddr);
void do_listen(int sock);
int do_accept(int sockS, struct sockaddr_in sockAddr);
int do_socket();
void do_connect(int sock, struct sockaddr * sockAddr);
void do_send(char * buffer, int sock, char * who);
void do_receive(int sock, char * buffer, int type, int rdwrSock);

#endif /* SOCKET_SETUP_H_ */
