#include "../inc/opcli.h"
int do_register(int sfd,Msg buf)
{

	ssize_t res =0;
	ssize_t ret =0;
	printf("请输入你要注册的身份: 0: 管理员, 1: 员工\n");
	scanf("%c",&buf.id);
	if(!((buf.id==48)||(buf.id==49)))
	{
		printf("没有此类身份\n");
		return -1;
	}
	printf("请输入用户名:\n");
	scanf("%s",buf.usrname);
	while(getchar()!=10);
	printf("请输入密码:\n");
	scanf("%s",buf.passwd);
	while(getchar()!=10);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	if('N'==buf.type)
	{
		printf("用户名已存在注册失败\n");
	}
	else if('Y'==buf.type)
	{
		printf("注册成功请登录\n");
	}
	return 0;
}
int do_login(int sfd,Msg buf,char* nameflag)
{
	ssize_t res =0;
	ssize_t ret =0;
	printf("请输入用户名:\n");
	scanf("%s",buf.usrname);
	while(getchar()!=10);
	printf("请输入密码:\n");
	scanf("%s",buf.passwd);
	while(getchar()!=10);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	strcpy(nameflag,buf.usrname);
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	if('X'==buf.type)
	{
		printf("用户名不存在登录失败\n");
	}
	else if('Y'==buf.type)
	{
		printf("管理员登录成功\n");
		int flag=1;
		char choose=0;
		while(flag)
		{ 
			printf("---------提示--------\n");
			printf("--------1增加--------\n");
			printf("--------2删除--------\n");
			printf("--------3修改--------\n");
			printf("--------4查找--------\n");
			printf("--------5显示--------\n");
			printf("--------6返回--------\n");
			printf("please input your choose\n");
			memset(&buf,0,sizeof(buf));
			choose=getchar();
			while(getchar()!=10);
			switch(choose)
			{
			case '1':
				buf.type='A';
                do_add(sfd,buf);
                break;
			case '2':
				buf.type='D';
				do_delete(sfd,buf);
				break;
			case '3':
                do_modify(sfd,buf);
                break;
			case '4':
				buf.type='S';
				do_search(sfd,buf);
				break;
			case '5':
				buf.type='V';
				do_view(sfd,buf);
				break;
			case '6':
			//	buf.type='q';
				//跳出循环;
				flag=0;
				break;
			default:
				printf("协议有误请重新输入\n");
				continue;
			}
		}
	}
	else if('y'==buf.type)
	{
		printf("员工登录成功\n");
		int flag=1;
        char choose=0;
        while(flag)
        {
			printf("---------提示--------\n");
			printf("--------1修改个人信息--------\n");
			printf("--------2显示个人信息--------\n");
			printf("--------3返回上一级--------\n");
            printf("please input your choose\n");
            memset(&buf,0,sizeof(buf));
            choose=getchar();
            while(getchar()!=10);
            switch(choose)
            {
            case '1':
                do_modify_staff(sfd,buf,nameflag);
                break;
			case '2':
				buf.type='s';
				do_look(sfd,buf,nameflag);
				break;
            case '3':
                //buf.type='q';
                //跳出循环;
                flag=0;
                break;
            default:
                printf("协议有误请重新输入\n");
                continue;
            }
        }

	}
	else if('Z'==buf.type)
	{
		printf("用户已登录,登录失败\n");
	}
	else if('N'==buf.type)
	{
		printf("密码错误\n");
	}
	return 0;
}

int do_search(int sfd,Msg buf)
{
	
	ssize_t res =0;
	ssize_t ret =0;
	printf("请输入要查询的员工姓名:\n");
	scanf("%s",buf.usrname);
	while(getchar()!=10);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	//分析收到协议进行输出
	int i=0;
	if('Y'==buf.type)
	{
		printf("%s\t%s\t%s\t%s\t%s\n",buf.usrname,buf.phone,buf.addr,buf.age,buf.salary);
	}
	else if('N'==buf.type)
	{
		printf("该员工不存在\n");
	}
	return 0;
}
int do_look(int sfd,Msg buf,char *nameflag)
{
	//发送本人姓名给服务器
	ssize_t res =0;
	ssize_t ret =0;
	strcpy(buf.usrname,nameflag);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	//分析协议接收个人信息
	if('Y'==buf.type)
	{
		printf("%s\t%s\t%s\t%s\t%s\n",buf.usrname,buf.phone,buf.addr,buf.age,buf.salary);
	}
	else if('N'==buf.type)
	{
		printf("信息未填写，请联系管理员添加\n");
	}
	return 0;
}
int do_quit(int sfd,Msg buf,char *nameflag)
{
	//发送本人姓名给服务器
	ssize_t res =0;
	strcpy(buf.usrname,nameflag);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	exit(0);
}
int do_add(int sfd,Msg buf)
{
	ssize_t res = 0;
	ssize_t ret = 0;
	printf("请输入用户名：\n");
	scanf("%s",buf.usrname);
	while(getchar()!=10);
	printf("请输入手机号：\n");
	scanf("%s",buf.phone);
	while(getchar()!=10);
	printf("请输入地址：\n");
	scanf("%s",buf.addr);
	while(getchar()!=10);
	printf("请输入年龄：\n");
	scanf("%s",buf.age);
	while(getchar()!=10);
	printf("请输入薪资：\n");
	scanf("%s",buf.salary);
	while(getchar()!=10);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	if('N'==buf.type)
	{
		printf("该员工已存在，添加失败\n");
	}
	else if('Y'==buf.type)
	{
		printf("添加员工信息成功\n");
	}
	return 0;
}
int do_view(int sfd,Msg buf)
{
	ssize_t res =0;
	ssize_t ret =0;
	int row;
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	int i=0;
	if('Y'==buf.type)
	{
		row = buf.row;
		memset(&buf,0,sizeof(buf));
		for(i=0;i<row;i++)
		{
			ret = recv(sfd,&buf,sizeof(buf),0);
			printf("%s\t%s\t%s\t%s\t%s\n",buf.usrname,buf.phone,buf.addr,buf.age,buf.salary);
			memset(&buf,0,sizeof(buf));
		}
	}
	else if('N'==buf.type)
	{
		printf("员工信息为空\n");
	}
	return 0;
}
int do_delete(int sfd,Msg buf)
{
	ssize_t res =0;
	ssize_t ret =0;
	printf("请输入要删除员工的姓名：\n");
	scanf("%s",buf.usrname);
	while(getchar()!=10);
	res = send(sfd,&buf,sizeof(buf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	memset(&buf,0,sizeof(buf));
	ret = recv(sfd,&buf,sizeof(buf),0);
	if(ret<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	else if(0==ret)
	{
		printf("服务器关闭\n");
		return -1;
	}
	if('Y'==buf.type)
	{
		printf("删除成功\n");
	}
	else if('N'==buf.type)
	{
		printf("要删除的员工信息不存在\n");
	}
	return 0;
}
int do_modify(int sfd,Msg buf)
{
	ssize_t res =0;
	ssize_t ret =0; 
	char name[10];
	char ch =0;
	int flag =1;
	printf("请输入要修改员工的姓名：\n");
	scanf("%s",name);
	while(getchar()!=10);
	while(flag)
	{
		printf("------------提示------------\n");
		printf("--------1修改手机号码-------\n");
		printf("--------2修改家庭地址-------\n");
		printf("--------3修改年龄大小-------\n");
		printf("--------4修改薪资金额-------\n");
		printf("--------5返回上一级---------\n");
		printf("please input your choose\n");
		ch=getchar();
		while(getchar()!=10);
		switch(ch)
		{
		case '1':
			buf.type='1';
			strcpy(buf.usrname,name);
			printf("请输入修改后的手机号：\n"); 
			scanf("%s",buf.phone); 
			while(getchar()!=10);
			res = send(sfd,&buf,sizeof(buf),0);                                                    
			if(res<0)
			{   
				ERR_MSG("send");
				return -1; 
			}   
			memset(&buf,0,sizeof(buf));
			ret = recv(sfd,&buf,sizeof(buf),0);
			if(ret<0)
			{   
				ERR_MSG("recv");
				return -1; 
			}   
			else if(0==ret)
			{   
				printf("服务器关闭\n");
				return -1; 
			}                                                                                      
			if('Y'==buf.type)
			{   
				printf("修改成功\n");
			}   
			else if('N'==buf.type)
			{   
				printf("要修改的员工信息不存在\n");
			}
			break;
		case '2':
			buf.type='2';
			strcpy(buf.usrname,name);
			printf("请输入修改后的家庭地址：\n"); 
			scanf("%s",buf.addr); 
			while(getchar()!=10);
			res = send(sfd,&buf,sizeof(buf),0);                                                    
			if(res<0)
			{   
				ERR_MSG("send");
				return -1; 
			}   
			memset(&buf,0,sizeof(buf));
			ret = recv(sfd,&buf,sizeof(buf),0);
			if(ret<0)
			{   
				ERR_MSG("recv");
				return -1; 
			}   
			else if(0==ret)
			{   
				printf("服务器关闭\n");
				return -1; 
			}                                                                                      
			if('Y'==buf.type)
			{   
				printf("修改成功\n");
			}   
			else if('N'==buf.type)
			{   
				printf("要修改的员工信息不存在\n");
			}
			break;
		case '3':
			buf.type='3';
			strcpy(buf.usrname,name);
			printf("请输入修改后的年龄大小：\n"); 
			scanf("%s",buf.age); 
			while(getchar()!=10);
			res = send(sfd,&buf,sizeof(buf),0);                                                    
			if(res<0)
			{   
				ERR_MSG("send");
				return -1; 
			}   
			memset(&buf,0,sizeof(buf));
			ret = recv(sfd,&buf,sizeof(buf),0);
			if(ret<0)
			{   
				ERR_MSG("recv");
				return -1; 
			}   
			else if(0==ret)
			{   
				printf("服务器关闭\n");
				return -1; 
			}                                                                                      
			if('Y'==buf.type)
			{   
				printf("修改成功\n");
			}   
			else if('N'==buf.type)
			{   
				printf("要修改的员工信息不存在\n");
			}
			break;
		case '4':
			buf.type='4';
			strcpy(buf.usrname,name);
			printf("请输入修改后的薪资金额：\n"); 
			scanf("%s",buf.salary); 
			while(getchar()!=10);
			res = send(sfd,&buf,sizeof(buf),0);                                                    
			if(res<0)
			{   
				ERR_MSG("send");
				return -1; 
			}   
			memset(&buf,0,sizeof(buf));
			ret = recv(sfd,&buf,sizeof(buf),0);
			if(ret<0)
			{   
				ERR_MSG("recv");
				return -1; 
			}   
			else if(0==ret)
			{   
				printf("服务器关闭\n");
				return -1; 
			}                                                                                      
			if('Y'==buf.type)
			{   
				printf("修改成功\n");
			}   
			else if('N'==buf.type)
			{   
				printf("要修改的员工信息不存在\n");
			}
			break;

		case '5':
			//跳出循环;
			flag=0;
			break;
		default:
			printf("协议有误请重新输入\n");
			continue;
		}
	}
	return 0;
}
int do_modify_staff(int sfd,Msg buf,char *nameflag)
{
	ssize_t res =0;
	ssize_t ret =0; 
	char ch =0;
	int flag =1;
	while(flag)
	{
		printf("------------提示------------\n");
		printf("--------1修改手机号码-------\n");
		printf("--------2修改家庭地址-------\n");
		printf("--------3返回上一级---------\n");
		printf("please input your choose\n");
		ch=getchar();
		while(getchar()!=10);
		switch(ch)
		{
		case '1':
			buf.type='1';
			strcpy(buf.usrname,nameflag);
			printf("请输入修改后的手机号：\n"); 
			scanf("%s",buf.phone); 
			while(getchar()!=10);
			res = send(sfd,&buf,sizeof(buf),0);                                                    
			if(res<0)
			{   
				ERR_MSG("send");
				return -1; 
			}   
			ret = recv(sfd,&buf,sizeof(buf),0);
			if(ret<0)
			{   
				ERR_MSG("recv");
				return -1; 
			}   
			else if(0==ret)
			{   
				printf("服务器关闭\n");
				return -1; 
			}                                                                                      
			if('Y'==buf.type)
			{   
				printf("修改成功\n");
			}   
			else if('N'==buf.type)
			{   
				printf("要修改的员工信息不存在\n");
			}
			break;
		case '2':
			buf.type='2';
			strcpy(buf.usrname,nameflag);
			printf("请输入修改后的家庭地址：\n"); 
			scanf("%s",buf.addr); 
			while(getchar()!=10);
			res = send(sfd,&buf,sizeof(buf),0);                                                    
			if(res<0)
			{   
				ERR_MSG("send");
				return -1; 
			}

			ret = recv(sfd,&buf,sizeof(buf),0);
			if(ret<0)
			{   
				ERR_MSG("recv");
				return -1; 
			}   
			else if(0==ret)
			{   
				printf("服务器关闭\n");
				return -1; 
			}                                                                                      
			if('Y'==buf.type)
			{   
				printf("修改成功\n");
			}   
			else if('N'==buf.type)
			{   
				printf("要修改的员工信息不存在\n");
			}
			break;
		
		case '3':
			//跳出循环;
			flag=0;
			break;
		default:
			printf("协议有误请重新输入\n");
			continue;
		}
	}
	return 0;
}
