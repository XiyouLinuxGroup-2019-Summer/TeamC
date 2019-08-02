/*************************************************************************
	> File Name: 1j.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月31日 星期三 19时47分10秒

mZ#include<stdio.h>
#include <stdlib.h>

struct Time
{
    int  start_time;
    int  end_time;
}time[100];

int cmp(const void *a,const void *b)
{
    return ((*( struct Time*)a).end_time > (*(struct Time *)b).end_time? 1:-1);
}

int main()
{
    int n;
    int i,j,sum=1;
    while(scanf("%d",&n)==1)
    {
        if(n==0)
        {
            return 0;
        }
        else
        {
            for (i = 0 ; i < n ; i++)
            {
                scanf("%d %d",&time[i].start_time,&time[i].end_time);
            }
            qsort(time,n,sizeof(struct Time),cmp);
            
                i=0;
                for ( j =1 ; j<n ; j++ )
                {
                    if(time[j].start_time >=time[i].end_time)
                    {
                        i=j;
                        sum++;
                    }
                }

            

        }
        printf("%d\n",sum);
        sum=1;
    }
}
