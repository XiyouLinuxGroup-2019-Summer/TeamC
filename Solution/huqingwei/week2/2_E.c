#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int n = 0, m = 0;
    int ai[10000];
    int i, j;

    scanf("%d %d", &n, &m);

    for(i=0; i<n; i++){
        scanf("%d", &ai[i]);
    }

    for(i=0; i<n-1; i++){
        for(j=0; j<n-i-1; j++){
            if(ai[j] > ai[j+1]){
                int temp = ai[j+1];
                ai[j+1] = ai[j];
                ai[j] = temp;
            }
        }
    }

    int sum = 0;
    for(i=0; i<m; i++){
        if(ai[i] < 0){
            sum += ai[i];
        }
    }

    printf("%d\n", -sum);


    return 0;
}
