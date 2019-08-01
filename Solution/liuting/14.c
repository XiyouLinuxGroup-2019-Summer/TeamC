/*************************************************************************
	> File Name: 14.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月30日 星期二 20时31分07秒
 ************************************************************************/

#include<stdio.h>

int main()
{
    int n,A,B,C,T,i,sum=0;
    scanf("%d %d %d %d %d",&n,&A,&B,&C,&T);
    int a[n];
    for(i=0;i<n;i++)
    {
        scanf("%d",&a[i]);
    }


    if(B>=C)
    {
        printf("%d\n",n*A);
        return 0;
    }
    else
    {
        for(i=0;i<n;i++)
        {
            if(a[i]<T)
            {
                sum+=(T-a[i])*C;
                sum-=(T-a[i])*B;
            }
        }
        sum+=(A*n);
        printf("%d\n",sum);
    }
    
    
}
