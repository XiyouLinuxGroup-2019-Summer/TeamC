#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int n, a, b, c, t;
    int i;
    int ti[10000];

    scanf("%d %d %d %d %d", &n, &a, &b, &c, &t);

    for(i=0; i<n; i++){
        scanf("%d", &ti[i]);
    }

    int sum = a*n;

    if(b<c){
        for(i=0; i<n; i++){
            sum = sum + ((c-b)*t-(c-b)*ti[i]);
        }
    }

    printf("%d\n", sum);

    return 0;
}
