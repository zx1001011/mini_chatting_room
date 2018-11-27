#include "server.h"
NODE *head;
int host_flag = 0, all_count = 0;
//初始化套接字
int init_socket()
{
	int sockfd, ret;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in seraddr;
	int size = sizeof(seraddr);
	bzero(&seraddr, size);
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(PORT);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(sockfd, (struct sockaddr*)&seraddr, size);
	if (ret == -1)
	{
		perror("bind failed!\n");
		exit(-1);
	}
	ret = listen(sockfd, BACKLOG);
	if (ret == -1)
	{
		perror("listen failed!\n");
		exit(-1);
	}

	printf("waiting for client connecting........\n");
	return sockfd;
}
//监听接受客户端
int myaccept(int sockfd)
{
	int conntfd;
	struct sockaddr_in cliaddr;
	int size = sizeof(cliaddr);
	conntfd = accept(sockfd, (struct sockaddr*)&cliaddr, (socklen_t *)&size);
	if (conntfd == -1)
	{
		perror("accept failed!\n");
	}

	printf("connect from client IP addr:%s  sockfd:%d\n", inet_ntoa(cliaddr.sin_addr), conntfd);

	return conntfd;
}
//每个客户端一个线程，线程功能是对接受的用户信息类型判断作出相应操作
void *handle_client(void *socket)
{
	int *clisocket = (int *)socket;
	int conntfd = *clisocket;
	MSG msg;
	int ret, choice = 1;
	while(choice)
	{
		bzero(&msg, sizeof(msg));
		ret = read(conntfd,(void *)&msg, sizeof(msg));
		printf("read from client sockfd %d\n", conntfd);
		if (ret == -1)
		{
			perror("read failed!\n");
		}
		if (ret == 0)
		{
			//用户退出操作
			choice = 0;
		}
		switch(msg.type)
		{
		case MSG_REGISTER:  
			reg(conntfd, &msg); //用户注册操作
			break;
		case MSG_LOGIN:
			login(conntfd, &msg);//用户登录操作
			break;
		case MSG_MODSCRT:
			modscrt12(conntfd, &msg);	//用户修改密码操作步骤一二
			break;
		case MSG_NEWSCRTRET:
			modscrt34(conntfd, &msg);	//接受用户修改的新密码操作步骤三四
			break;
			//用户选择聊天功能
		case MSG_SEARCH:
			search(conntfd, &msg);//用户查询在线人员操作
			break;
		case MSG_PRI:
			private_chat(conntfd, &msg);//用户私聊操作
			break;
		case MSG_ALL:
			all_chat(conntfd, &msg);//用户群聊操作
			break;
		case MSG_CANCEL:
			cancel(conntfd, &msg);//用户注销操作
			break;
		case MSG_GROUP_KICK:
			kick_notalk_noban(conntfd, &msg);//用户踢人操作
			break;
		case MSG_GROUP_BAN:
			kick_notalk_noban(conntfd, &msg);//用户禁言操作
			break;
		case MSG_GROUP_MEMBER:
			kick_notalk_noban(conntfd, &msg);//用户解禁操作
			break;
		case MSG_FILE_PATH:
			putfilepath(conntfd, &msg);//用户传输文件操作
			break;
		case MSG_FILE_DONE:
			putfiledone(conntfd, &msg);//用户传输文件操作
			break;
		case MSG_FILE_CONTENT:
			putfilecontent(conntfd, &msg);//用户传输文件操作
			break;
			//用户选择聊天功能
		}
		printf("all count : %d\n", all_count);
		if (all_count) print_list();
	}
	printf("client %d exit!\n", conntfd);
	delete_list(conntfd);
	close(conntfd);
}
//mysql数据库操作
MYSQL my_mysql_connect() //连接数据库
{
	MYSQL mysql,*pret;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int ret, i;
	pret = mysql_init(&mysql);
	if (pret == NULL)
	{
		printf("mysql_init() : %s\n", mysql_error(&mysql));
	}

	pret = mysql_real_connect(&mysql, "localhost", "root", "123123", "servermsg", 0, NULL, 0);
	if (pret == NULL)
	{
		printf("mysql_real_connect() : %s\n", mysql_error(&mysql));
	}
	printf("Connect mysql test successful!\n");
	return mysql;
}
//主要是对query_str的操作来删除、增加和修改、查询	
//结束mysql数据库操作
//用户操作功能
int reg(int conntfd,MSG *msg) //用户注册操作
{
	MYSQL mysql;
	int ret;
	char buf[1024] = {0};
	printf("client %d register\n", conntfd);
	sprintf(buf,"insert into infor values('%s', '%s', '%s', '%s')", 
			msg->name, msg->password, msg->question, msg->answer);//插入/增加
	mysql = my_mysql_connect();
	ret = mysql_real_query(&mysql, buf, strlen(buf));
	if (ret != 0)
	{
		printf("mysql_real_query() : %s\n", mysql_error(&mysql));
		printf("register failure!\n");
		msg->type = MSG_FAILURE;
	}
	else
	{
		printf("register success!\n");
		msg->type = MSG_SUCCESS;
	}
	write(conntfd, (const void *)msg, sizeof(MSG));
	mysql_close(&mysql);
	return 0;
}
int login(int conntfd,MSG *msg)//用户登录操作
{
	MYSQL mysql;
	MYSQL_RES *res = NULL;
	int ret, flag = 0;
	char buf[1024] = {0};
	Userdata data;
	printf("client %d login \n", conntfd);
	//到数据库中查询是否有这个用户，并且密码正确
	sprintf(buf,"select * from infor where name='%s'and password='%s'", msg->name, msg->password);//查询
	mysql = my_mysql_connect();
	ret = mysql_real_query(&mysql, buf, strlen(buf));
	if (ret != 0)
	{
		printf("mysql_real_query() : %s\n", mysql_error(&mysql));
	}
	res = mysql_store_result(&mysql);
	if (res == NULL)
	{
		printf("mysql_store_result() : %s\n", mysql_error(&mysql));
	}
	flag = mysql_num_rows(res);
	mysql_close(&mysql);
	printf("mysql end!\n");
	if (flag == 0)
	{
		printf("no register, no login!\n");
		msg->type = MSG_FAILURE;
	}
	else
	{
		//遍历在线链表查看是否在链表中
		flag = traverse_list(msg->name);
		printf("traverse list ! flag %d\n", flag);
		if (flag != 0) //online 
		{
			printf("online!\n");
			msg->type = MSG_FAILURE;
		}
		else    //offline
		{
			//插入到在线链表中，群标记为GROUP_MEMBER
			strcpy(data.name, msg->name);
			data.socket = conntfd;
			data.allchat = MSG_GROUP_MEMBER;
			flag = insert_list(&data);
			if (flag)
			{
				printf("login success!\n");
				msg->type = MSG_SUCCESS;
			}
			else
			{
				printf("insert into list failed!\n");
				msg->type = MSG_FAILURE;
			}
		}
	}
	write(conntfd, (const void *)msg, sizeof(MSG));
	return 0;
}//结束登录
//聊天功能
int search(int conntfd, MSG *msg)//用户查询在线人员操作
{
	msg->count = all_count; 
	//将在线人员的姓名放到消息内
	NODE *tmp = head->next;
	int len =  0;
	while (tmp)
	{
		strncpy(msg->msg+len, tmp->data.name, 20);
		len += 20;
		tmp = tmp->next;
	}
	write(conntfd, msg, sizeof(MSG));
	printf("send over\n");
	return 0;
}
int private_chat(int conntfd, MSG *msg)//用户私聊操作
{
	int fri_sockfd = -1;
	fri_sockfd = traverse_list(msg->toname);
	if (fri_sockfd == 0)
	{
		printf("toclient offline!\n");
		msg->count = MSG_FAILURE;	
		write(conntfd, msg, sizeof(MSG));
	}
	else
	{
		msg->count = MSG_SUCCESS;	
		write(fri_sockfd, msg, sizeof(MSG));
	}
	return 0;
}
int all_chat(int conntfd, MSG *msg)//用户群聊操作
{
	NODE *tmp = head->next;
	MSG smsg = *msg;
	while(tmp)
	{
		//还没有人群聊,这位就是群主
		if (!host_flag && tmp->data.socket == conntfd)
		{
			smsg.type = MSG_GROUP_HOST;
			tmp->data.allchat = MSG_GROUP_HOST;
			write(conntfd, &smsg, sizeof(MSG));
			host_flag = 1;
		}
		//fromclient和被踢的人接受不到群消息
		if (!(tmp->data.socket == conntfd || tmp->data.allchat == MSG_GROUP_KICK))
		{
			write(tmp->data.socket, msg, sizeof(MSG));
		}
		tmp = tmp->next;
	}
	return 0;
}
int cancel(int conntfd, MSG *msg)//用户注销操作
{
	printf("offline!\n");
	int ret = delete_list(conntfd);
	if (ret)
	{
		printf("cancel success\n");
	}
	else
	{
		printf("cancel failed!\n");
	}
	write(conntfd, msg, sizeof(MSG));
	return 0;
}
int kick_notalk_noban(int conntfd, MSG *msg)//群主踢人经验和解禁操作
{
	NODE *tmp = head->next;
	while(tmp)
	{
		//群主和被踢的人接受不到群消息
		if (!(tmp->data.socket == MSG_GROUP_HOST || tmp->data.allchat == MSG_GROUP_KICK))
		{
			write(tmp->data.socket, msg, sizeof(MSG));
		}
		//设置禁言和解禁和被踢标志
		if (strncmp(tmp->data.name , msg->msg, 20) == 0)
		{
			tmp->data.allchat = msg->type;
		}
		tmp = tmp->next;
	}
	return 0;
}
//传输文件需要判断文件消息类型
/*文件传输步骤：
 * fromclient->server->toclient:发送文件请求,服务器中转请求
 * 		MSG : type&count->MSG_FILE_PATH,msg->filepath,fromname,toname
 * toclient->server->fromclient:发送文件结果,是否在线,是否接受,服务器中转结果
 * 		MSG : type->MSG_FILE_PATH, count->MSG_FAILURE|REFUSE|SUCCESS,fromname,toname
 * note :不考虑在发送过程中fromclient突然下线问题
 * 如果toclient接受count->MSG_SUCCESS:
 * fromclient->server->toname:发送文件内容,经过服务器转发
 * 		MSG : type&count->MSG_FILE_CONTENT_FIRST|CONTENT|EXCEPTION|DONE, fromname, toname
 * 直到文件内容发送完毕type->MSG_FILE_DONE:
 * toclient->server->fromclient:发送文件接受成功，经过服务器转发
 * 		MSG : type->MSG_FILE_DONE, count->MSG_SUCCESS,fromname,toname
 */
int putfilepath(int conntfd, MSG *msg)//用户传输文件操作
{
	int fri_sockfd;
	if (msg->count == MSG_FILE_PATH)
	{
		//fromclient->toclient
		printf("client %s send file to client %s!\n", msg->fromname, msg->toname);
		fri_sockfd = traverse_list(msg->toname);
		if (fri_sockfd == 0)
		{
			printf("toclient offline!\n");
			msg->count = MSG_FAILURE;
			write(conntfd, msg, sizeof(MSG));
		}
		else
		{
			write(fri_sockfd, msg, sizeof(MSG));
		}
	}
	else// if (msg->count == MSG_FILE_REFUSE||msg->count == MSG_FILE_SUCCESS)
	{
		//toclient->fromclient
		printf("toclient %s send 'accept or refuse' to fromclient %s about file %s\n", msg->toname, msg->fromname, msg->msg);
		fri_sockfd = traverse_list(msg->toname);
		if (fri_sockfd != 0)
		{
			write(fri_sockfd, msg, sizeof(MSG));
		}
	}
	return 0;
}
int putfilecontent(int conntfd, MSG *msg)//用户传输文件操作
{
	int fri_sockfd;
	fri_sockfd = traverse_list(msg->toname);
	if (fri_sockfd != 0)
	{
	//	printf("file content %d->%d\n", conntfd, fri_sockfd);
		write(fri_sockfd, msg, sizeof(MSG));
	}
	return 0;
}
int putfiledone(int conntfd, MSG *msg)//用户传输文件操作
{
	int fri_sockfd;
	fri_sockfd = traverse_list(msg->toname);
	if (fri_sockfd != 0)
	{
		write(fri_sockfd, msg, sizeof(MSG));
		printf("file over!%d->%d\n", conntfd, fri_sockfd);
	}
	return 0;
}
//结束聊天功能
//修改密码操作
/*主要分为四步骤：
 * client->server:接受客户端一个需要修改的信号MSG_MODSCRT
 * server->client:发送客户端信息，是否online或者问题和答案
 * client->server:接受客户端是否修改或者新密码
 * server->client:发送客户端是否修改成功
 */
int modscrt12(int conntfd, MSG *msg)//用户修改密码操作步骤1、2
{
	MYSQL mysql;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int ret, flag = 0;
	char buf[1024] = {0};
	//遍历在线链表查看是否在链表中
	flag = traverse_list(msg->name);
	if (flag != 0) //online
	{
		printf("online!\n");
		msg->type = MSG_FAILURE;
	}
	else //offline
	{
		//到数据库中拿出问题和答案
		sprintf(buf,"select * from infor where name='%s'", msg->name);//查询
		mysql = my_mysql_connect();
		ret = mysql_real_query(&mysql, buf, strlen(buf));
		if (ret != 0)
		{
			printf("mysql_real_query() : %s\n", mysql_error(&mysql));
		}
		res = mysql_store_result(&mysql);
		if (res == NULL)
		{
			printf("mysql_store_result() : %s\n", mysql_error(&mysql));
		}
		int rows = mysql_num_rows(res);
		if (rows == 0)
		{
			msg->type = MSG_FAILURE;
		}
		else
		{
			int fields = mysql_num_fields(res);
			if ((row = mysql_fetch_row(res)))
			{
				strcpy(msg->question, row[2]);
				strcpy(msg->answer, row[3]);
			}
			msg->type = MSG_SUCCESS;
		}
		mysql_close(&mysql);	
	}
	//将Q&A发送给用户
	write(conntfd, msg, sizeof(MSG));
	return 0;
}
int modscrt34(int conntfd, MSG *msg)//用户修改密码操作步骤3、4
{
	MYSQL mysql;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int ret;
	char buf[1024] = {0};
	//接受客户端发送过来的新密码或不能修改
	mysql = my_mysql_connect();
	sprintf(buf, "update infor set password='%s' where name='%s'", 
			msg->password, msg->name);
	ret = mysql_real_query(&mysql, buf, strlen(buf));
	bzero(msg, sizeof(MSG));
	if (ret != 0)
	{
		printf("mysql_real_query() : %s\n", mysql_error(&mysql));
		printf("modify secret failed!\n");
		msg->type = MSG_FAILURE;
	}
	else
	{
		printf("modify secret success!\n");
		msg->type = MSG_SUCCESS;
	}
	//将修改结果发送给服务端
	write(conntfd, msg, sizeof(MSG));
	mysql_close(&mysql);	
	return 0;
}//结束密码修改
//结束用户操作功能
//在线链表操作
//初始化在线人员链表
NODE *create_list()
{
	NODE *p = (NODE *)malloc(sizeof(NODE));
	if (p == NULL)
	{
		perror("malloc failed!\n");
	}
	p->next = NULL;
	return p;
}
int traverse_list(char *name)
{
	NODE *tmp = head->next;
	while(tmp)
	{
		if(strcmp(tmp->data.name, name) == 0)
			return tmp->data.socket; //在线
		tmp = tmp->next;
	}
	return 0; //不在线
}
int insert_list(Userdata *data)
{
	if (head == NULL)
	{
		return -1;
	}
	NODE *tmp = head;
	while(tmp->next)
	{
		tmp = tmp->next;
	}
	NODE *p = (NODE *)malloc(sizeof(NODE));
	if (p == NULL)
	{
		perror("malloc failed!\n");
		return 0;
	}	
	p->data = *data;
	p->next = NULL;
	tmp->next = p;
	all_count++;
	return 1;
}
int delete_list(int conntfd) //从在线链表中删除
{
	if (head == NULL)
		return 0;
	NODE *tmp = head->next, *pre = head;
	int c = 0;
	while (tmp)
	{
		if (tmp->data.socket == conntfd)
		{
			pre->next = tmp->next;
			printf("client %d %s cancel\n", tmp->data.socket, tmp->data.name);
			if (host_flag && tmp->data.allchat == MSG_GROUP_HOST)
			{
				if (tmp->next)
				{
					tmp->next->data.allchat = MSG_GROUP_HOST;
					MSG msg;
					msg.type = MSG_GROUP_HOST;
					write(tmp->next->data.socket, &msg, sizeof(MSG));
				}
				else
				{
					host_flag = 0;
				}
			}
			free(tmp);
			c++;
			break;
		}
		pre = tmp;
		tmp = tmp->next;
	}
	if (c == 0) return 0;
	all_count -= c;
	return 1;
}
void print_list()
{
	NODE *p = head->next;
	while (p) 
	{
		printf("%s %d\n", p->data.name, p->data.socket);
		p = p->next;
	}
	printf("\n");
}
void free_list()
{
	NODE *tmp = head, *p;
	while (tmp)
	{
		p = tmp->next;
		free(tmp);
		tmp = p;
	}
}
//结束队列操作
int main()
{
	//初始化套接字
	int listen_socket = init_socket();
	//初始化在线人员队列
	head = create_list();
	if (head == NULL)
	{
		printf("create list failed!\n");
	}
	//建立在线客户的列表
	while(1)
	{
		//监听接受客户端
		int client_socket = myaccept(listen_socket);
		//每个客户端一个线程，线程功能是对接受的用户信息类型判断作出相应操作
		pthread_t id1;
		pthread_create(&id1, NULL, handle_client, (void *)&client_socket);
		pthread_detach(id1); //线程分离
	}
	//收尾工作
	close(listen_socket);
	free_list();
	return 0;
}
