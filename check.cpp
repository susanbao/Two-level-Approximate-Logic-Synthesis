#include<iostream>//change the multiple stitution
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<malloc.h>

using namespace std;
const int power[11]={1,3,9,27,81,243,729,2187,6561,19683,59049};
const int maxbit=20;
const int maximum=1048576;
const int maxout=20;
int a_sum[maximum][maxout];
int b_sum[maximum][maxout];
char s[maxbit+maxout+10];
int m,n,x=0;
FILE* f_in;
FILE* f_out;

void init(int sum[][maxout],int x,int y)
{
	if (x==m) 
	{
		for (int i=m+1;i<=m+n;i++)
				if (s[i]=='1') 
				{
					sum[y][i-m-1]=1;
				}
		return;
	}
	if (s[x]=='-') 
	{
		init(sum,x+1,y*2);
		init(sum,x+1,y*2+1);
	}
	else init(sum,x+1,y*2+s[x]-'0');	
}
void PrintLoc(int locx,int locy)
{
	int mapx=0;
	printf("input part: ");
	for(int i=m-1;i>=0;i--)
	{
		printf("%d",(locx>>i)%2);
		mapx=mapx*3+(locx>>i)%2;
	}
	printf(", %d",mapx);
	printf(",  output part: ");
	//for(int i=n-1;i>=0;i--)
		printf("%d",locy);
	printf("\n");
	return;
}


int compare()
{
	int t=0;
	bool flag=false;
	for(int i=0;i<(1<<m);i++)
		{
		flag=false;
		for(int j=0;j<n;j++)
			if(a_sum[i][j]!=b_sum[i][j])
			{flag=true;PrintLoc(i,j);}
		if(flag)
		{t++;}
		}
	return t;
}
int main()
{
	f_in=fopen("result_first.ans","r");
	//f_in=fopen("a_in.in","r");
	fscanf(f_in,".i %d\n",&m);
	fscanf(f_in,".o %d\n",&n);
	fscanf(f_in,".p %d\n",&x);
	while (true)
	{
		fgets(s,m+n+10,f_in);
		if (s[0]=='.') break;
		init(a_sum,0,0);
	}
	fclose(f_in);

	f_in=fopen("result_final.ans","r");
	//f_in=fopen("r_EXS.ans","r");
	fscanf(f_in,".i %d\n",&m);
	fscanf(f_in,".o %d\n",&n);
	fscanf(f_in,".p %d\n",&x);
	while (true)
	{
		fgets(s,m+n+10,f_in);
		if (s[0]=='.') break;
		init(b_sum,0,0);
	}
	
	int result=compare();
        f_out=fopen("result_all.out","a");
        fprintf(f_out,"change minterm:%d\n",result);
	fclose(f_out);
	printf("change minterm:%d\n",result);
	return 0;
}
