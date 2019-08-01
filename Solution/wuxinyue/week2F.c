#include<stdio.h>
int main()
{
    int k,i,t;
    scanf("%d",&k);
    if ( k>36 )
    {
        printf("-1\n");
        return 0;
    }
    if ( k%2 ==0 )
    {
        t = k/2;
        for (i=0; i<t; i++)
            printf("8");
        printf("\n");
    }
    else 
    {
        t = k/2;
        for (i=0; i<t; i++)
            printf("8");
        printf("6\n");
    }
}
