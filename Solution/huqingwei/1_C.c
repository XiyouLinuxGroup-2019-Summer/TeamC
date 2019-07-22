#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int *problem;
    int n;
    int i, j;

    scanf("%d", &n);

    problem = (int*)malloc(sizeof(int)*n);

    for(i=0; i<n; i++){
        scanf("%d", problem+i);
    }
    
    //对题组进行排序，从小到大
    for(i=0; i<n-1; i++){
        for(j=0; j<n-i-1; j++){
            if(*(problem+j) > *(problem+j+1)){
                int temp = *(problem+j);
                *(problem+j) = *(problem+j+1);
                *(problem+j+1) = temp;
            }
        }
    }

    int days = 1;
    int tmp = 0;

    for(i=1; i<=n; i++){
        if(*(problem+i-1) < days){
            continue;
        }
        else{
            days++;
            tmp++;
        }
    }

    printf("%d\n", tmp);

    return 0;
}
