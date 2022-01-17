#ifndef OPCLI_H
#define OPCLI_H
#include<stdio.h>                                                                    
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
//协议结构体                                                                          
typedef struct{
	char type;//协议 R 注册 L 登录 S 查找 H 历史记录 Q 退出
	char id; 	//身份
	char usrname[10];//用户名
	char passwd[10];//密码
	char phone[12];
	char addr[20];
	char age[3];
	char salary[10];
	int  row;
	char state;
}__attribute__((packed))Msg;
int do_add(int sfd,Msg buf); 
int do_view(int sfd,Msg buf);
int do_delete(int sfd,Msg buf); 
int do_modify(int sfd,Msg buf);
int do_look(int sfd,Msg buf,char *nameflag);
int do_modify_staff(int sfd,Msg buf,char *nameflag);

int do_register(int sfd,Msg buf);
int do_login(int sfd,Msg bufi,char *nameflag);
int do_search(int sfd,Msg buf);
int do_history(int sfd,Msg buf,char *nameflag);
int do_quit(int sfd,Msg buf,char *nameflag);

#endif
