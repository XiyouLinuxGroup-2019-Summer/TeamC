/*************************************************************************
	> File Name: 16.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月30日 星期二 21时25分30秒
 ************************************************************************/

#include<stdio.h>

int main()
{
    long long k;
    long long i;
    scanf("%lld",&k);

    for(i=0;k!=0;i++)
    {
        if(k>=35)
        {
            printf("-1\n");
            return 0;
        }
        if(k%2==0)
        {
            k=k-2;
            printf("8");
        }
        else
        {
            k=k-1;
            printf("6");
        }

    }
    printf("\n");
}
