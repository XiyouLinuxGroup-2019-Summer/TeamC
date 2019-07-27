/*************************************************************************
	> File Name: lt_ls1.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月27日 星期六 17时24分41秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>


#define PARAM_NONE 0
#define PARAM_A    1
#define PARAM_L    2
#define PARAM_R    4

void analy_dirr(char param[][20],char * dirr);
void  analy_dirA();
void analy_dirno();
void analy_dirl();
void analy_dirAL();
void analy_dirR(char param[][20]);
void analy_dirRL(char param[][20]);
void display_attribute();
void display_single();
void analy_param(char param[][20]);

int t;

typedef struct p{
	
    char ptflname[256];
    char filename[256];
	struct p *next;

}P,*p;

P * phead;

void my_err(const char **err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    exit(1);
}

int main(int argc,char **argv)
{
    int i;
    char param[20][20];//参数存入该数组
    char dirr[256];

   
    for(i=1;i<argc;i++)
    {
        if(argv[i][0]=='-')
        {
            strcpy(param[i-1],argv[i]);
            t++;
        }

    }
    

    if(argv[argc-1][0]=='-')//当前目录
    {
        strcpy(dirr,"./");
        dirr[2]='\0';
    }
    else  //指定目录
    {
        strcpy(dirr,argv[argc-1]);
    }
    analy_dirr(param,dirr);
}

void analy_dirr(char param[][20],char *dirr)
{
    DIR *dir;
    struct dirent *ptr;
    
    P *pend,*pnew;
    
    phead=(p)malloc(sizeof(P));
    phead->next=NULL;
    pend=phead;
    struct stat buf;
    
    stat(dirr,&buf);
    int len;
    len=strlen(dirr);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
        
             if(dirr[len-1]!='/')
                {
                    dirr[len]='/';
                    dirr[len+1]='\0';
                }

        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {
            pnew=(p)malloc(sizeof(P));

            strcpy(pnew->filename,ptr->d_name);

            strncpy(pnew->ptflname,dirr,len+1);
            pnew->ptflname[len+1]='\0';
            strcat(pnew->ptflname,ptr->d_name);
            pnew->ptflname[len+strlen(ptr->d_name)+1]='\0';

            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
        }
        free(pnew);
        closedir(dir);
        
    }
    else//普通文件
    {
            pnew=(p)malloc(sizeof(P));
            strcpy(pnew->ptflname,dirr);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
    }
   	p f,y,x,p,t,q;
	f=NULL;
	while(f!=phead->next->next)
	{
		
		for(p=phead;f!=p->next->next;p=p->next)
		{
			
			if((strcmp(p->next->filename,p->next->next->filename))>0)
			{
				x=p->next;
				y=p->next->next;
				p->next=y;
				x->next=y->next;
				y->next=x;
				
			}
			
		}
		f=p->next;
	}
    analy_param(param);
}


void analy_param(char param[][20])
{
    int i,j;
    int flag_param=PARAM_NONE;

    for(i=0;i<t;i++)
    {
        for(j=1;j<strlen(param[i]);j++)
        {
            if(param[i][j] =='a')
            {
                flag_param |=PARAM_A;
            }
            else if(param[i][j]=='l')
            {
                flag_param |=PARAM_L;
            }
            else if(param[i][j]=='R')
            {
                flag_param |=PARAM_R;

            }
            else
            {("my_ls:invalid option -%c\n",param[i]);
                exit(1);
            }

        }
    }

    switch(flag_param)
    {
        case PARAM_NONE:
            analy_dirno();
            break;
        case PARAM_A:
            analy_dirA();
            break;
        case PARAM_L:
            analy_dirl();
            break;
        case PARAM_R:
            analy_dirR(param);
            break;
        case PARAM_A+PARAM_L:
            analy_dirAL();
            break;
        case PARAM_A+PARAM_R:
            analy_dirR(param);
            break;
        case PARAM_L+PARAM_R:
            analy_dirRL(param);
            break;
        case PARAM_A+PARAM_L+PARAM_R:
            analy_dirRL(param);
            break;

    }
}
void analy_dirA()
{
    p t;
    int i=1;
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            display_single(t->filename);
            i++;
        
        if(i%3==0)
        {
            printf("\n");
        }
        }
        t=t->next;
    }
    printf("\n");
}
void analy_dirno()
{
    p t;
    int i=1;
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            display_single(t->filename);
            i++;
        }
        if(i%3==0)
        {
            printf("\n");
        }
        t=t->next;
    }
}
void analy_dirl()
{
    p t;

    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            display_attribute(t->ptflname);
            printf("   %-s",t->filename);
            printf("\n");
        }
        t=t->next;
        
    }
}
void analy_dirAL()
{
    p t;
    t=phead->next;
    while(t)
    {
        display_attribute(t->ptflname);
        printf("   %-s",t->filename);
        printf("\n");
        t=t->next;
    }
}
void analy_dirR(char param[][20])
{   
    p t;
    int i=1;
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            display_single(t->filename);
            i++;
        }
        if(i%3==0)
        {
            printf("\n");
        }
        t=t->next;
     }
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            
            struct stat buf;
            stat(t->ptflname,&buf);
            if(S_ISDIR(buf.st_mode))
            {
                printf("%s:\n",t->filename);
                analy_dirr(param,t->ptflname);
               
            }
        }
        t=t->next;
    }

    printf("\n");
}

void analy_dirRL(char param[][20])
{
    p t;
    int i=1;
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            display_attribute(t->ptflname);
            printf("%s\n",t->filename);
        }
        t=t->next;
     }
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
            
            struct stat buf;
            stat(t->ptflname,&buf);
            if(S_ISDIR(buf.st_mode))
            {
                printf("%s:\n",t->filename);
                analy_dirr(param,t->ptflname);
               
            }
        }
        t=t->next;
    }


}






void display_attribute(char *name)
{
    char but_time[32];
    struct passwd *psd;
    struct group  *grp;
    struct stat buf;
    memset(&buf,0,sizeof(struct stat));
    stat(name,&buf);

    if(S_ISLNK(buf.st_mode))
    {
        printf("1");
    }
    else if(S_ISREG(buf.st_mode))
    {
        printf("-");
    }
    else if(S_ISDIR(buf.st_mode))
    {
        printf("d");
    }
    else if(S_ISCHR(buf.st_mode))
    {
        printf("c");
    }
    else if(S_ISBLK(buf.st_mode))
    {
        printf("b");
    }
    else if(S_ISFIFO(buf.st_mode))
    {
        printf("f");
    }
    else if(S_ISSOCK(buf.st_mode))
    {
        printf("s");
    }

    if(buf.st_mode & S_IRUSR)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }

    if(buf.st_mode & S_IWUSR)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }
    if(buf.st_mode & S_IXUSR)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }

    if(buf.st_mode & S_IRGRP)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }

    if(buf.st_mode & S_IWGRP)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }
    if(buf.st_mode & S_IXGRP)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }
    
    if(buf.st_mode & S_IROTH)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }

    if(buf.st_mode & S_IWOTH)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }
    if(buf.st_mode & S_IXOTH)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }
    
    printf("  ");

    psd=getpwuid(buf.st_uid);
    grp=getgrgid(buf.st_gid);
    printf("%-4d",buf.st_nlink);
    printf("%-6s",psd->pw_name);
    printf("%-6s",grp->gr_name);

    printf("%6d",buf.st_size);
    strcpy(but_time,ctime(&buf.st_mtime));
    but_time[strlen(but_time)-1]='\0';
    printf("  -%s",but_time);
}

void display_single(char *name)
{
    int len;
    len=sizeof(name);
    printf("%-20s",name);

  
}


