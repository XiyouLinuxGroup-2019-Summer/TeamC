/*************************************************************************
	> File Name: 17.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月31日 星期三 11时02分31秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>

 struct val 
        {
            double j;
            double f;
            double arg;
        }value[100];

int com(const void *a,const void *b)
{
    return ( ( *(struct val*)a).arg>(*(struct val *)b).arg ?-1:1);
}

int main()
{
    int M,N,i;
    double t,t1,sum=0,sum1=0;

    while(scanf("%d %d",&M,&N)==2)
    {
        struct val 
        {
            double j;
            double f;
            double arg;
        }value[N];
        if(M!=-1 && N!=-1)
        {
            for(i=0;i<N;i++)
            {
                scanf("%lf %lf",&value[i].j,&value[i].f);
                value[i].arg=value[i].j/value[i].f;
            }
            
            qsort(value,N,sizeof(struct val),com);

            for(i=0;i<N;i++)
            {
                t=sum;
                t1=sum1;
                sum+=value[i].j;
                sum1+=value[i].f;
                if(sum1>=M)
                {
                    t=t+(value[i].arg * (M-t1));
                    break;
                }
            }
            printf("%0.3lf\n",t);
            t=0;
            sum=0;
            sum1=0;
            t1=0;
        }
        else
        {
            return 0;
            
        }

    }
}
