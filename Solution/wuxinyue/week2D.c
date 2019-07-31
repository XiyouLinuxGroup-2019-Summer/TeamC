#include <stdio.h>

int main()
{
    int ti[1000];
    int a,b,c,t,n,i,sum=0;
    scanf ("%d %d %d %d %d",&n,&a,&b,&c,&t);
    for ( i=0;i<n;i++ )
        scanf("%d",&ti[i]);
    if (b >= c)
         sum = a * n;
    else 
    {
        for ( i=0;i<n;i++ )
        {
            sum += (a-b*(t-ti[i])) + c*(t-ti[i]);
        }
    }
    printf("%d\n",sum);
    return 0;
}
