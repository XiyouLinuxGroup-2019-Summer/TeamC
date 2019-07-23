/*************************************************************************
	> File Name: hehanshu.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月23日 星期二 14时06分18秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    int t,n,i,j;
    int x=0;
    int a[100];
    scanf("%d",&t);
    for(i=0;i<t;i++)
    {
        scanf("%d",&n);
        for(i=0;i<n;i++)
        {
            scanf("%d",&a[i]);
        }
        for(i=0;i<n;i++)
        {

            if(a[i]%3==0)
            {
                x++;
                a[i]=0;
            }
        }
        for(i=0;i<n/2;i++)
        {
                for(j=n/2+1;j<n;j++)
                {
                
                    if((a[i]+a[j])%3==0)
                    {
                        x++;
                        a[i]=0;
                        a[j]=0;
                    }
                }
        }
        for(i=0;i<n;i++)
        {
            printf("%d ",a[i]);
        }
        printf("%d\n",x);
    }
    
}
