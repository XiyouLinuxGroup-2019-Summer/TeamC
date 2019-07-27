#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int  main()
{
    int n;
    int weishu = 0;
    scanf("%d", &n);

    while(n != 0){
        weishu++;
        n /= 2;
    }

    printf("%d\n", weishu);

    return 0;
}
