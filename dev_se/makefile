CC = gcc
Imysql = -I/usr/include/mysql -L/usrb/mysql 
Lmysql = -lmysqlclient
Lthread = -lpthread
In = -I../Include -I./
Objectpath = ../Out/
server:server.c
	$(CC) -o $(Objectpath)$@ $(Imysql) $(In) $^ $(Lmysql) $(Lthread)
