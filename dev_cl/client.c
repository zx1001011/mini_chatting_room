#include "client.h"
//服务菜单
void print_mainmenu()
{
	printf("Welcome to use mini chatting room!\n");
	time(&t);
	printf("time : %s", ctime(&t));
	printf("------------------------------------\n");
	printf("---------------主菜单---------------\n");
	printf("---------------1.注册---------------\n");
	printf("---------------2.登录---------------\n");
	printf("---------------3.修改密码-----------\n");
	printf("---------------4.退出---------------\n");
	printf("\n请从1-4选择操作:");
}//print_mainmenu
//群主聊天菜单
void print_hostmenu()
{
	printf("Mini chatting room\n");
	time(&t);
	printf("name : %s\ntime : %s", myname, ctime(&t));
	printf("------------------------------------\n");
	printf("--------------群主聊天菜单----------\n");
	printf("--------------1.在线人数查询--------\n");
	printf("--------------2.私聊----------------\n");
	printf("--------------3.群聊----------------\n");
	printf("--------------4.注销----------------\n");
	printf("--------------5.文件传输------------\n");
	printf("--------------6.踢人----------------\n");
	printf("--------------7.禁言----------------\n");
	printf("--------------8.解禁----------------\n");
	printf("\n请从1-8选择操作:");
}//print_hostmenu
//正常聊天菜单
void print_chatmenu()
{
	printf("Mini chatting room\n");
	time(&t);
	printf("name : %s\ntime : %s", myname, ctime(&t));
	printf("------------------------------------\n");
	printf("--------------聊天菜单--------------\n");
	printf("--------------1.在线人数查询--------\n");
	printf("--------------2.私聊----------------\n");
	printf("--------------3.群聊----------------\n");
	printf("--------------4.注销----------------\n");
	printf("--------------5.文件传输------------\n");
	printf("\n请从1-5选择操作:");
}//print_chatmenu
//判断从键盘输入的数据，使符合长度要求
char *input_length_judge(char *p, int size) //判断从键盘输入的数据，使符合要求
{
	char *buf;
	buf = (char *)malloc(sizeof(char)*(size+5));
	printf("%s:", p);
	fgets(buf, sizeof(char)*(size+5), stdin);
	buf[strlen(buf) - 1] = '\0';
	while (strlen(buf) >= size || strlen(buf)<= 0) 
	{
		bzero(buf, sizeof(buf));
		printf("错误!%s输入不符合要求!\n", p);
		printf("%s:", p);
		fgets(buf, sizeof(char)*(size+5), stdin);
		buf[strlen(buf) - 1] = '\0';
	}
	return buf;
}//input_length_judge
//判断从键盘输入的数据,为y或n
char *input_yesno_judge(char *p, int size) //判断从键盘输入的数据，使符合要求
{
	char *ch, tmp[BUFSIZE];
	sprintf(tmp, "%s(Y/N)", p);
	ch = input_length_judge(tmp, size);
	while(!(ch[0] == 'y' || ch[0] == 'Y' || ch[0] == 'n'|| ch[0] == 'N'))
	{
		printf("输入错误!\n请重新输入!\n");
		ch = input_length_judge(tmp, size);
	}
	return ch;
}//input_yesno_judge
//判断从键盘输入的数据,输入的姓名不能是自己
char *input_notmyself_judge(char *p, int size)
{
	char *fri_name;
	fri_name = input_length_judge(p, size);
	while (strncmp(fri_name, myname, 20) == 0)
	{
		printf("%s不能为自己！请重新输入！\n", p);
		fri_name = input_length_judge(p, size);
	}
	return fri_name;
}
//在长度为n的数组p中找到a和b的下标
int array_locate_subscript(char p[][120], int n, char *a, char *b) //在长度为n的数组p中找到a和b的下标
{
	int i;
	for (i = 0; i<n; ++i)
		if(strncmp(p[i], a, strlen(a)) == 0 && strncmp(p[i]+20, b, strlen(b)) == 0)
			break;
	return i;
}//array_locate_sybscript
//输出错误
void print_err(char *p)
{
	printf("%s\n", p);
	printf("%s\n", strerror(errno));
}//print_err
//主菜单
//注册模块
int reg(int sockfd) //注册
{
	char *p;
	int count = 3;
	bzero(&msg, sizeof(msg));
	//设置消息类型为MSG_REGISTER
	msg.type = MSG_REGISTER;
	//输入用户名、密码和密码修改问题及答案
	printf("\n**************欢迎注册*************\n");
	printf("注意:用户名和密码都要<20字符,一个汉字2个字符,其他为1个字符存储。\n");
	strncpy(msg.name, input_length_judge("用户名", 20), 20);
	strncpy(msg.password, input_length_judge("密码", 20), 20);
	printf("以防忘记密码，请设置问题以及答案，以便修改密码。\n");
	printf("注意:问题和答案都要<100字符,一个汉字2个字符,其他为1个字符存储。\n");
	strncpy(msg.question, input_length_judge("问题", 100), 100);
	strncpy(msg.answer, input_length_judge("答案", 100), 100);
	//传送给服务端
	write(sockfd, (const void *)&msg, sizeof(msg));
	bzero(&msg, sizeof(msg));
	printf("注册中.........\n");
	//接受服务端的回复
	read(sockfd, (void *)&msg, sizeof(msg));
	if (msg.type ==  MSG_FAILURE)
	{
		printf("*******  ！该账号已被注册！  ******\n");
		printf("**************注册失败*************\n");
	}
	else if (msg.type == MSG_SUCCESS)
	{
		printf("**************注册成功*************\n");
	}
	return 0;
}//reg
//登录模块
int login(int sockfd)
{
	char name[40] = {0};
	bzero(&msg, sizeof(msg));
	//设置消息类型为MSG_LOGIN
	msg.type = MSG_LOGIN;
	//输入用户名、密码
	printf("\n**************欢迎登录*************\n");
	strncpy(msg.name, input_length_judge("用户名", 20), 20);
	strncpy(name, msg.name, 20);
	strncpy(msg.password, input_length_judge("密码", 20), 20);
	//传送给服务端
	write(sockfd, (const void *)&msg, sizeof(msg));
	bzero(&msg, sizeof(msg));
	printf("登录中.........\n");
	//接受服务端的回复
	read(sockfd, (void *)&msg, sizeof(msg));
	if (msg.type ==  MSG_FAILURE)
	{
		printf("**************登录失败*************\n");
	}
	else if (msg.type == MSG_SUCCESS)
	{
		strncpy(myname, name, 20);
		printf("**************登录成功*************\n");
		//创建属于该用户的文件夹，以便存放默认路径文件
		sprintf(name, "./%s", myname);
		mkdir(name,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
		//本线程只管发送消息，并且启动接受消息的线程
		chat_senddata(sockfd);
	}
	return 0;
}//login
//聊天模块
//只管接收数据的线程
void *chat_recvdata(void *socket)
{
	int *sock = (int *)socket;
	int sockfd = *sock;
	MSG rmsg;
	int choice = 1, recv_filenum = 0, i, j, ret;
	char *recv_filename[BUFSIZE], recv_name[BUFSIZE][120];
	while(choice)
	{
		bzero(&rmsg, sizeof(rmsg));
		ret = read(sockfd, &rmsg, sizeof(rmsg));
		if (ret == 0)
		{
			break;
		}
		switch(rmsg.type)
		{
		case MSG_CANCEL:
			choice = 0;
			break;
		case MSG_SEARCH:
			printf("\n------------------------------------\n");
			printf("在线人数:%d\n", rmsg.count);
			int len = 0, i = 1;
			printf("序号  用户名\n");
			while (i <= rmsg.count)
			{
				printf("%-4d  %s\n", i, rmsg.msg+len);
				len += 20;
				i += 1;
			}	
			break;
		case MSG_PRI:
			printf("\n------------------------------------\n");
			if (MSG_FAILURE == rmsg.count)
			{
				printf("用户%s不在线!\n", rmsg.toname);
			}
			else
			{
				printf("*私发消息*\n%s : %s\n", rmsg.fromname, rmsg.msg);
			}
			break;
		case MSG_ALL:
			printf("\n------------------------------------\n");
			if (my_dentification != MSG_GROUP_KICK)
			{
				printf("×群消息×\n%s : %s\n", rmsg.fromname, rmsg.msg);
			}
			break;
		case MSG_GROUP_HOST:
			printf("\n------------------------------------\n");
			printf("$您现在是群主啦!$\n");
			my_dentification = MSG_GROUP_HOST;
			break;
		case MSG_GROUP_KICK:
			printf("\n------------------------------------\n");
			recv_kick_notalk_noban(sockfd, &rmsg);//接受禁言踢人和接触禁言的消息
			break;
		case MSG_GROUP_BAN:
			printf("\n------------------------------------\n");
			recv_kick_notalk_noban(sockfd, &rmsg);//接受禁言踢人和接触禁言的消息
			break;
		case MSG_GROUP_MEMBER:
			printf("\n------------------------------------\n");
			recv_kick_notalk_noban(sockfd, &rmsg);//接受禁言踢人和接触禁言的消息
			break;
		case MSG_FILE_PATH:
			printf("\n------------------------------------\n");
			recv_filename[recv_filenum] = recv_filepath(sockfd, &rmsg); //接收文件名并确认文件是否接受，如果接受，返回文件名，否则，返回空指针
			if (recv_filename[recv_filenum]) 
			{
				strncpy(recv_name[recv_filenum], rmsg.toname, 20);
				strncpy(recv_name[recv_filenum]+20, rmsg.msg, 100);
				recv_filenum += 1;
			}
			break;
		case MSG_FILE_CONTENT:
			i = array_locate_subscript(recv_name, recv_filenum, rmsg.fromname, msg.question); //从存储的fromname中找到发送的用户名的下标，以便查找存储定义的文件名
			recv_filecontent(sockfd, &rmsg, recv_filename[i]); //文件内容保存	
			break;
		case MSG_FILE_DONE:
			printf("\n------------------------------------\n");
			i = array_locate_subscript(recv_name, recv_filenum, rmsg.fromname, msg.question); //从存储的fromname中找到发送的用户名的下标，以便查找存储定义的文件名
			//文件读完了
			if (recv_filedone(sockfd, &rmsg))
			{
				//接收完毕后，将文件名和fromname删除
				for (j = i; j<recv_filenum-1; ++j)
				{
					recv_filename[j] = recv_filename[j+1];
					strncpy(recv_name[j], recv_name[j+1], 120);
				}
				recv_filenum -= 1;
			}
			break;
		}
	}
}//chat_recvdata
//只管发送数据的线程
int chat_senddata(int sockfd)
{
	int choice = 1,ret;
	pthread_t id;
	//创建一个线程只管接受消息
	pthread_create(&id, NULL, chat_recvdata,(void *)&sockfd);
	system("clear");
	while(choice) //用户进入功能只管发送消息
	{
		usleep(100);
		pthread_mutex_lock(&mutex_input);
		printf("\n------------------------------------\n");
		if (my_dentification == MSG_GROUP_HOST) print_hostmenu();
		else print_chatmenu();
		scanf("%d", &choice);
		getchar();
		system("clear");
		printf("\n------------------------------------\n");
		pthread_mutex_unlock(&mutex_input);
		switch(choice)
		{
		case 1:
			send_search(sockfd);
			break;
		case 2:
			send_private_chat(sockfd);
			break;
		case 3:
			send_all_chat(sockfd);
			break;
		case 4:
			send_cancel(sockfd);
			choice = 0;
			break;
		case 5:
			send_filepath(sockfd);
			break;
		case 6:
			if (my_dentification == MSG_GROUP_HOST)	send_kick_notalk_noban(sockfd, MSG_GROUP_KICK);
			else printf("无用输入!请重新选择!\n");
			break;
		case 7:
			if (my_dentification == MSG_GROUP_HOST)	send_kick_notalk_noban(sockfd, MSG_GROUP_BAN);
			else printf("无用输入!请重新选择!\n");
			break;
		case 8:
			if (my_dentification == MSG_GROUP_HOST)	send_kick_notalk_noban(sockfd, MSG_GROUP_MEMBER);
			else printf("无用输入!请重新选择!\n");
			break;
		default:
			printf("无用输入!请重新选择!\n");
			break;
		}
	}
	pthread_cancel(id);
	return 0;
}//chat_senddata
//具体发送消息的功能,聊天消息发送
//发送在线人数查找的功能
int send_search(int sockfd)
{
	bzero(&msg, sizeof(MSG));
	msg.type = MSG_SEARCH;
	write(sockfd, &msg, sizeof(msg));
	return 0;
}//send_search
//发送私聊的功能
int send_private_chat(int sockfd)
{
	if (my_dentification == MSG_GROUP_BAN)
	{
		printf("您已被群主禁言！\n");
	}
	else
	{
		bzero(&msg, sizeof(MSG));
		msg.type = MSG_PRI;
		strncpy(msg.fromname, myname, 20);
		strncpy(msg.toname, input_notmyself_judge("接收人", 20),20);
		strncpy(msg.msg, input_length_judge("消息", BUFSIZE), BUFSIZE);
		write(sockfd, &msg, sizeof(msg));
	}
	return 0;
}//send_private_chat
//发送群聊的消息
int send_all_chat(int sockfd)
{
	if (my_dentification == MSG_GROUP_BAN)
	{
		printf("您已被群主禁言！\n");
	}
	else if (my_dentification == MSG_GROUP_KICK)
	{
		printf("您已被群主踢出群聊！\n");
	}
	else
	{
		bzero(&msg, sizeof(MSG));
		msg.type = MSG_ALL;
		strncpy(msg.fromname, myname, 20);
		strncpy(msg.msg, input_length_judge("消息", BUFSIZE), BUFSIZE);
		write(sockfd, &msg, sizeof(msg));
	}
	return 0;
}//send_all_chat
//发送注销的消息
int send_cancel(int sockfd)
{
	bzero(&msg, sizeof(MSG));
	msg.type = MSG_CANCEL;
	write(sockfd, &msg, sizeof(msg));
	return 0;
}//send_cancel
//发送踢人禁言和解禁的消息
int send_kick_notalk_noban(int sockfd, int type)
{
	bzero(&msg, sizeof(MSG));
	msg.type = type;
	//是否为群主
	if (my_dentification == MSG_GROUP_HOST)
	{
		strncpy(msg.fromname, myname, 20);
		strncpy(msg.msg, input_notmyself_judge("用户名", 20),20);
		write(sockfd, &msg, sizeof(msg));
	}
	return 0;
}//send_kick_notalk_noban
//发送文件给服务端转发,as fromclient
/*文件传输步骤：
 * fromclient->server->toclient:发送文件请求,服务器中转请求
 * 		MSG : type&count->MSG_FILE_PATH,msg->filepath,fromname,toname
 * toclient->server->fromclient:发送文件结果，是否接受,服务器中转结果
 * 		MSG : type->MSG_FILE_PATH, count->MSG_FAILURE|SUCCESS,fromname,toname
 * 如果toclient接受count->MSG_SUCCESS:
 * fromclient->server->toclient:发送文件内容,经过服务器转发
 * 		MSG : type&count->MSG_FILE_CONTENT|DONE, fromname, toname
 * 直到文件内容发送完毕type->MSG_FILE_DONE:
 * toclient->server->fromclient:发送文件接受成功，经过服务器转发
 * 		MSG : type->MSG_FILE_DONE, count->SUCCESS,fromname,toname
 */
//发送文件名给服务端
int send_filepath(int sockfd)
{
	MSG msg;
	char *filepath, *fri_name;
	int ret;
	//发送文件名
	msg.type = MSG_FILE_PATH;
	msg.count = MSG_FILE_PATH;
	strncpy(msg.fromname, myname, 20);
	fri_name = input_notmyself_judge("接收人", 20);
	strncpy(msg.toname, fri_name, 20);
	filepath = input_length_judge("文件路径", 100);
	strncpy(msg.msg, filepath, 100);
	ret = write(sockfd, &msg, sizeof(msg));
	if (ret == -1) print_err("write filepath failed!");
	return 0;
}//send_filepath
//发送文件内容给服务端
int send_filecontent(int sockfd, MSG *rm)
{
	MSG msg;
	FILE *fp;
	int ret, done = 0;
	pthread_t id;
	//创建一个线程只管接受消息
	pthread_create(&id, NULL, chat_recvdata,(void *)&sockfd);
	//发送文件内容或者文件结束或者异常
	fp = fopen(rm->msg,"r");
	if (fp == NULL) print_err("fopen failed!");
	while (!done)
	{
		usleep(15); //must wait server operation
		bzero(&msg, sizeof(msg));
		strncpy(msg.question, rm->msg, 100);
		strncpy(msg.fromname, myname, 20);
		strncpy(msg.toname, rm->fromname, 20);
		if (feof(fp))
		{
			msg.type = MSG_FILE_DONE;
			msg.count = MSG_FILE_DONE;
			done = 1;
		}
		else
		{
			msg.len = fread(msg.msg, sizeof(char), BUFSIZE, fp);
			if (ferror(fp) != 0)
			{
				perror("read failed!\n");
				clearerr(fp);
			}
			else
			{
				msg.type = MSG_FILE_CONTENT;
				msg.count = MSG_FILE_CONTENT;
			}
		}
		ret = write(sockfd, &msg, sizeof(MSG));
		if (ret == -1) print_err("send filecontent failed!");
	}
	fclose(fp);
	pthread_cancel(id);
	return 1;
}//send_filecontent
//结束文件发送操作
//结束聊天发送
//具体聊天接受的消息功能,聊天消息接受操作
//接受禁言踢人和接触禁言的消息
int recv_kick_notalk_noban(int sockfd, MSG *msg)//接受禁言踢人和接触禁言的消息
{
	if (strncmp(myname, msg->msg, strlen(myname)) == 0)
	{
		if (msg->type == MSG_GROUP_KICK) printf("你被群主踢出群聊!\n");
		else if (msg->type == MSG_GROUP_BAN) printf("你被群主禁言!\n");
		else if (msg->type == MSG_GROUP_MEMBER) printf("你被群主解禁!\n");
		my_dentification = msg->type;
	}
	else
	{
		if (msg->type == MSG_GROUP_KICK) printf("%s被群主踢出群聊!\n", msg->msg);
		else if (msg->type == MSG_GROUP_BAN) printf("%s被群主禁言!\n", msg->msg);
		else if (msg->type == MSG_GROUP_MEMBER) printf("%s被群主解禁!\n", msg->msg);
	}
	return 0;
}//recv_kick_notalk_noban
//接收服务器转发的文件操作,as toclient
//接收文件内容
int recv_filecontent(int sockfd, MSG *msg, char *filepath)
{
	if (msg->type == MSG_FILE_CONTENT)
	{
		//as toclient
		FILE *fp;
		fp = fopen(filepath,"a+");
		fwrite(msg->msg, sizeof(char), msg->len, fp);
		fclose(fp);
	}
	return 0;
}//recv_filecontent
//接收文件结束
int recv_filedone(int sockfd, MSG *msg)
{
	int flag = 0;
	if (msg->count == MSG_FILE_DONE)
	{
		//as toclient
		msg->count = MSG_SUCCESS;
		printf("%s发送的文件%s接收完毕\n", msg->fromname, msg->question);
		strncpy(msg->toname, msg->fromname,20);
		strncpy(msg->fromname, myname, 20);
		write(sockfd, msg, sizeof(MSG));
		flag = 1;
	}
	else if (msg->count == MSG_SUCCESS)
	{
		//as fromcliet
		time(&t);
		printf("time : %s", ctime(&t));
		printf("发送给用户%s的文件%s接收完毕!\n", msg->fromname, msg->question);
	}
	return flag;
}//recv_filedone
//接收文件名
char *recv_filepath(int sockfd, MSG *msg)
{
	char *filepath = NULL;
	if (msg->count == MSG_FAILURE) 
	{
		//as fromclient
		printf("用户%s不在线！无法传送！\n", msg->fromname);
	}
	else if (msg->count == MSG_FILE_REFUSE)
	{
		//as fromclient
		printf("用户%s拒绝接受文件%s！\n", msg->fromname, msg->msg);
	} 
	else if (msg->count == MSG_SUCCESS)
	{
		//as fromclient
		time(&t);
		printf("time : %s", ctime(&t));
		printf("用户%s接受文件%s中.........\n", msg->fromname, msg->msg);
		//发送文件内容给服务器转发
		send_filecontent(sockfd, msg);
	}
	else 
	{
		//as toclient
		char *ch;
		printf("%s 向你发送文件 %s!\n@@请选择1之后会弹出是否接受选择！\n", msg->fromname, msg->msg);
		pthread_mutex_lock(&mutex_input);
		ch = input_yesno_judge("是否接收", 2);
		if (ch[0] == 'y' || ch[0] == 'Y')
		{
			msg->count = MSG_SUCCESS;
			printf("\n");
			ch = input_yesno_judge("文件是否另存", 2);
			if (ch[0] == 'y'|| ch[0] == 'Y')
			{
				filepath = input_length_judge("文件路径", 20);
			}
			else
			{
				printf("文件默认保存在当前路径的文件夹%s下！\n", myname);
				//保存在默认文件夹中
				filepath = (char *)malloc(sizeof(char)*100);
				char *tmp;
				tmp = strrchr(msg->msg, '/');
				if (tmp == NULL)
				{
					tmp = msg->msg;
				}
				else
				{
					tmp += 1;
				}
				sprintf(filepath ,"./%s/%s", myname, tmp);
			}
		}
		else if(ch[0] == 'n'|| ch[0] == 'N')
		{
			msg->count = MSG_FILE_REFUSE;
		}
		pthread_mutex_unlock(&mutex_input);
		strncpy(msg->toname, msg->fromname,20);
		strncpy(msg->fromname, myname, 20);
		write(sockfd, msg, sizeof(MSG));
	}
	return filepath;
}//recv_filepath
//结束聊天消息接受操作
//结束聊天模块
//修改密码模块
int modscrt(int sockfd)  //修改密码
{
	/*主要分为四步骤：
	 * client->server:发送服务端一个需要修改的信号MSG_MODSCRT
	 * server->client:接受服务端的信息，是否online或者问题和答案
	 * client->server:发送服务端是否修改或者新密码
	 * server->client:接受服务端是否修改成功
	 */
	MSG msg;
	char *name, buf[100] = {0};
	bzero(&msg, sizeof(msg));
	printf("\n**************修改密码**************\n");
	//设置消息类型为MSG_MODSCRT
	msg.type = MSG_MODSCRT;
	name = input_length_judge("用户名", 20);
	strncpy(msg.name, name, 20);
	printf("连接服务器中.........\n");
	//步骤一：发送给客户端一个修改密码的请求
	write(sockfd, (const void *)&msg, sizeof(msg));
	bzero(&msg, sizeof(msg));
	//步骤二：接受服务端发送给我的问题及答案 或者不能修改当用户online
	read(sockfd, (void *)&msg, sizeof(msg));
	if (msg.type == MSG_FAILURE)
	{
		printf("**************修改失败**************\n");
	}
	else
	{
		int count = 3;
		//回答问题
		while(count)
		{
			printf("%s:", msg.question);
			fgets(buf, sizeof(buf), stdin);
			buf[strlen(buf) - 1] = '\0';
			if (strncmp(buf, msg.answer, sizeof(msg.answer)) == 0)
				break;
			if (count >= 1) printf("回答错误!\n");
			count--;
		}
		if (count != 0)
		{
			//输入新密码
			bzero(&msg, sizeof(msg));
			strncpy(msg.password, input_length_judge("新密码", 20), 20);
			strncpy(msg.name, name, 20);
			msg.type = MSG_NEWSCRTRET;
			//步骤三：传送给服务端
			write(sockfd, (const void *)&msg, sizeof(msg));

			printf("修改中.........\n");
			//步骤四：接受服务端的回复
			bzero(&msg, sizeof(msg));
			read(sockfd, (void *)&msg, sizeof(msg));
			if (msg.type ==  MSG_FAILURE)
			{
				printf("**************修改失败**************\n");
			}
			else if (msg.type == MSG_SUCCESS)
			{
				printf("**************修改成功**************\n");
			}
		}
		else if (count == 0)
		{
			printf("**************修改失败**************\n");
		}
	}
	return 0;
}//modscrt
//结束服务模块

//选择服务
void ask_server(int sockfd)
{
	int choice = 1;
	system("clear");
	while(choice)
	{
		printf("\n------------------------------------\n");
		print_mainmenu();
		scanf("%d", &choice);
		getchar();
		system("clear");
		switch(choice)
		{
		case 1:
			reg(sockfd);
			break;
		case 2:
			login(sockfd);
			break;
		case 3:
			modscrt(sockfd);
			break;
		case 4:
			printf("BYE! Welcome to use it next!\n");
			choice = 0;
			break;
		default:
			printf("无用输入!请重新选择!\n");
			break;
		}
	}
}
int main()
{
	//连接服务端
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket failed!\n");
		exit(-1);
	}

	struct sockaddr_in seraddr;
	bzero(&seraddr, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(PORT);
	seraddr.sin_addr.s_addr = inet_addr(SERVERIP);

	int ret = connect(sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
	if (ret == -1)
	{
		perror("connect failed!\n");
		return -1;
	}
	printf("connect success!\n");

	//用户选择菜单
	ask_server(sockfd);

	//收尾工作
	close(sockfd);
	return 0;
}
