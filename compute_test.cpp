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
int m,n,x,y,c,num1,num2,num3,d_num,i,num=0;
char s[100];
char t[100];

int main()
{
	f_in=fopen("result_first.ans","r");
	fscanf(f_in,".i %d\n",&m);
	fscanf(f_in,".o %d\n",&n);
	fscanf(f_in,".p %d\n",&c);
	while(true)
	{
		memset(t,0,sizeof(t));
		fgets(t,m+n+10,f_in);
		for(int i=0;i<m;i++)
		if(t[i]=='1'||t[i]=='0')
		num1++;
		for(int i=m+1;i<=m+n;i++)
		if(t[i]=='1')
		num1++;
		if (t[0]=='.') break;
	}
	fclose(f_in);

	f_in=fopen("result_final.ans","r");
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


	d_num=num1-num2;
	printf("decrease literals:%d\n",d_num);
	printf("literals of original pla:%d\n",num1);
	printf("literals of changed pla:%d\n",num2);
	return 0;
}
