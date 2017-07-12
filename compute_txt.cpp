#include<iostream>//change the multiple stitution
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<malloc.h>
#include<time.h>

using namespace std;

FILE* f_in;
FILE* f_out;
int m,n,x,y,c,num2,num3,d_num,i,num=0;
char s[100];
char t[100];

bool excelsystem(const char *cmdstring)
{
	int status;
	if(NULL == cmdstring) //如果cmdstring为空趁早闪退吧，尽管system()函数也能处理空指针
	{
    		return false;
	}
	status = system(cmdstring);
	if(status < 0)
	{
    	printf("cmd: %s\t ", cmdstring); // 这里务必要把errno信息输出或记入Log
    	return false;
	}

	if(WIFEXITED(status))
	{
    	//printf("normal termination, exit status = %d\n", WEXITSTATUS	(status)); //取得cmdstring执行结果
	}
	else if(WIFSIGNALED(status))
	{
    	printf("abnormal termination,signal number =%d\n", WTERMSIG(status)); //如果cmdstring被信号中断，取得信号值
	}
	else if(WIFSTOPPED(status))
	{
    	printf("process stopped, signal number =%d\n", WSTOPSIG(status)); //如果cmdstring被信号暂停执行，取得信号值
	}
	return true;
}

int main(int argc,char **argv)
{
	f_in=fopen("result_second.ans","r");
	fscanf(f_in,".i %d\n",&m);
	fscanf(f_in,".o %d\n",&n);
	fscanf(f_in,".p %d\n",&c);
	while(true)
	{
		memset(t,0,sizeof(t));
		fgets(t,m+n+10,f_in);
		for(int i=0;i<m;i++)
		if(t[i]=='1'||t[i]=='0')
		num2++;
		for(int i=m+1;i<=m+n;i++)
		if(t[i]=='1')
		num2++;
		if (t[0]=='.') break;
	}
	fclose(f_in);

	f_in=fopen("result_third.ans","r");
	fscanf(f_in,".i %d\n",&m);
	fscanf(f_in,".o %d\n",&n);
	fscanf(f_in,".p %d\n",&c);
	while(true)
	{
		memset(t,0,sizeof(t));
		fgets(t,m+n+10,f_in);
		for(int i=0;i<m;i++)
		if(t[i]=='1'||t[i]=='0')
		num3++;
		for(int i=m+1;i<=m+n;i++)
		if(t[i]=='1')
		num3++;
		if (t[0]=='.') break;
	}
	fclose(f_in);
	memset(s,0,sizeof(s));
	f_out=fopen("record_mindl.ans","w");
	if(num3>num2)
	{
		strcat(s,"cp result_second.ans b_");
		fprintf(f_out,"%d.",num2);
	}
	else
	{
		strcat(s,"cp result_third.ans b_");
		fprintf(f_out,"%d.",num3);
	}
	strcat(s,argv[1]);
	strcat(s,".txt");
	excelsystem(s);
	fclose(f_out);
	return 0;
}
