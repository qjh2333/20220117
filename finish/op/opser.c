#include "../inc/opser.h"
int do_register(sqlite3 *db,Msg cinbuf,int newfd)
{
	char sql[128]="";
	char *errmsg=NULL;
	cinbuf.state='0';
	//获取usrname,passwd
	//组成sql语句
	sprintf(sql,"insert into info values(\"%c\",\"%s\",\"%s\",\"%c\")",cinbuf.id,cinbuf.usrname,cinbuf.passwd,cinbuf.state);
	//调用sqlite3_exec函数
	ssize_t res=0;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)                             
	{
		if(2==errno)
		{
			memset(&cinbuf,0,sizeof(cinbuf));
			cinbuf.type='N';
			res=send(newfd,&cinbuf,sizeof(cinbuf),0);
			if(res<0)
			{
				ERR_MSG("send");
				return -1;
			}
			return -1;
		}
		printf("sqlite3_exec:%s __%d__\n",errmsg,__LINE__);
		return -1;
	}

	memset(&cinbuf,0,sizeof(cinbuf));
	cinbuf.type='Y';
	res=send(newfd,&cinbuf,sizeof(cinbuf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}

int do_login(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取usrname
	//组成sql语句
	char name[10]="";
	ssize_t res =0;
	char **presult;                                                                   
	int row=0,column=0;
	char sql[128]="";
	strcpy(name,cinbuf.usrname);
	sprintf(sql,"select *from info where usrname=\"%s\"",cinbuf.usrname);
	char *errmsg=NULL;
	//调用sqlite_get_table函数，sql只能使用查询语句;
	if(sqlite3_get_table(db,sql,&presult,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("sqlite3_exec:%s __%d__\n",errmsg,__LINE__);
		return -1;
	}
	//查找失败
	if(0==row)
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='X';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	else
	{
		//查询成功比较密码信息
		printf("flag=%c\n",presult[7][0]);
		if(strcmp(presult[6],cinbuf.passwd)==0&&presult[7][0]=='0')
		{
			printf("%c\n",presult[4][0]);
			if('0'==presult[4][0])
			{
				memset(&cinbuf,0,sizeof(cinbuf));
				cinbuf.type='Y';
				send(newfd,&cinbuf,sizeof(cinbuf),0);
				printf("%s管理员登录成功\n",cinbuf.usrname);
				//更新状态信息
				//组成sql语句
				bzero(sql,sizeof(sql));
				sprintf(sql,"update info set state='1' where usrname=\"%s\"",name);
				char * errmsg = NULL;
				//调用sqlite3_exec函数更新登录状态
				if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
				{
					printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
					return -1;
				}
				return 0;
			}
			else
			{
				memset(&cinbuf,0,sizeof(cinbuf));
				cinbuf.type='y';                                                                                                              
                send(newfd,&cinbuf,sizeof(cinbuf),0);
                printf("%s员工登录成功\n",cinbuf.usrname);
                //更新状态信息
                //组成sql语句
                bzero(sql,sizeof(sql));
                sprintf(sql,"update info set state='1' where usrname=\"%s\"",name);
                char * errmsg = NULL;
                //调用sqlite3_exec函数更新登录状态
                if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
                {   
                    printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
                    return -1; 
                }   
				return 0;
			}
		}
		else if(strcmp(presult[6],cinbuf.passwd)==0)
		{
			memset(&cinbuf,0,sizeof(cinbuf));
			cinbuf.type='Z';
			send(newfd,&cinbuf,sizeof(cinbuf),0);
			return -1;
		}else
		{
			memset(&cinbuf,0,sizeof(cinbuf));
			cinbuf.type='N';
			send(newfd,&cinbuf,sizeof(cinbuf),0);
			return -1;
		}
	}
	return 0;
}
int do_search_self(sqlite3 *db,Msg cinbuf,int newfd)
{
   	//获取name
    //组成sql语句
    ssize_t res =0;
    char **presult;
    int row=0,column=0;
    char sql[256]="";
    sprintf(sql,"select *from staff where usrname=\"%s\"",cinbuf.usrname);
    char *errmsg=NULL;
    //调用sqlite_get_table函数，sql只能使用查询语句;
    if(sqlite3_get_table(db,sql,&presult,&row,&column,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_get_table:%s __%d__\n",errmsg,__LINE__);
        return -1;
    }
    //查找失败
    if(0==row)
        return -1;
    //查找成功
    else
        return 1;
    return 0;
}

int do_search(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取name
	//组成sql语句
	ssize_t res =0;
	char **presult;                                                                   
	int row=0,column=0;
	char sql[256]="";
	sprintf(sql,"select *from staff where usrname=\"%s\"",cinbuf.usrname);
	char *errmsg=NULL;
	//调用sqlite_get_table函数，sql只能使用查询语句;
	if(sqlite3_get_table(db,sql,&presult,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("sqlite3_get_table:%s __%d__\n",errmsg,__LINE__);
		return -1;
	}
	//查找失败
	if(0==row)
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	//查找成功
	else 
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		strcpy(cinbuf.usrname,presult[5]);
		strcpy(cinbuf.phone,presult[6]);
		strcpy(cinbuf.addr,presult[7]);
		strcpy(cinbuf.age,presult[8]);
		strcpy(cinbuf.salary,presult[9]);
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	return 0;
}

int do_look(sqlite3 *db,Msg cinbuf,int newfd)
{
	ssize_t res =0;
	//获取登录人的姓名
	//组成sql语句
	char **presult;                                                                   
	int row=0,column=0;
	char sql[128]="";
	sprintf(sql,"select *from staff where usrname=\"%s\"",cinbuf.usrname);
	char *errmsg=NULL;
	//调用sqlite_get_table函数，sql只能使用查询语句;
	if(sqlite3_get_table(db,sql,&presult,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("sqlite3_exec:%s __%d__\n",errmsg,__LINE__);
		return -1;
	}
	//查找失败
	if(0==row)
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	//查找成功
	else 
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		strcpy(cinbuf.usrname,presult[5]);
		strcpy(cinbuf.phone,presult[6]);
		strcpy(cinbuf.addr,presult[7]);
		strcpy(cinbuf.age,presult[8]);
		strcpy(cinbuf.salary,presult[9]);
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	return 0;
}
int do_quit(sqlite3 *db,Msg cinbuf,int newfd)
{
	//更新状态信息
	//组成sql语句
	char sql[128]="";
	sprintf(sql,"update info set state='0' where usrname=\"%s\"",cinbuf.usrname);
	char * errmsg = NULL;
	//调用sqlite3_exec函数更新登录状态
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
		return -1;
	}
	return 0;
}
int do_add(sqlite3 *db, Msg cinbuf,int newfd)
{
	char sql[128]="";
	char *errmsg=NULL;
	//获取员工信息
	//组成sql语句
	sprintf(sql,"insert into staff values(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",cinbuf.usrname,cinbuf.phone,cinbuf.addr,cinbuf.age,cinbuf.salary);
	//调用sqlite3_exec函数
	ssize_t res =0;
	//先查找看此员工是否存在
	if(1==do_search_self(db,cinbuf,newfd))
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type = 'N';
		res = send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		if(2==errno)
		{
			memset(&cinbuf,0,sizeof(cinbuf));
			cinbuf.type = 'N';
			res = send(newfd,&cinbuf,sizeof(cinbuf),0);
			if(res<0)
			{
				ERR_MSG("send");
				return -1;
			}
			return -1;
		}
		printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
		return -1;
	}
	memset(&cinbuf,0,sizeof(cinbuf));
	cinbuf.type ='Y';
	res=send(newfd,&cinbuf,sizeof(cinbuf),0);
	if(res<0)
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
int do_view(sqlite3 *db,Msg cinbuf,int newfd)
{
	//组成sql语句
	ssize_t res = 0;
	char **presult;
	int row=0,column=0;
	char sql[128]="select *from staff";
	char *errmsg=NULL;
	//调用sqlite_get_table函数,sql只能使用查询语句;
	if(sqlite3_get_table(db,sql,&presult,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("sqlite3_get_table:%s__%d__\n",errmsg,__LINE__);
		return -1;
	}
	//查找失败
	if(0==row)
	{
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	//查找成功
	else
	{
		int i=0;
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		cinbuf.row=row;
		res =send(newfd,&cinbuf,sizeof(cinbuf),0);
		for(i=0;i<row;i++)
		{
			strcpy(cinbuf.usrname,presult[5*(i+1)]);
			strcpy(cinbuf.phone,presult[5*(i+1)+1]);
			strcpy(cinbuf.addr,presult[5*(i+1)+2]);
			strcpy(cinbuf.age,presult[5*(i+1)+3]);
			strcpy(cinbuf.salary,presult[5*(i+1)+4]);
			res =send(newfd,&cinbuf,sizeof(cinbuf),0);
			if(res<0)
			{
				ERR_MSG("send");
				return -1;
			}
		}
		return 1;
	}
	return 0;
}
int do_delete(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取要删除姓名
	//组成sql语句
	ssize_t res =0;
	char sql[128]="";
	sprintf(sql,"delete from staff where usrname=\"%s\"",cinbuf.usrname);
	char *errmsg=NULL;
	//先查找看此员工是否存在
	if(1==do_search_self(db,cinbuf,newfd))
	{
		//员工存在，进行删除
		//调用sqlite3_exec函数删除行
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
			printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
			return -1;
		}
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	else
	{
		//员工不存在无法删除
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type ='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	return 0;
}
int do_modify1(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取要修改的员工的姓名
	//组成sql语句
	ssize_t res =0;
	char sql[128]="";
	sprintf(sql,"update staff set phone=\"%s\" where usrname=\"%s\"",cinbuf.phone,cinbuf.usrname);
	char *errmsg=NULL;
	//先查找看此员工是否存在
	if(1==do_search_self(db,cinbuf,newfd))
	{
		//员工存在，进行修改
		//调用sqlite3_exec函数进行修改手机号
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
			printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
			return -1;
		}
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	else
	{
		//员工不存在无法修改
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type ='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	return 0;
}

int do_modify2(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取要修改的员工的姓名
	//组成sql语句
	ssize_t res =0;
	char sql[128]="";
	sprintf(sql,"update staff set addr=\"%s\" where usrname=\"%s\"",cinbuf.addr,cinbuf.usrname);
	char *errmsg=NULL;
	//先查找看此员工是否存在
	if(1==do_search_self(db,cinbuf,newfd))
	{
		//员工存在，进行修改
		//调用sqlite3_exec函数进行修改手机号
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
			printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
			return -1;
		}
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	else
	{
		//员工不存在无法修改
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type ='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	return 0;
}

int do_modify3(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取要修改的员工的姓名
	//组成sql语句
	ssize_t res =0;
	char sql[128]="";
	sprintf(sql,"update staff set age=\"%s\" where usrname=\"%s\"",cinbuf.age,cinbuf.usrname);
	char *errmsg=NULL;
	//先查找看此员工是否存在
	if(1==do_search_self(db,cinbuf,newfd))
	{
		//员工存在，进行修改
		//调用sqlite3_exec函数进行修改手机号
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
			printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
			return -1;
		}
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	else
	{
		//员工不存在无法修改
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type ='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	return 0;
}

int do_modify4(sqlite3 *db,Msg cinbuf,int newfd)
{
	//获取要修改的员工的姓名
	//组成sql语句
	ssize_t res =0;
	char sql[128]="";
	sprintf(sql,"update staff set salary=\"%s\" where usrname=\"%s\"",cinbuf.salary,cinbuf.usrname);
	char *errmsg=NULL;
	//先查找看此员工是否存在
	if(1==do_search_self(db,cinbuf,newfd))
	{
		//员工存在，进行修改
		//调用sqlite3_exec函数进行修改手机号
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
		{
			printf("sqlite3_exec:%s__%d__\n",errmsg,__LINE__);
			return -1;
		}
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type='Y';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return 1;
	}
	else
	{
		//员工不存在无法修改
		memset(&cinbuf,0,sizeof(cinbuf));
		cinbuf.type ='N';
		res=send(newfd,&cinbuf,sizeof(cinbuf),0);
		if(res<0)
		{
			ERR_MSG("send");
			return -1;
		}
		return -1;
	}
	return 0;
}

