#include "../inc/opser.h"
int main(int argc, const char *argv[])
{
	//外部传参IP和端口
	if(argc!=3)
	{
		printf("外部传参错误，请输入IP和端口号\n");
		return -1;
	}
	//初始化数据库
	sqlite3 * db = NULL;
	//1，创建打开数据库
	if(sqlite3_open("./dic.db",&db)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("db open success\n");
	//2，创建表格存储注册登录信息
	char sql[128] = "create table if not exists info (id char, usrname char primary key,passwd char,state char)";
	char * errmsg = NULL;
	//调用sqlite3_exec函数
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)                             
	{   
		printf("sqlite3_exec:%s __%d__\n",errmsg,__LINE__);
		return -1; 
	}

	//3,创建表格存储用户详细信息
	//组成sql语句
	bzero(sql,sizeof(sql));
	strcpy(sql,"create table if not exists staff (usrname char primary key,phone char,addr char, age char,salary char)");
	//调用sqlite3_exec函数
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("sqlite3_exec:%s __%d__\n",errmsg,__LINE__);
		return -1;
	}
	//网络初始化
	//创建流式套接字
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		ERR_MSG("socket");
		return -1;
	}
	//允许端口快速重用
	int reuse = 1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}
	//填充服务器IP和端口信息 int类型由主机字节序转化为网络字节序
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[2]));	//先从char类型转化为int类型
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	//绑定服务器IP和端口
	if(bind(sfd,(struct sockaddr *)&sin,sizeof(sin))<0)
	{
		ERR_MSG("bind");
		return -1;
	}
	printf("bind success\n");
	//将套接字设为被动监听
	if(listen(sfd,5)<0)
	{
		ERR_MSG("listen");
		return -1;
	}
	printf("listen success\n");
	//定义父进程接收信息所需变量
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	int newfd = 0;
	//父子进程传参结构体
	pthread_t tid;
	struct CliMsg info;
	while(1)
	{
		//主线程用于处理客户端连接和接收客户端信息
		newfd = accept(sfd,(struct sockaddr *)&cin,&addrlen);
		if(newfd<0)
		{
			ERR_MSG("accept");
			return -1;
		}
		printf("[%s:%d] newfd=%d\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),newfd);
		//将接收到的客户端信息存到结构体中方便传参给子进程
		info.cin = cin;
		info.newfd=newfd;
		info.db=db;
		//链接成功后，创建分支线程，用于和客户端进行交互
		if(pthread_create(&tid,NULL,recv_cli_msg,(void *)&info)<0)
		{
			ERR_MSG("pthread_create");
			return -1;
		}
		printf("tid=%ld",tid);
	}
	close(sfd);
	return 0;
}
void * recv_cli_msg(void *arg)
{
	//线程分离，自己分离自己
	pthread_detach(pthread_self());
	//接收传参信息
	struct CliMsg info = *(struct CliMsg *)arg;
	int newfd = info.newfd;
	sqlite3 *db=info.db;
	//定义接收客户端信息结构体
	Msg cinbuf;
	ssize_t res =0;
	char nameflag[20]="";
	//接收客户端信息并解析协议
	while(1)
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		//接收客户端信息
		res=recv(newfd,&cinbuf,sizeof(cinbuf),0);
		//获取注册或登录人的姓名
		if(strlen(cinbuf.usrname)!=0)
		{
	   		strcpy(nameflag,cinbuf.usrname);
		}
		printf("nameflag=%s",nameflag);
		printf(" 	%ld",res);
		if(res<0)
		{
			ERR_MSG("recv");
			break;
		}else if(0==res)
		{
			//异常退出，状态置为零
			//组成sql语句
			char sql[128] ="";
			sprintf(sql,"update info set state='0' where usrname=\"%s\"",nameflag);
			char * errmsg = NULL;
			//调用sqlite3_exec函数更新登录状态
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
			{
				printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
				return NULL;
			}
			bzero(nameflag,sizeof(nameflag));
			break;
		}
		printf("type=%c usrname=%s,passwd=%s\n",cinbuf.type,cinbuf.usrname,cinbuf.passwd);
		//解析协议走不同代码
		switch(cinbuf.type)
		{
		case 'R':
			do_register(db,cinbuf,newfd);
			break;
		case 'L':
			do_login(db,cinbuf,newfd);
			break;
		case 'A':
			do_add(db,cinbuf,newfd);
			break;
		case 'V':
			do_view(db,cinbuf,newfd);
			break;
		case 'S':
			do_search(db,cinbuf,newfd);
			break;
		case 's':
			do_look(db,cinbuf,newfd);
			break;
		case 'D':
			do_delete(db,cinbuf,newfd);
			break;
		case '1':
			do_modify1(db,cinbuf,newfd);
			break;
		case '2':
			do_modify2(db,cinbuf,newfd);
			break;
		case '3':
			do_modify3(db,cinbuf,newfd);
			break;
		case '4':
			do_modify4(db,cinbuf,newfd);
			break;
		case 'Q':
			do_quit(db,cinbuf,newfd);
			break;
		default:
			printf("协议有误请重新输入\n");  
			break;
		}
	}
	close(newfd);
	pthread_exit(NULL);
}	
