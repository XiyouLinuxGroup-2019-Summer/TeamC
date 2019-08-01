/*************************************************************************
	> File Name: 15.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月30日 星期二 20时58分37秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>

int com(const void *a,const void *b)
{
    return (*(int *)a -*(int *)b);
}
int main()
{
    int n,m,i,sum=0;
    scanf("%d %d",&n,&m);
    long long  a[n];
    for(i=0;i<n;i++)
    {
        scanf("%lld",&a[i]);
    }

    qsort(a,n,sizeof(long long),com);


    for(i=0;i<m;i++)
    {
        if(a[i]<0)
        {
            sum+=a[i];
        }
    }
    printf("%d\n",-1*sum);
}
