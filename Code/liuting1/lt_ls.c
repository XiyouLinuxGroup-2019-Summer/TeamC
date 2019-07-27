
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

int g_leave_len=MAXNAMLEN;
int g_maxlen;

void my_err(const char **err_string,int line)
{
    fprintf(stderr,"line:%d",line);
    exit(1);
}

void  analy_param(char param[][20],char *dirr);
void  analy_dir(char *dirr);
void analy_dirno(char *dirr);
void analy_dirl(char *dirr);
void analy_dirAL(char *dirr);
void analy_dirR(char *dirr);
void analy_dirRL(char *dirr);
void display_attribute(char *name);
void display_single(char *name);

int t=0;


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
    analy_param(param,dirr);

}


void analy_param(char param[][20],char *dirr)
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
            analy_dirno(dirr);
            break;
        case PARAM_A:
            analy_dir(dirr);
            break;
        case PARAM_L:
            analy_dirl(dirr);
            break;
        case PARAM_R:
            analy_dirR(dirr);
            break;
        case PARAM_A+PARAM_L:
            analy_dirAL(dirr);
            break;
        case PARAM_A+PARAM_R:
            analy_dirR(dirr);
            break;
        case PARAM_L+PARAM_R:
            analy_dirRL(dirr);
            break;
        case PARAM_A+PARAM_L+PARAM_R:
            analy_dirRL(dirr);
            break;

    }
}
void analy_dir(char *dirr)
{
    DIR *dir;
    struct dirent *ptr;
    typedef struct flnm{
        char filename[100];
        struct flnm *next;
    }*fl,FL;
    FL *phead,*pend,*pnew;
    phead=(fl)malloc(sizeof(FL));
    phead->next=NULL;
    pend=phead;
    int i=0;
    struct stat buf;
    
    stat(dirr,&buf);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
        
             if(dirr[strlen(dirr)-1]!='/')
                {
                    dirr[strlen(dirr)]='/';
                    dirr[strlen(dirr)+1]='\0';
                }

       
        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,ptr->d_name);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i++;
        }
        free(pnew);
        closedir(dir);
        
    }
     else//普通文件
    {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,dirr);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i=2;
    }
   	fl f,y,x,p,t,q;
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
    t=phead->next;
    int j=0;
    while(t)
    {
        display_single(t->filename);
        j++;
        if((j+1)%4==0)
        {
             printf("\n");
        }

        t=t->next;
    }
    printf("\n");
    
}

void analy_dirno(char *dirr)
{
    DIR *dir;
    struct dirent *ptr;
    typedef struct flnm{
        char filename[100];
        struct flnm *next;
    }*fl,FL;
    FL *phead,*pend,*pnew;
    phead=(fl)malloc(sizeof(FL));
    phead->next=NULL;
    pend=phead;
    int i=0;
    struct stat buf;
    
    stat(dirr,&buf);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
        
             if(dirr[strlen(dirr)-1]!='/')
                {
                    dirr[strlen(dirr)]='/';
                    dirr[strlen(dirr)+1]='\0';
                }

       
        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,ptr->d_name);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i++;
        }
        free(pnew);
        closedir(dir);
        
    }
     else//普通文件
    {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,dirr);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i=2;
    }
   	fl f,y,x,p,t,q;
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
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
           display_single(t->filename);
        }
        t=t->next; 
    }
}
 


void analy_dirR(char *dirr)
{
    DIR *dir;
    struct dirent *ptr;
    char filename[20][256];
    int i=0,j=0;
    struct stat buf;
    stat(dirr,&buf);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
            printf("%s:\n",dirr);
                  
             if(dirr[strlen(dirr)-1]!='/')
                {
                    dirr[strlen(dirr)]='/';
                    dirr[strlen(dirr)+1]='\0';
                }

      
        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {   
            j++;
            if(ptr->d_name[0]!='.')
            {
                display_single(ptr->d_name);
            }
            if((j+1)%4==0)
            {
                printf("\n");
            };
        }
        closedir(dir);
        printf("\n");

        int len;
        len=strlen(dirr);
        dir= opendir(dirr);
        i=0;
        while((ptr=readdir(dir))!=NULL)
        {
            strncpy(filename[i],dirr,len);
            filename[i][len]='\0';
            strcat(filename[i],ptr->d_name);
            filename[i][len+strlen(ptr->d_name)]='\0';
            
            i++;
        }

        closedir(dir);
        for(j=0;j<i;j++)
        {
            if(filename[j][len]!='.')
            {
                analy_dirR(filename[j]);

            }

        }
    }
    else{
        return ;
    }
}

void analy_dirRL(char *dirr)
{
    DIR *dir;
    struct dirent *ptr;
    typedef struct flnm{
        char filename[100];
        struct flnm *next;
    }*fl,FL;
    FL *phead,*pend,*pnew;
    phead=(fl)malloc(sizeof(FL));
    phead->next=NULL;
    pend=phead;
    int i=0;
    struct stat buf;
    
    stat(dirr,&buf);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
        
             if(dirr[strlen(dirr)-1]!='/')
                {
                    dirr[strlen(dirr)]='/';
                    dirr[strlen(dirr)+1]='\0';
                }

       
        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,ptr->d_name);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i++;
        }
        free(pnew);
        closedir(dir);
        
    }
     else//普通文件
    {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,dirr);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i=2;
    }
   	fl f,y,x,p,t,q;
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
    t=phead->next;
    while(t)
    {
           display_attribute(t->filename);
            printf("   %-s",t->filename);
            printf("\n");
           t=t->next;
    }
}
 
/*
        mZnt len;
        len=strlen(dirr);
        dir= opendir(dirr);
        i=0;
        while((ptr=readdir(dir))!=NULL)
        {
            strncpy(filename[i],dirr,len);
            filename[i][len]='\0';
            strcat(filename[i],ptr->d_name);
            filename[i][len+strlen(ptr->d_name)]='\0';
            
            i++;
        }

        closedir(dir);
        for(j=0;j<i;j++)
        {
            if(filename[j][len]!='.')
            {
                analy_dirRL(filename[j]);

            }

        }
    }
    else{
        return ;
    }
}
*/
void analy_dirAL(char *dirr)
{    
    DIR *dir;
    struct dirent *ptr;
    typedef struct flnm{
        char filename[100];
        struct flnm *next;
    }*fl,FL;
    FL *phead,*pend,*pnew;
    phead=(fl)malloc(sizeof(FL));
    phead->next=NULL;
    pend=phead;
    int i=0;
    struct stat buf;
    
    stat(dirr,&buf);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
        
             if(dirr[strlen(dirr)-1]!='/')
                {
                    dirr[strlen(dirr)]='/';
                    dirr[strlen(dirr)+1]='\0';
                }

       
        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,ptr->d_name);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i++;
        }
        free(pnew);
        closedir(dir);
        
    }
     else//普通文件
    {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,dirr);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i=2;
    }
   	fl f,y,x,p,t,q;
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
    t=phead->next;
    while(t)
    {
           display_attribute(t->filename);
            printf("   %-s",t->filename);
            printf("\n");
           t=t->next;
    }
}

void analy_dirl(char *dirr)
{
    DIR *dir;
    struct dirent *ptr;
    typedef struct flnm{
        char filename[100];
        struct flnm *next;
    }*fl,FL;
    FL *phead,*pend,*pnew;
    phead=(fl)malloc(sizeof(FL));
    phead->next=NULL;
    pend=phead;
    int i=0;
    struct stat buf;
    
    stat(dirr,&buf);
    if(S_ISDIR(buf.st_mode))//目录文件
    {
        
             if(dirr[strlen(dirr)-1]!='/')
                {
                    dirr[strlen(dirr)]='/';
                    dirr[strlen(dirr)+1]='\0';
                }

       
        dir=opendir(dirr);
        while((ptr=readdir(dir))!=NULL)
        {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,ptr->d_name);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i++;
        }
        free(pnew);
        closedir(dir);
        
    }
     else//普通文件
    {
            pnew=(fl)malloc(sizeof(FL));
            strcpy(pnew->filename,dirr);
            pnew->next=NULL;
            pend->next=pnew;
            pend=pnew;
            i=2;
    }
   	fl f,y,x,p,t,q;
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
    t=phead->next;
    while(t)
    {
        if(t->filename[0]!='.')
        {
           display_attribute(t->filename);
            printf("   %-s",t->filename);
            printf("\n");
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
    int i,len;
    
   
    len=sizeof(name);
    printf("%-20s",name);

  
}

