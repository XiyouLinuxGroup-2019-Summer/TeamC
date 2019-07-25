#include<stdio.h>
int main()
{
    int i;
    int m;
    long long int n;
    scanf("%d",&m);
    while(m--)
    {
        i=0;
        scanf("%lld",&n);
        if(n<1)
           {
               printf("-1\n");
               continue;
           }
        if(n==1)
           {
               printf("0\n");
               continue;
           }
        while(n!=1 && n>1)
        {   
            if(n%5==0)                                                                                                  
           {   
               n=4*n/5;
               i++;
               continue;
           }
            if(n%3==0)
           {
               n=2*n/3;
               i++;
               continue;
           }
           if(n%2==0)
           {   
              n=n/2;
              i++;
           }   
           else 
           {
               printf("-1\n");
               break;
           }
        }
        if(n==1)
        {
            printf("%d\n",i);
        }
    }
    return 0;
}
