#include <stdio.h>

int main()
{
    int a,i, sum,b;
    scanf("%d", &b);
    sum = 0;
    for ( i = 1; i <= b; i++ ) {
         a=i;
         sum = 1;
         while ( a ) {
             sum*=2;
             a--;
         }
         sum--;
        if ( sum >= b )
            break;
    }

    printf("%d\n", i);
    return 0;
}
