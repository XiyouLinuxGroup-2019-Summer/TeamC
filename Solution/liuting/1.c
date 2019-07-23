#include<stdio.h>
#include <math.h>
void fun(long long a);
int main()
{
    int q;
    int i;
    if(scanf("%d",&q)>1000)
    {
        return -1;
    }
    long long a[q];
    for(i=0;i<q;i++)
    {
        scanf("%lld",&a[i]);
        if( a[i]>=1 && a[i]<=pow(10,18))
        {
            fun(a[i]);
        }
    }
}
    void fun(long long a)
    {
        int i=0;
        while(a!=1&&i!=-1)
        {
            if(a%3==0)
            {
                a=(a/3)*2;
                i++;
                
            }
            else if(a%5==0)
            {
                a=(a/5)*4;
                i++;
            }
            else if(a%2==0)
            {
                a=a/2;
                i++;
            }
            else
            {
                i=-1;
            }
        }

        printf("%d\n",i);
        i=0;
    }
