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
    for(i=0;;i++)
    {
        sum=(1-pow(k,i))/(1-k);
        if(sum>=n)
        {
            v=pow(k,(i-1));
            printf("%d\n",v);
            break;
        }
    }

}
