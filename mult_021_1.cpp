#include<iostream>
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<malloc.h>
#include<queue>
//after record_44_2 example 6

using namespace std;

int cir;
const int power[11]={1,3,9,27,81,243,729,2187,6561,19683,59049};
const int maxc=3;
const int maxbit=8;   
const int maximum=59049;
const int maxout=7;
const int maxmultout=128;	//2^m
int c=maxc-1;
int m,n,u,x,y,top=0;
char s[maxbit+maxout+10];
int mindl[maxc];

struct cube //cube to be complemented
{
    int mi;
    int mo;
	int literal;
    int size;
	int imp; //number of implicant
	cube *c;
};

struct minterm  //error minterm part
{
    int nem; // number of error minterm
    int dl; //decrease literal
    int *mi; //input part of error minterm
    int *mo; //output part of error minterm
    cube *c;
    int order;
};

struct map
{
    int cube;
    map *p;
};
int xi[100],yi[100];

FILE* f_in;
FILE* f_out;
int sum[maximum][maxout];
bool o[maximum][maxout];
int h[maximum][maxmultout];
bool flag=false;
minterm *mint[maximum];
map *mymap[maximum];
int ei[maximum][maxmultout]; //the literal of essential implicant
int em[maximum][maxout];  //the number of minterm appearing
int fem[maximum][maxout]; //the father implicant of minterm
queue<int> q1;
int ermi[32];   //store the input and output of error minterm temporarily
int ermo[32];
int minx[maxc][maxc]; //the first vector is error part the second vector is the input part of minterm
int mino[maxc][maxc*2];    // the first vector is error part, store the final output part of complement minterms
int nim[maximum][maxout];// number of implicant's minterm
int ncm[maximum][maxout];//number of cube's minterm
bool fncm[maximum];
int impmi[100]; //store the input and output of contained implicant temporarily.
int impmo[100];
int unitliteral; //the maximal decreasing literal for single error
int MTER=0; //this MTER=the idea MTER+1

inline int calc(int x,int k)
{
    if (o[x][k]) return sum[x][k];
    for (int i=0;i<m;i++)
        if ((x/(power[i]))%3==2)
        {
            sum[x][k]=calc(x-power[i],k)+calc(x-power[i]*2,k);
            o[x][k]=true;
            return sum[x][k];
        }
    fprintf(f_out,"\n!!wrong!!\n");
    return -1;
}

inline int get3(int x) //The number of 2 in the ternary notation of x
{
    int s=0;
    for (int i=0;i<m;i++)
        if ((x/power[i])%3==2) s++;
    return s;
}

inline int get2(int x) //The number of 1 in the binary notation of x
{
    int s=0;
    while(x!=0)
    {
        x&=x-1;
        s++;
    }
    return s;
}

inline void init(int x,int y,int f)
{
    if (x==m)
    {
        for (int i=m+1;i<=m+n;i++)
            if (s[i]=='1')
            {
                sum[y][i-x-1]=1;
                em[y][i-x-1]++;
                fem[y][i-x-1]=f;
            }
        return;
    }
    if (s[x]=='-')
    {
        init(x+1,y*3,f);
        init(x+1,y*3+1,f);
    }
    else init(x+1,y*3+s[x]-'0',f);
}

int init_implicant()
{
    int locx=0;
    int locy=0;
    int locn=0;
    for(int i=0;i<m;i++)
    {
        if(s[i]=='-')
            locx=locx*3+2;
        else{
            locx=locx*3+s[i]-'0';
            locn++;
        }
    }
    for(int i=m+1;i<=m+n;i++)
    {
        if(s[i]=='1')
        {
            locy=locy+(1<<(i-m-1));
            locn++;
        }
    }
    ei[locx][locy]=locn;
    return locx;
}

void push(int x,int y) // push element into the queue
{
    int p=y*power[m]+x;
    q1.push(p);
    return;
}

void pop(int *x,int *y) //pop elemenet from the queue
{
    if(!q1.empty())
    {
        int p=q1.front();
        q1.pop();
        *y=p/power[m];
        *x=p-(*y)*power[m];
    }
    else
        printf("The queue is empty!\n");
}

void Errornum(int x,int y,int v) //find the number of error minterm in the cube, the minterm found is ordered.
{
    if(v==m)
    {
        if(u>=maxc)
            return;
        for(int i=0;i<n;i++)
            if((y>>i)%2&&sum[x][i]==0)
            {
                ermi[u]=x;
                ermo[u]=ermo[u]|(1<<i);
            }
        if(ermo[u]!=0)
            u++;
    }
    else if((x/power[v])%3==2)
    {Errornum(x-power[v],y,v+1);
        Errornum(x-power[v]*2,y,v+1);}
    else
        Errornum(x,y,v+1);
}

bool includeflag(int x,int xi)
{
    for(int i=0;i<m;i++)
    {
        if(((x/power[i])%3)!=2&&((xi/power[i])%3)!=((x/power[i])%3))
            return false;
    }
    return true;
}

bool IncludeAllEssentialMinterm(int x,int y,int fx,int v)
{
    if(v>=m)
    {
        if(em[x][y]==1)   // essential minterm and not include in fx
            return includeflag(fx,x);
        return true;
    }
    else if((x/power[v])%3==2)
    {
        if(!IncludeAllEssentialMinterm(x-power[v],y,fx,v+1))
            return false;
        return IncludeAllEssentialMinterm(x-power[v]*2,y,fx,v+1);
    }
    else
        return IncludeAllEssentialMinterm(x,y,fx,v+1);
}

void FindEssentialMinterm(int x,int y, int fx,int fy,int *childx, int *childy,int v)
{
    if(v>=m)
    {
        if(em[x][y]==1)
        {
            int cx=fem[x][y];
            for(int i=0;i<top;i++)
                if(cx==childx[i])
                    return;
            int cy=0;
            for(int i=1;i<(1<<n);i++)
                if(ei[cx][i]!=0)
                {cy=i;break;}
            if((fy&cy)!=cy)
                return;
            for(int i=0;i<n;i++)
                if((cy>>i)%2==1)
                    if(!IncludeAllEssentialMinterm(cx,i,fx,0))
                        return;
            childx[top]=cx;
            childy[top]=cy;
            top++;
        }
    }
    else if((x/power[v])%3==2)
    {
        FindEssentialMinterm(x-power[v],y,fx,fy,childx,childy,v+1);
        FindEssentialMinterm(x-power[v]*2,y,fx,fy,childx,childy,v+1);
    }
    else
        FindEssentialMinterm(x,y,fx,fy,childx,childy,v+1);
}

int ContainEssential(int x,int y,int lm,int ln)
{
    int childx[ln<<lm];
    int childy[ln<<lm];
    memset(childx,-1,sizeof(childx));
    memset(childy,-1,sizeof(childy));
    top=0;
    for(int i=0;i<n;i++)
        if((y>>i)%2==1)
        {
            FindEssentialMinterm(x,i,x,y,childx,childy,0);
        }
    int sumliteral=0;
    for(int i=0;i<top;i++) {
        sumliteral = sumliteral + ei[childx[i]][childy[i]];
        xi[i]=childx[i];
        yi[i]=childy[i];
    }
    return sumliteral;
}

void SetHto3(int x,int y,int lm,int ln)
{
    if(h[x][y]==3)	return;
    if(lm>0)
        for(int k=0;k<m;k++)
            if((x/power[k])%3==2)
            {
                SetHto3(x-power[k],y,lm-1,ln);
                h[x-power[k]][y]=3;
                SetHto3(x-power[k]*2,y,lm-1,ln);
                h[x-power[k]*2][y]=3;
            }
    if(ln>1)
        for(int k=0;k<n;k++)
            if((y>>k)%2==1)
            {
                SetHto3(x,y-(1<<k),lm,ln-1);
                h[x][y-(1<<k)]=3;
            }
}

int find_hash(int all,int p)
{
    int x=all%power[m];
    map *head=mymap[x];
    while(head!=0)
    {
        int locx=head->cube;
        if(p==mint[locx]->nem)
        {
            int i;
            for(i=0;i<p;i++)
            {
                if(ermi[i]!=mint[locx]->mi[i])
                    break;
            }
            if(i==p)
                return locx;
        }
         head=head->p;
    }
    return -1;
}

void Insert_cube(cube *cub,minterm *min)
{
    if(min->c==NULL)
        min->c=cub;
    else
    {
        cube *head=min->c;
        cube *tail=head;
        while(head!=NULL)
        {
            if(head->size<cub->size||(head->size==cub->size&&head->imp<=cub->imp)) //increasing order,<= is better
                {tail=head;head=head->c;}
            else
            {
                if(tail==head)
                {
                    min->c=cub;
                    cub->c=tail;
                }
                else
                {
                    tail->c=cub;
                    cub->c=head;
                }
                break;
            }
        }
        if(head==NULL)
            tail->c=cub;
    }
    return;
}


void Set_hash(int all,int locx) //Build the relationship between error minterm and cube
{
    int x=all%power[m];
    map *head=mymap[x];
    map *t=new map;
    t->p=NULL;
    t->cube=locx;
    if(head==NULL)
    {mymap[x]=t;return;}
    map *tail=head;
    while(head!=0)
    {
        tail=head;
        head=head->p;
    }

    tail->p=t;
    return ;
}

void dp_cube()
{
    int x,y;
    pop(&x,&y);
    if(h[x][y]) return;
    h[x][y]=1;
    if(y==0)
        return;
    int lm=get3(x);
    int ln=get2(y);
    u=0;
    memset(ermo,0,sizeof(ermo));
    Errornum(x,y,0);
    int p=u;
    flag=false;
    if(p<MTER&&p>0)
    {
        flag=true;
        for(int j=0;j<n;j++)
            if((y>>j)%2&&sum[x][j]==0)
            {flag=false;break;}
    }
    if(flag)
    {
        int cubedl=ContainEssential(x,y,lm,ln)-(m-lm+ln);
        if(cubedl>=0) //don't know whether it should be > or >=
        {

            if(p==1)
            {
                SetHto3(x,y,lm,ln);
                u=ermi[0];
                if(mint[u]==0)
                {
                    mint[u]=new minterm;
                    mint[u]->dl=0;
                    mint[u]->mo=new int[1];
                    mint[u]->c=NULL;
                    mint[u]->nem=1;
                    mint[u]->mi=new int[1];
                    mint[u]->mi[0]=u;
                    mint[u]->mo[0]=ermo[0];
                    mint[u]->order=-1;
                }
                mint[u]->mo[0]|=ermo[0];
            }
            else
            {
                int allmin=0;
                for(int i=0;i<p;i++)
                    allmin+=ermi[i];
                u=find_hash(allmin,p);
                if(u==-1)
                {
                    u=x;
                    mint[u]=new minterm;
                    mint[u]->dl=0;
                    mint[u]->mo=new int[p];
                    mint[u]->c=NULL;
                    mint[u]->nem=p;
                    mint[u]->mi=new int[p];
                    mint[u]->order=-1;
                    for(int i=0;i<p;i++)
                    {
                        mint[u]->mi[i]=ermi[i];
                        mint[u]->mo[i]=ermo[i];
                    }
                    Set_hash(allmin,u);
                }
            }
            if(mint[u]->c!=NULL)
            {
                for(int i=0;i<p;i++)
                    for(int j=0;j<mint[u]->nem;j++)
                        if(ermi[i]==mint[u]->mi[j])
                        {mint[u]->mo[j]|=ermo[i];break;}
            }
            cube *head=mint[u]->c;
            while(head!=NULL)
            {
                if(head->mi==x)
                {
                    if((head->mo|y)!=head->mo)
                    {
                        head->mo|=y;
                        ln=get2(head->mo);
                        head->literal=m-lm+ln;
                        head->size=pow(2,lm)*ln;
                    }
                    break;
                }
                head=head->c;
            }
            if(head==NULL)
            {
                cube *cub=new cube;
                cub->mi=x;
                cub->mo=y;
                cub->imp=top;
                cub->literal=m-lm+ln;
                cub->size=pow(2,lm)*ln;
                cub->c=NULL;
                Insert_cube(cub,mint[u]);
            }
        }
    }
    if(lm==1) return;
    for(int k=0;k<m;k++)
        if((x/power[k])%3==2)
        {
            push(x-power[k],y);
            push(x-power[k]*2,y);
        }
    for(int k=0;k<n;k++) // the child cube only has one variable(output or input) different with the parent cube.
    {
        if((y>>k)%2==1&&ln>1)
            push(x,y-(1<<k));
    }
    return ;
}

int FindHash(int fx,int all,int error)
{
    int x=all%power[m];
    map *head=mymap[x];
    while(head!=0)
    {
        int locx=head->cube;
        if(mint[locx]->nem==error)
        {
            for(int i=0;i<mint[locx]->nem;i++)
            {
                flag=false;
                for(int j=0;j<mint[fx]->nem;j++)
                    if(mint[locx]->mi[i]==mint[fx]->mi[j])
                    {flag=true;break;}
                if(!flag)
                    break;
            }
            if(flag)
                return locx;
        }
        head=head->p;
    }
    return -1;
}

void Merge_minterm(int fx,int x)
{
    cube *head=mint[x]->c;
    cube *fhead;
    bool f;
    for(int i=0;i<mint[fx]->nem;i++)
        for(int j=0;j<mint[x]->nem;j++)
            if(mint[fx]->mi[i]==mint[x]->mi[j])
            {
                mint[fx]->mo[i]|=mint[x]->mo[j];
                break;
            }
    while(head!=NULL)
    {
        fhead=mint[fx]->c;
        f=true;
        while(fhead!=NULL)
        {
            if(fhead->mi==head->mi)
            {
                f=false;
                fhead->mo|=head->mo;
                break;
            }
            fhead=fhead->c;
        }
        if(f)
        {
            cube *cub=new cube;
            cub->imp=head->imp;
            cub->mi=head->mi;
            cub->mo=head->mo;
            cub->c=NULL;
            cub->size=head->size;
            cub->literal=head->literal;
            Insert_cube(cub,mint[fx]);
        }
        head=head->c;
    }
}

void Find_sonminterm(int x,int all,int loc,int error)
{
    int sx=-1;
    int sall=0;
    if(error<2)
        return;
    if(error>2)
        flag=true;
    for(int i=loc;i<mint[x]->nem;i++)
    {
        sall=all-mint[x]->mi[i];
        if(error>2)
            sx=FindHash(x,sall,error-1);
        else if(error==2)
            sx=sall;
        if(sx==-1)
            Find_sonminterm(x,sall,i+1,error-1);
        else if(mint[sx]!=0&&!fncm[sx])
        {
            fncm[sx]=true;
            Merge_minterm(x,sx);
        }
    }
    return ;
}


void Add_cubeminterm(int x,int y,int v)
{
    if(v>=m)
    {
        for(int i=0;i<n;i++)
            if((y>>i)%2)
            {
                ncm[x][i]++;
            }
    }
    else if((x/power[v])%3==2)
    {
        Add_cubeminterm(x-power[v],y,v+1);
        Add_cubeminterm(x-power[v]*2,y,v+1);
    }
    else
        Add_cubeminterm(x,y,v+1);
}

void Add_impminterm(int x,int y,int v)
{
    if(v>=m)
    {
        for(int i=0;i<n;i++)
            if((y>>i)%2)
                nim[x][i]++;
    }
    else if((x/power[v])%3==2)
    {
        Add_impminterm(x-power[v],y,v+1);
        Add_impminterm(x-power[v]*2,y,v+1);
    }
    else
        Add_impminterm(x,y,v+1);
}

void Judge_cube(int x,int y,int v)
{
    if(flag)
        return ;
    if(v>=m)
    {
            if(ncm[x][y]==1&&nim[x][y]>=em[x][y]&&em[x][y]!=0)
                {flag=true; return ;}
    }
    else if((x/power[v])%3==2)
    {
        Judge_cube(x-power[v],y,v+1);
        Judge_cube(x-power[v]*2,y,v+1);
    }
    else
        Judge_cube(x,y,v+1);
}

void Decrease_cubeminterm(int x,int y,int v)
{
    if(v>=m)
    {
                ncm[x][y]--;
    }
    else if((x/power[v])%3==2)
    {
        Decrease_cubeminterm(x-power[v],y,v+1);
        Decrease_cubeminterm(x-power[v]*2,y,v+1);
    }
    else
        Decrease_cubeminterm(x,y,v+1);
}

void Judge_implicant(int x,int y,int v)
{
    if(v>=m)
    {
        for(int i=0;i<n;i++)
            if((y>>i)%2&&nim[x][i]>=em[x][i]&&ncm[x][i]<=0)
            {
                flag=false;
                break;
            }
    }
    else if((x/power[v])%3==2)
    {
        Judge_implicant(x-power[v],y,v+1);
        Judge_implicant(x-power[v]*2,y,v+1);
    }
    else
        Judge_implicant(x,y,v+1);
}

void Decrease_implicant(int x,int y,int v)
{
    if(v>=m)
    {
        for(int i=0;i<n;i++)
            if((y>>i)%2)
            {
                nim[x][i]--;
            }
    }
    else if((x/power[v])%3==2)
    {
        Decrease_implicant(x-power[v],y,v+1);
        Decrease_implicant(x-power[v]*2,y,v+1);
    }
    else
        Decrease_implicant(x,y,v+1);
}

void Check_IncludeImp(int x,int y,int v)
{
    if(v>=m)
    {
        for(int i=0;i<n;i++)
            if((y>>i)%2&&(em[x][i]==1||em[x][i]-nim[x][i]==1)&&ncm[x][i]==0)
            {flag=false;break;}
    }
    else if((x/power[v])%3==2)
    {
        Check_IncludeImp(x-power[v],y,v+1);
        Check_IncludeImp(x-power[v]*2,y,v+1);
    }
    else
        Check_IncludeImp(x,y,v+1);
}

void Delete_implicant(int x,int y,int v)
{
    if(v>=m)
    {
        for(int i=0;i<n;i++)
            if((y>>i)%2&&em[x][i]==1)
                ncm[x][i]=-1;
    }
    else if((x/power[v])%3==2)
    {
        Delete_implicant(x-power[v],y,v+1);
        Delete_implicant(x-power[v]*2,y,v+1);
    }
    else
        Delete_implicant(x,y,v+1);
}




void Find_implicant(int x)
{
    int y=0;
    for(int i=0;i<(1<<n);i++)
        if(ei[x][i]!=0)
        {
            y=i;
            break;
        }
    flag=true;
    Check_IncludeImp(x,y,0);
    if(flag)
    {
        impmi[top]=x;
        impmo[top++]=y;
        Add_impminterm(x,y,0);
    }
    Delete_implicant(x,y,0);
}

void dp_minterm(int x)    //postorder traversal, decrease the unnecessary cube
{
    if(o[x][0]) return;
    o[x][0]=true;
    for(int k=0;k<m;k++)
        if((x/power[k])%3==2)
        {
            dp_minterm(x-power[k]);
            dp_minterm(x-power[k]*2);
        }
    if(mint[x]!=0)
    {
        if(mint[x]->nem>1)
        {
            int all=0;
            for(int i=0;i<mint[x]->nem;i++)
                all+=mint[x]->mi[i];
            memset(fncm,0,sizeof(fncm));
            Find_sonminterm(x,all,0,mint[x]->nem);
        }
        memset(nim,0,sizeof(nim));
        memset(ncm,0,sizeof(ncm));
        memset(fncm,0,sizeof(fncm));
        cube *head=mint[x]->c;
        int cubedl=0;
        int impdl=0;
        while(head!=NULL)
        {
            if(!fncm[head->mi])
            {Add_cubeminterm(head->mi,head->mo,0);fncm[head->mi]=true;}
            head=head->c;
        }
        top=0;
        for(int i=0;i<power[m];i++)
            for(int j=0;j<n;j++)
                if(ncm[i][j]>0&&em[i][j]==1)
                    Find_implicant(fem[i][j]); //have found all implicant
        memset(ncm,0,sizeof(ncm));
        memset(fncm,0,sizeof(fncm));
        head=mint[x]->c;
        while(head!=NULL)
        {
            if(!fncm[head->mi])
            {Add_cubeminterm(head->mi,head->mo,0);fncm[head->mi]=true;}
            head=head->c;
        }
        head=mint[x]->c;
        int cubemo=0;
        while(head!=NULL) //find the min cube gather, but only record the sum of literals of them, don't not decrease them
        {
            cubemo=0;
            for(int i=0;i<n;i++)
                if(((head->mo)>>i)%2)
                {
                    flag=false;
                    if(fncm[head->mi])
                        Judge_cube(head->mi,i,0);
                    if(flag)
                        cubemo+=1<<i;
                    else
                        Decrease_cubeminterm(head->mi,i,0);
                }
            if(cubemo==0)   //whether it should be deleted
            {
                head=head->c;
            }
            else
            {
                fncm[head->mi]=false;
                if(head->mo!=cubemo)
                {
                    cubedl+=m-get3(head->mi)+get2(cubemo);
                }
                else
                    cubedl+=head->literal;
                head=head->c;
            }
        }
        for(int i=0;i<top;i++)  //decrease the redundant implicant
        {
            flag=true;
            Judge_implicant(impmi[i],impmo[i],0);
            if(flag)
                impdl+=ei[impmi[i]][impmo[i]];
            else
                Decrease_implicant(impmi[i],impmo[i],0);
        }
        mint[x]->dl=impdl-cubedl;
        for(int i=0;i<mint[x]->nem;i++)// decrease the redundant output part of minterms
            for(int j=0;j<n;j++)
                if((mint[x]->mo[i]>>j)%2==1&&ncm[mint[x]->mi[i]][j]==0)
                    mint[x]->mo[i]&=((1<<n)-1-(1<<j));
    }
    return ;
}

void full(int x,int y)
{
    for(int i=0;i<n;i++)
        if((y>>i)%2==1&&sum[x][i]==0)
        {
            //printf("Change the %d minterm on %d output\n",x,i);
            sum[x][i]=1;
        }
    return;
}


void Set_order(int x) //decreasing order
{
    if(u==-1)
        u=x;
    else
    {
        int head=u;
        int tail=head;
        while(head!=-1)
        {
            if((mint[x]->dl*1.0/mint[x]->nem)<(mint[head]->dl*1.0/mint[head]->nem))
            {tail=head;head=mint[head]->order;}
            else
            {
                if(head==u)
                {
                    u=x;
                    mint[x]->order=head;
                }
                else
                {
                    mint[tail]->order=x;
                    mint[x]->order=head;
                }
                break;
            }
        }
        if(head==-1)
        {
            mint[tail]->order=x;
        }
    }
}



bool Compute_literal(int *x,int num)
{
    int cubedl=0;
    int impdl=0;
    memset(ncm,0,sizeof(ncm));
    memset(nim,0,sizeof(nim));
    memset(fncm,0,sizeof(fncm));
    cube *head;
    /***changing part***/
    for(int i=num-1;i>=0;i--)
        if(x[i]!=-1)
        {
            head=mint[x[i]]->c;
            while(head!=NULL)
            {
                if(!fncm[head->mi])
                {Add_cubeminterm(head->mi,head->mo,0);fncm[head->mi]=true;}
                head=head->c;
            }
        }
    top=0;
    for(int i=0;i<power[m];i++)
        for(int j=0;j<n;j++)
            if(ncm[i][j]>0&&em[i][j]==1)
                Find_implicant(fem[i][j]); //have found all implicant
    memset(ncm,0,sizeof(ncm));
    memset(fncm,0,sizeof(fncm));
    for(int i=num-1;i>=0;i--)
        if(x[i]!=-1)
        {
            head=mint[x[i]]->c;
            while(head!=NULL)
            {
                if(!fncm[head->mi])
                {Add_cubeminterm(head->mi,head->mo,0);fncm[head->mi]=true;}
                head=head->c;
            }
        }
    int cubemo=0;
    for(int i=num-1;i>=0;i--)
        if(x[i]!=-1)
        {
            head=mint[x[i]]->c;
            while(head!=NULL)
            {
                cubemo=0;
                for(int j=0;j<n;j++)
                    if((head->mo>>j)%2)
                    {
                        flag=false;
                        if(fncm[head->mi])
                            Judge_cube(head->mi,j,0);
                        if(flag)
                            cubemo+=1<<j;
                        else
                            Decrease_cubeminterm(head->mi,j,0);
                    }
                if(cubemo!=0)
                {

                    fncm[head->mi]=false;
                    if(head->mo==cubemo)
                        cubedl+=head->literal;
                    else
                        cubedl+=m-get3(head->mi)+get2(cubemo);
                }
                head=head->c;
            }
        }
    for(int i=0;i<top;i++)
    {
        flag=true;
        Judge_implicant(impmi[i],impmo[i],0);
        if(flag)
            impdl+=ei[impmi[i]][impmo[i]];
        else
            Decrease_implicant(impmi[i],impmo[i],0);
    }

    if(impdl-cubedl>mindl[num])
    {
        mindl[num]=impdl-cubedl;
        int nummin=0;
        for(int k=0;k<num;k++)
        {
            minx[num][k]=x[k];
            if(x[k]!=-1)
                for(int i=0;i<mint[x[k]]->nem;i++)
                {
                    mino[num][nummin]=mint[x[k]]->mo[i];
                    for(int j=0;j<n;j++)
                        if((mint[x[k]]->mi[i]>>j)%2==1&&ncm[mint[x[k]]->mi[i]][j]==0)
                            mino[num][nummin]&=((1<<n)-1-(1<<j));
                    nummin++;
                }
        }
    }
    //else if(impdl-cubedl+unitliteral<mindl[num])
        //return false;
    return true;
    /***changing part***/
}

void Find_errorminterm2(int *temp,int order,int error)
{
    if(error<=0)
        return;
    int x[c];
    for(int i=0;i<(c-error);i++)
        x[i]=temp[i];
    int addx=order;
    int addx_error=0;
    while(addx!=-1)
    {
        //if(mint[addx]->nem<=error)
        //{
        addx_error=mint[addx]->nem;
        for(int i=0;i<mint[addx]->nem;i++)// find the error number of the combining minterm tree.
        {
            flag=true;
            for(int j=0;j<(c-error);j++)
            {
                if(x[j]!=-1)
                    for(int k=0;k<mint[x[j]]->nem;k++)
                        if(mint[x[j]]->mi[k]==mint[addx]->mi[i])
                        {flag=false;break;}
                if(!flag)
                    break;
            }
            if(!flag)
                addx_error--;
        }
        if(addx_error>0&&addx_error<=error)
        {
            x[c-error]=addx;
            for(int i=1;i<addx_error;i++)
                x[c-error+i]=-1;
            if(Compute_literal(x,c-error+addx_error))
                Find_errorminterm2(x,mint[addx]->order,error-addx_error);
        }
        //}
        addx=mint[addx]->order;
    }
    return;
}


void Find_errorminterm(int order,int error) // use dp to find the maximum minterm complement set
{
    int addx=order;
    int findmin=0;
    int x[c];
    while(addx!=-1)
    {

        if(mint[addx]->nem<=error)
        {
            x[0]=addx;
            for(int i=1;i<mint[addx]->nem;i++)
                x[i]=-1;
            if(mint[addx]->dl>mindl[mint[addx]->nem])
            {
                mindl[mint[addx]->nem]=mint[addx]->dl;
                for(int i=0;i<mint[addx]->nem;i++)
                {
                    minx[mint[addx]->nem][i]=x[i];
                    mino[mint[addx]->nem][i]=mint[addx]->mo[i];
                }
            }
            Find_errorminterm2(x,mint[addx]->order,error-mint[addx]->nem);
        }
        addx=mint[addx]->order;
        if((findmin++)>cir)
            return;
    }
    return;
}

void Change_em(int *x)
{
    memset(ncm,0,sizeof(ncm));
    memset(nim,0,sizeof(nim));
    memset(fncm,0,sizeof(fncm));
    int cubedl=0;
    int impdl=0;
    cube *head;
    for(int i=c-1;i>=0;i--)
        if(x[i]!=-1)
        {
            head=mint[x[i]]->c;
            while(head!=NULL)
            {
                if(!fncm[head->mi])
                {Add_cubeminterm(head->mi,head->mo,0);fncm[head->mi]=true;}
                head=head->c;
            }
        }
    top=0;
    for(int i=0;i<power[m];i++)
        for(int j=0;j<n;j++)
            if(ncm[i][j]>0&&em[i][j]==1)
                Find_implicant(fem[i][j]); //have found all implicant
    memset(ncm,0,sizeof(ncm));
    memset(fncm,0,sizeof(fncm));
    for(int i=c-1;i>=0;i--)
        if(x[i]!=-1)
        {
            head=mint[x[i]]->c;
            while(head!=NULL)
            {
                if(!fncm[head->mi])
                {Add_cubeminterm(head->mi,head->mo,0);fncm[head->mi]=true;}
                head=head->c;
            }
        }
    int cubemo=0;
    for(int i=c-1;i>=0;i--)
        if(x[i]!=-1)
        {
            head=mint[x[i]]->c;
            while(head!=NULL)
            {
                cubemo=0;
                for(int j=0;j<n;j++)
                    if((head->mo>>j)%2)
                    {
                        flag=false;
                        if(fncm[head->mi])
                            Judge_cube(head->mi,j,0);
                        if(flag)
                            cubemo+=1<<j;
                        else
                            Decrease_cubeminterm(head->mi,j,0);
                    }
                if(cubemo!=0)
                {

                    fncm[head->mi]=false;
                    if(head->mo==cubemo)
                        cubedl+=head->literal;
                    else
                        cubedl+=m-get3(head->mi)+get2(cubemo);
                    ei[head->mi][cubemo]=100;
                }
                head=head->c;
            }
        }
    for(int i=0;i<top;i++)
    {
        flag=true;
        Judge_implicant(impmi[i],impmo[i],0);
        if(flag)
        {
            impdl+=ei[impmi[i]][impmo[i]];
            ei[impmi[i]][impmo[i]]=0;
        }
        else
            Decrease_implicant(impmi[i],impmo[i],0);
    }

}


int main(int argc,char **argv)
{
    c=0;
    for(int i=0;argv[1][i];i++)
	c=c*10+argv[1][i]-'0';
    f_in=fopen("a_in.in","r");
    fscanf(f_in,".i %d\n",&m);
    fscanf(f_in,".o %d\n",&n);
    fscanf(f_in,".p %d\n",&x);
    memset(sum,0,sizeof(sum));
    memset(ei,0,sizeof(ei));
    memset(xi,-1,sizeof(xi));
    memset(yi,-1,sizeof(yi));
    while (true)
    {
        fgets(s,m+n+10,f_in);
        if (s[0]=='.') break;
        top=init_implicant();
        init(0,0,top);

    }
    memset(o,0,sizeof(o));
    for (int i=0;i<(1<<m);i++)
    {
        y=0;
        for (int j=m-1;j>=0;j--)
        {
            y*=3;
            y+=(i>>j)%2;
        }
        for (int j=0;j<n;j++)
            o[y][j]=true;
    }

    for (int i=0;i<power[m];i++)
        for (int j=0;j<n;j++)
            sum[i][j]=calc(i,j);
    memset(h,0,sizeof(h));
    int vn= pow(2,n)-1;
    push(power[m]-1,vn);
    if(c==1)
        MTER=2;
    else if(int(log(c)/log(2))>2)
        MTER=int(log(c)/log(2))+1;
    else
        MTER=3;
    while(!q1.empty())
        dp_cube();

    ei[maximum-1][maxmultout-1]=0;
    memset(o,0,sizeof(o));
    dp_minterm(power[m]-1);


    for(int i=0;i<maxc;i++)
    {
        for(int j=0;j<maxc;j++)
            minx[i][j]=-1;
        mindl[i]=0;
    }

    u=-1;
    for(int i=0;i<power[m];i++) //give the order of all
        if(mint[i]!=0)
            Set_order(i);

    unitliteral=mint[u]->dl/mint[u]->nem;


    if(c==1)
    {
        minx[1][0]=mint[u]->mi[0];
        mino[1][0]=mint[u]->mo[0];
        mindl[1]=mint[u]->dl;
    }
    else
    {
	int countall=0;
        int count=0;
        for(int i=u;i!=-1;i=mint[i]->order)
            countall++;

        countall=countall;
	cir=countall;
	unitliteral=unitliteral/2;
        for(int i=u;i!=-1;i=mint[i]->order)
        {
            count++;
            if(count>countall)
            {
                mint[i]->order=-1;
                break;
            }
        }
        Find_errorminterm(u,c);
    }
    int max=0;
    for(int i=0;i<=c;i++)
        if(mindl[i]>mindl[max])
            max=i;

    if(mindl[max]>0)
    {
        Change_em(minx[max]);    //change the implicants into cubes
        top=0;
        for(int i=0;i<max;i++)    //complement the minterm
            for(int j=0;minx[max][i]!=-1&&j<mint[minx[max][i]]->nem;j++)
                full(mint[minx[max][i]]->mi[j],mino[max][top++]);
    }
    f_out=fopen("a_out.out","w");
    fprintf(f_out,".i %d\n",m);
    fprintf(f_out,".o %d\n",n);
    for (int i=0;i<(1<<m);i++)
    {
        y=0;
        for (int j=0;j<m;j++)
            y+=(i>>j)%2*power[j];
        for (int j=m-1;j>=0;j--)
            fprintf(f_out,"%d",(i>>j)%2);
        fprintf(f_out," ");
        for (int j=0;j<n;j++)
            fprintf(f_out,"%d",sum[y][j]);
        fprintf(f_out,"\n");
    }
    fprintf(f_out,".e\n");
    fclose(f_out);
    f_out=fopen("result_third.ans","w");   //store the cube and implicant into result_second.ans
    fprintf(f_out,".i %d\n",m);
    fprintf(f_out,".o %d\n",n);
    fprintf(f_out,".p %d\n",x);
    int k=0;
    for(int i=0;i<power[m];i++)
        for(int j=0;j<(1<<n);j++)
            if(ei[i][j]>0)
            {
                for(k=m-1;k>=0;k--)
                {
                    if((i/power[k])%3==2)
                        fprintf(f_out,"-");
                    else
                        fprintf(f_out,"%d",(i/power[k])%3);
                }
                fprintf(f_out," ");
                for(k=0;k<n;k++)
                    fprintf(f_out,"%d",(j>>k)%2);
                fprintf(f_out,"\n");
            }
    fprintf(f_out,".e\n");
    fclose(f_out);
    //printf("running time: %ld\n",clock()/1000);
    return 0;
}
