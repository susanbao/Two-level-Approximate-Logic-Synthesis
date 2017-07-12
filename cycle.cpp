#include<iostream>
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<malloc.h>
#include<queue>
// Since we should use BFS to identify the error minterm, I use 3-scale to name the approximate file with four ten-scale size.
using namespace std;
const int maxc=17;
const int H=10000;//the max search number
FILE* f_in;
FILE* f_out;
char s[100];
char u[50];
int c=maxc-1;
int mindl[maxc];
int minorder[maxc];
int literalorder[maxc];
const int p3[]={1,3,9,27,81,243,729,2187,6561,19683,59049,177147,531441,1594323,4782969,14348907,43046721};
const int power[8]={1,10,100,1000,10000,100000,1000000,10000000};
struct Error{
	int reerror;	// remind error
	int literal;	//the literal after approximating
	int file;	//the file(b_xxxx.txt) number 
};
Error *er[maxc][H];	//the point that only contain the top H reuslt

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

void EnterError(int reerror,int file,int nowdl)
{
	bool flag=false;
	for(int i=0;i<H;i++)
	{
		if(er[reerror][i]==0)
		{
			flag=true;
		}
		else if(er[reerror][i]->literal>nowdl)
		{
			flag=true;
			if(er[reerror][H-1]!=0)
				delete er[reerror][H-1];
			for(int j=H-1;j>i;j--)
			{
				er[reerror][j]=er[reerror][j-1];
			}	
		}
		if(flag)
		{
			Error *head=new Error;
			head->literal=nowdl;
			head->file=file;
			head->reerror=reerror;
			er[reerror][i]=head;
			break;
		}
	}
	return;
}


void MakeError(int reminderror,int deerror,int file,int order) //file is name as XXYY, XX is the top error, YY=XX-deerror.
{
	if(deerror>=3)
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
	for(int i=0;i<8;i++)
		u[i]=(file/power[7-i])%10+'0';
	strcat(s,u);
	excelsystem(s);
	f_in=fopen("record_mindl.ans","r");
	memset(s,0,sizeof(s));
	fgets(s,10,f_in);
	int nowdl=0;
	for(int i=0;s[i]!='.'&&i<10;i++)
		nowdl=nowdl*10+s[i]-'0';
	fclose(f_in);
	int error=c-reminderror+deerror;
	if(mindl[error]>nowdl)
	{
		mindl[error]=nowdl;// the smaller, the better
		minorder[error]=file;
		literalorder[error]=order;
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
	EnterError(reminderror-deerror,file,nowdl);
}

inline int get3(int x) //The number of 2 in the ternary notation of x
{
    int s=0;
    for (int i=0;i<maxc;i++)
        if ((x/p3[i])%3!=0) s++;
    return s;
}

void Begin(int reminderror)
{
	int num;
	if(reminderror<=0)
		return;
	excelsystem("cp b_0000.txt a_in.in");
	MakeError(reminderror,1,1,0);
	if(reminderror>1)
	{
		excelsystem("cp b_0000.txt a_in.in");
		MakeError(reminderror,2,2,0);
	}
	for(int i=reminderror-1;i>0;i--)
	{
		for(int j=0;j<H;j++)
			if(er[i][j]!=0)
			{
				memset(s,0,sizeof(s));
				memset(u,0,sizeof(u));
				strcat(s,"cp b_");
				for(int k=0;k<8;k++)
					u[k]=(er[i][j]->file/power[7-k])%10+'0';
				strcat(s,u);
				strcat(s,".txt a_in.in");
				excelsystem(s);
				num=get3(er[i][j]->file);
				MakeError(i,1,er[i][j]->file+(1*p3[num]),j);
				if(i>1)
				{
					MakeError(i,2,er[i][j]->file+(2*p3[num]),j);
				}		
			}
			else
			{
				break;
			}
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
	printf("the literal of file will be %d after approximate!\n",mindl[error]);
	for(int j=1;j<=c;j++)
	{
		printf("When error is %d, the approximate literal is %d",j,mindl[j]);
		printf(" and decrease order is ");
		for(int i=0;i<maxc;i++)
			printf("%d",(minorder[j]/p3[i])%3);
		printf("\n");
		printf("Its father is the top %d one\n",literalorder[j]);
	}
	
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
