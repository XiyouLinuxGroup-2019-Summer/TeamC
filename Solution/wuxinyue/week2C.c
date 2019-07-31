#include <stdio.h>

int main()
{
    char  c[10000];
    int n, d;
    int  i, count, max, flag; 
    count = max = 0;
    scanf ("%d %d",&n,&d);
    while ( d-- )
    {
        scanf ("%s",c);
        flag = 1;
        for ( i=0;i<n;i++ )
        {
            if ( c[i] == '0' )
            {
                flag=0;
                break;
            }
        }
        if ( flag == 1 )
            count = 0;
        if ( flag == 0 )
            count++;
        if (count > max )
            max = count;
    }
    printf("%d\n",max);
}
