#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int k;
    
    scanf("%d", &k);

    if(k>36){
        printf("-1");
    }
    else if(k % 2 == 0){
        for(int i=0; i<k/2; i++){
            printf("8");
        }
    }
    else if(k % 2 != 0){
        for(int i=0; i<k/2; i++){
            printf("8");
        }
        printf("9");
    }

    printf("\n");

    return 0;
}
