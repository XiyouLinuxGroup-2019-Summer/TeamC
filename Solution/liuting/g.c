/*************************************************************************
	> File Name: g.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月27日 星期六 08时40分52秒
 ********************************************************k***************/

#include<stdio.h>
#include <math.h>

int main()
{
    int i,n,k,v,t,sum;
    scanf("%d %d",&n,&k);
    t=k;
    for(i=2;;i++)
    {
        k=k*k;
        v=k;
        sum=(1-t*k)/(1-t);
        if(sum>=n)
        {
            printf("%d\n",v);
            break;
        }
    }

}
