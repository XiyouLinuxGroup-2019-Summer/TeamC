/*************************************************************************
	> File Name: 思维.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月26日 星期五 22时54分40秒
 ************************************************************************/

#include<stdio.h>
#include <math.h>
int main()
{
    int x;
    int sum=1,i;
    while(scanf("%d",&x)==1)
    {
        for(i=1;i<=x;i++)
        {
            sum=pow(2,i);       
            if(sum>x)
            {
                printf("%d\n",i);
                break;
            }
        }
        sum=1;
    }
}
