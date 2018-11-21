#ifndef CHANNELS_H_
#define CHANNELS_H_

struct channelInfo;

struct channelInfo * createChannel();
char * getChannelName(struct channelInfo * channel);
int getChannelIndex(struct channelInfo * channel);
struct channelInfo * getChannelNext(struct channelInfo * channel);
void setChannelNext(struct channelInfo * channel, struct channelInfo * next);
<<<<<<< HEAD
void newChannel(struct channelInfo * channels, char * buffer);
struct channelInfo * searchByChannelName(struct channelInfo * channel, char * name);
struct channelInfo * searchChannelByIndex(struct channelInfo * channel, int index);
void deleteChannel(int index, struct channelInfo * channel);
void join(struct channelInfo * channels, struct userInfo * user, struct userInfo * users, char * buffer);
void quitChannel(struct channelInfo * channels, struct userInfo * user, char * buffer);
=======
void newChannel(struct channelInfo * channels, char * name, char * buffer);
struct channelInfo * searchByChannelName(struct channelInfo * channel, char * name);
struct channelInfo * searchChannelByIndex(struct channelInfo * channel, int index);
void deleteChannel(int index, struct channelInfo * channel);
void join(struct channelInfo * channels, char * name, struct userInfo * user, struct userInfo * users, char * buffer);
void quitChannel(struct channelInfo * channels, char * name, struct userInfo * user, char * buffer);
>>>>>>> fc939c25de2d3c2a3649dd53301ee61fa9fc4e82
void channelList(char * buffer, struct channelInfo * channels);

#endif /* CHANNELS_H_ */
