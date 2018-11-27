#ifndef _SERVER_H_
#define _SERVER_H_
#include "pub.h"
#include <mysql.h>
#include <signal.h>
#define BACKLOG 50 
typedef struct user
{
	char name[20]; //记录用户名
	int socket;  //记录该用户套接字描述符
	int allchat; //标记群权限
}Userdata;

typedef struct node 
{
	Userdata data;
	struct node *next;
}NODE;

int init_socket();//初始化套接字
int myaccept(int listen_socket);//监听接受客户端
void *handle_client(void *);//每个客户的线程操作
//用户操作功能
int reg(int conntfd,MSG *msg); //用户注册操作
int login(int conntfd,MSG *msg);//用户登录操作
int chat(int conntfd, MSG *msg); //用户选择聊天功能
//用户聊天功能
int search(int conntfd, MSG *msg);//用户查询在线人员操作
int private_chat(int conntfd, MSG *msg);//用户私聊操作
int all_chat(int conntfd, MSG *msg);//用户群聊操作
int cancel(int conntfd, MSG *msg);//用户注销操作
int kick_notalk_noban(int conntfd, MSG *msg);//群主踢人禁言和解禁操作
int putfilepath(int conntfd, MSG *msg);//用户传输文件操作
int putfiledone(int conntfd, MSG *msg);//用户传输文件操作
int putfilecontent(int conntfd, MSG *msg);//用户传输文件操作
//结束用户聊天功能
int modscrt12(int conntfd, MSG *msg);	//用户修改密码操作步骤一二
int modscrt34(int conntfd, MSG *msg);	//用户修改密码操作步骤三四
//结束用户操作功能
//在线人员队列操作
NODE *create_list();//初始化在线人员队列
int traverse_list(char *name);//遍历在线链表查看是否在链表中
int insert_list(Userdata *data);//插入到在线队列中
int delete_list(int conntfd);//注销时从在线队列删除
void print_list();
void free_list(); //释放链表
//数据库操作
MYSQL my_mysql_connect();
#endif
