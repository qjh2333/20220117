#ifndef OPSER_H
#define OPSER_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <errno.h>
//错误提示信息
#define ERR_MSG(msg) do{\
	perror(msg);\
	printf("__%d__\n",__LINE__);\
}while(0)
//需要传入分支进程的结构体
struct CliMsg
{
	struct sockaddr_in cin;
	int newfd;
	sqlite3 *db;
};
//协议结构体
typedef struct{
	char type;//协议 R 注册 L 登录 S 查找 H 历史记录 Q 退出
	char  id; 	//身份
	char usrname[10];//用户名
	char passwd[10];//密码
	char phone[12];
	char addr[20];
	char age[3];
	char salary[10];
	int  row;
	char state;
}__attribute__((packed))Msg;
int do_add(sqlite3 *db,Msg cinbuf,int newfd); 
int do_view(sqlite3 *db,Msg cinbuf,int newfd); 
int do_delete(sqlite3 *db,Msg cinbuf,int newfd);                                                                              
int do_modify1(sqlite3 *db,Msg cinbuf,int newfd); 
int do_modify2(sqlite3 *db,Msg cinbuf,int newfd); 
int do_modify3(sqlite3 *db,Msg cinbuf,int newfd); 
int do_modify4(sqlite3 *db,Msg cinbuf,int newfd); 
int do_look(sqlite3 *db,Msg cinbuf,int newfd);
int do_search_self(sqlite3 *db,Msg cinbuf,int newfd);

void * recv_cli_msg(void *arg);
int do_register(sqlite3 *db,Msg cinbuf,int newfd);
int do_login(sqlite3 *db,Msg cinbuf,int newfd);
int do_search(sqlite3 *db,Msg cinbuf,int newfd);
int do_quit(sqlite3 *db,Msg cinbuf,int newfd);
#endif
