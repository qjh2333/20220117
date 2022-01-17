#include "../inc/opcli.h"
int main(int argc, const char *argv[])
{
	//外部传参IP和端口
	if(argc!=3)
	{   
		printf("外部传参错误，请输入IP和端口号\n");
		return -1; 
	}
	//创建套接字
	int sfd =socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		ERR_MSG("socket");
		return -1;
	} 
	//填充服务器IP和端口信息 int类型由主机字节序转化为网络字节序                      
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[2]));    //先从char类型转化为int类型
	sin.sin_addr.s_addr = inet_addr(argv[1]);

	//链接服务器
	if(connect(sfd,(struct sockaddr *)&sin,sizeof(sin))<0)
	{
		ERR_MSG("connect");
		return -1;
	}
	//通过协议选择功能
	Msg buf;
	char choose=0;
	char nameflag[20]="";
	while(1)
	{ 
		printf("--------提示-------\n");                                                                
        printf("-------1注册-------\n");
        printf("-------2登录-------\n");
        printf("-------3退出-------\n");
        printf("please input your choose\n");
		memset(&buf,0,sizeof(buf)); 
		choose=getchar();
		while(getchar()!=10);
		switch(choose)
		{
		case '1':
			buf.type='R';
			do_register(sfd,buf);
			break;
		case '2':
			buf.type='L';
			do_login(sfd,buf,nameflag);
			break;
		case '3':
			buf.type='Q';
			do_quit(sfd,buf,nameflag);
			break;
		default:
			printf("协议有误请重新输入\n");
			continue;
		}
		printf("输入任意字符清屏\n");
		getchar();
        while(getchar()!=10);
        system("clear");
	}
	//关闭套接字
	close(sfd);                         
	return 0;
}
