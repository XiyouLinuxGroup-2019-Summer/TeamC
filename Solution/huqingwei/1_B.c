#include <stdio.h>
#include <stdlib.h>

int min(int a, int b);
int max(int a, int b);

int main()
{
    int t;
    int i;
    int n;
    long long *a;

    scanf("%d", &t);
    while(t--){
        scanf("%d", &n);
        a = (long long*)malloc(sizeof(long long)*n);
        
        for(i=0; i<n; i++){
            scanf("%lld", a+i);
        }

        int yu_1 = 0;
        int yu_2 = 0;
        int num = 0;

        for(i=0; i<n; i++){
            if(*(a+i)%3 == 0){
                num++;
            }
            else if(*(a+i)%3 == 1){
                yu_1++;
            }
            else if(*(a+i)%3 == 2){
                yu_2++;
            }
        }

        num += min(yu_1, yu_2);
        num += (yu_1-min(yu_1, yu_2))/3+(yu_2-min(yu_1, yu_2))/3;

        printf("%d\n", num);

        free(a);
    }

    return 0;
}

int min(int a, int b){
    if(a > b){
        return b;
    }
    else{
        return a;
    }
}

int max(int a, int b){
    if(a > b){
        return a;
    }
    else{
        return b;
    }
}
