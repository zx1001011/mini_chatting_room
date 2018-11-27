#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/stat.h>

#define PORT 8888
#define BUFSIZE 1024
#define MSG_FAILURE -1
#define MSG_SUCCESS 0
#define MSG_REGISTER 1
#define MSG_LOGIN 2
#define MSG_MODSCRT 3
#define MSG_NEWSCRTRET 4
#define MSG_CANCEL 5
#define MSG_SEARCH 6
#define MSG_PRI 7
#define MSG_ALL 8 
#define MSG_GROUP_HOST 9 
#define MSG_GROUP_KICK 10
#define MSG_GROUP_BAN 11
#define MSG_GROUP_MEMBER 12
#define MSG_FILE_CONTENT 13
#define MSG_FILE_PATH 14
#define MSG_FILE_DONE 15
#define MSG_FILE_REFUSE 16

typedef struct msg
{
	int type; //消息类型
	int len; //消息的长度
	char name[20];     //用户账号
	char password[20]; //用户密码
	int count;		//在线人数
	char toname[20];
	char fromname[20];
	char question[100]; //忘记密码的问题
	char answer[100]; //问题的答案
	char msg[BUFSIZE];
}MSG;

#endif
