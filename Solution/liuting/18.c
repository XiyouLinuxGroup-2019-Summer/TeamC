/*************************************************************************
	> File Name: 18.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月29日 星期一 19时01分32秒
 ************************************************************************/
#include<stdio.h>
#include <stdlib.h>

struct p{
        int deadline;
        int score;
    }a[1000];

int com(const void * m,const void * n)
{
    return ( (*(struct p*)m).deadline > (*(struct p*)n).deadline?1:-1);
}

int main()
{
    int N;
    scanf("%d",&N);
    while(N--)
    {

        int T,i,sum=0,j,max=0,flag=0,flag2=0;
        scanf("%d",&T);

        for(i=0;i<T;i++)
        {
            scanf("%d",&a[i].deadline);
        }
        for(i=0;i<T;i++)
        {
            scanf("%d",&a[i].score);
        }

        qsort(a,T,sizeof(struct p),com);

        j=T-1;
        for(i=T;i>0;i--)
        {
            if(a[j].deadline>=i && j>=0)
            {
                if(max<a[j].score)
                {
                    max=a[j].score;
                    flag=j;
                }
                i=i+1;
                j--;
                flag2=1;
            }
            else if(flag2==1)
            {
                a[flag].score=0;  
                j=T-1;
                max=0;
            }
        }
        for(i=0;i<T;i++)
        {
                if(a[i].score!=0)
            {
                sum+=a[i].score;
            }
        }
        printf("%d\n",sum);
        sum=0;

    }
}
