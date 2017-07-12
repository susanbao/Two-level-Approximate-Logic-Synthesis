#include<iostream>
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<malloc.h>
#include<queue>

using namespace std;
const int maxc=17;
FILE* f_in;
FILE* f_out;
char s[100];
char u[50];
int c=maxc-1;
int mindl[maxc];
int power[5]={1,10,100,1000,10000};

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

void MakeError(int deerror,int file) //file is name as XXYY, XX is the top error, YY=XX-deerror.
{
	if(deerror>=3||file>9999||file<100)
		return;
	memset(s,0,sizeof(s));
	memset(u,0,sizeof(u));
	strcat(s,"./mult_021.exe ");
	u[0]=deerror+'0';
	strcat(s,u);
	excelsystem(s);
	excelsystem("./espresso_ubuntu_amd64.exe <a_out.out >result_second.ans 2>r.err");
	memset(s,0,sizeof(s));
	memset(u,0,sizeof(u));
	strcat(s,"./compute_txt.exe ");
	for(int i=0;i<4;i++)
		u[i]=(file/power[3-i])%10+'0';
	strcat(s,u);
	excelsystem(s);
	f_in=fopen("record_mindl.ans","r");
	memset(s,0,sizeof(s));
	fgets(s,10,f_in);
	int nowdl=0;
	for(int i=0;s[i]!='.'&&i<10;i++)
		nowdl=nowdl*10+s[i]-'0';
	fclose(f_in);
	int error=c-file%100;
	if(mindl[error]>nowdl)
	{
		mindl[error]=nowdl;// the smaller, the better
		memset(s,0,sizeof(s));
		strcat(s,"cp b_");
		strcat(s,u);
		strcat(s,".txt r_");
		memset(u,0,sizeof(u));
		int m=0;
		for(int i=0;error/power[i]>0;i++)
			m++;
		for(int i=m;i>0;i--)
			u[m-i]=int(error/power[i-1])%10+'0';
		strcat(s,u);
		strcat(s,".best");
		excelsystem(s);
	}
}

void Recursion(int reminderror,int file)
{
	if(reminderror<=0)
		return;
	memset(s,0,sizeof(s));
	memset(u,0,sizeof(u));
	strcat(s,"cp b_");
	for(int i=0;i<4;i++)
		u[i]=(file/power[3-i])%10+'0';
	strcat(s,u);
	strcat(s,".txt a_in.in");
	excelsystem(s);
	MakeError(1,reminderror*100+(reminderror-1));
	Recursion(reminderror-1,reminderror*100+(reminderror-1));
	if(reminderror>=2)
	{
		memset(s,0,sizeof(s));
		memset(u,0,sizeof(u));
		strcat(s,"cp b_");
		for(int i=0;i<4;i++)
			u[i]=(file/power[3-i])%10+'0';
		strcat(s,u);
		strcat(s,".txt a_in.in");
		excelsystem(s);
		MakeError(2,reminderror*100+(reminderror-2));
		Recursion(reminderror-2,reminderror*100+(reminderror-2));
	}
}

void Begin(int reminderror)
{
	int file;
	if(reminderror<=0)
		return;
	excelsystem("cp b_0000.txt a_in.in");
	file=c*100;
	MakeError(1,file+(c-1));
	Recursion(c-1,file+(c-1));
	if(reminderror>1)
	{
		excelsystem("cp b_0000.txt a_in.in");
		MakeError(2,file+(c-2));
		Recursion(c-2,file+(c-2));
	}
	return;
}

int main(void)
{
	for(int i=0;i<maxc;i++)
		mindl[i]=(1<<15);
	Begin(c);
	int error=c;
	for(int i=0;i<maxc;i++)
		if(mindl[i]<mindl[error])
			error=i;
	printf("the literal of file will be %d after approximate!",mindl[error]);
	memset(s,0,sizeof(s));
	memset(u,0,sizeof(u));
	strcat(s,"cp r_");
	int m=0;
	for(int i=0;(error/power[i])>0;i++)
		m++;
	for(int i=m;i>0;i--)
		u[m-i]=int(error/power[i-1])%10+'0';
	strcat(s,u);
	strcat(s,".best result_final.ans");
	excelsystem(s);
	printf("running time: %ld\n",clock()/1000);
	return 0;
}
