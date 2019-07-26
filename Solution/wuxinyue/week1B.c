#include <stdio.h>
#include<memory.h>
int main()
{
    int k;
    scanf("%d",&k);
    int n,flag,temp,b,count,i;
    int a[1000];
    while(k--)
    {
        b=0;
        count=0;
        scanf("%d",&n);
        memset(a,0,sizeof(a));
        for(i=0;i<n;i++)
        {
            scanf("%d",&flag);
            a[flag%3]++;
        }
         if(a[1]>a[2])
        {
            temp=a[1]-a[2];
            //if(temp%3==0)            
            {
                b=temp/3;
            }
            count=a[0]+a[2]+b;
        }
        if(a[2]>a[1])
        {
            temp=a[2]-a[1];
            /*if((temp*2)%3==0)
            {
                b=(temp*2)/3;
            }*/
            b = temp/3;
            count=a[0]+a[1]+b;
        }
        if(a[2]==a[1])
        {
            count=a[2]+a[0];
        }
        printf("%d\n",count);
    }
}
