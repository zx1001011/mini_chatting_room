#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "pub.h"
#include <time.h>
#define SERVERIP "127.0.0.1" //"192.168.100.216"
//记录自己的登陆之后的名字
char myname[20] = {0};
//记录自己的群的身份或者权限
int my_dentification = MSG_GROUP_MEMBER; 
//时间
time_t t;
//消息
MSG msg;
//用来控制文件路径输入时和选择输入时的互斥
pthread_mutex_t mutex_input = PTHREAD_MUTEX_INITIALIZER;

void ask_server(int sockfd); //选择操作菜单
void print_mainmenu(); //主菜单
void print_hostmenu(); //群主聊天菜单
void print_chatmenu(); //聊天菜单
char *input_length_judge(char *p, int size); //判断从键盘输入的数据，使符合要求
char *input_yesno_judge(char *p, int size); //判断从键盘输入的数据，使符合要求
char *input_notmyself_judge(char *p, int size); //判断从键盘输入的数据，使符合要求
int array_locate_subscript(char p[][120], int n, char *a, char *b); //在长度为n的数组p中找到a和b的下标
void print_err(char *p); //输出异常
//服务模块
int reg(int sockfd); //注册
int login(int sockfd); //登录
int chat(int sockfd); //聊天
int modscrt(int sockfd); //修改密码

//聊天模块
int chat_sendata(int socket); //用户只管发送消息
int send_search(int sokcfd); //在线人员查询
int send_private_chat(int sockfd);//私聊
int send_all_chat(int sockfd);//群聊
int send_cancel(int sockfd);//注销
int send_kick_notalk_noban(int sockfd, int type);/*踢人禁言解禁*/
int send_filepath(int sockfd);//文件传输
int send_filecontent(int sockfd, MSG *msg);//文件传输

void *chat_recvdata(void *socket);//用户只管接受消息
int recv_kick_notalk_noban(int sockfd, MSG *msg);//接受禁言踢人和接触禁言的消息
char *recv_filepath(int sockfd, MSG *msg);//接受文件路径
int recv_filecontent(int sockfd, MSG *msg, char *filepath); //接受文件内容
int recv_filedone(int sockfd, MSG *msg); //接受文件接受结束信号
#endif
