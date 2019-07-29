#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main()
{
    int n;
    int *arry;

    scanf("%d", &n);
    arry = (int*)malloc(sizeof(int)*n);

    int i = 0;
    for(i=0; i<n; i++){
        scanf("%d", arry+i);
    }

    int l = 0;
    int r = n-1;
    int flag = 0;
    int tmp = 0;
    char *ans;

    ans = (char*)malloc(sizeof(char)*n);
    i = 0;
    int last_l, last_r;
    while(l < r){
        last_l=0;
        last_r=0;
        //printf("a\n");
        printf("l = %d, r = %d\n", arry[l], arry[r]);
        //sleep(1);
        if(arry[l]<arry[r]){
            if(arry[l] < flag){
                l++;
                continue;
            }
            ans[i++] = 'L';
            last_l = 1;
            tmp++;
            flag = arry[l];
            l++;
        }
        else if(arry[l]>arry[r]){
            if(arry[r] < flag){
                r--;
                continue;
            }
            ans[i++] = 'R';
            last_r = 1;
            tmp++;
            flag = arry[r];
            r--;
        }
    }
    if(arry[l] > flag){
        ans[i] = 'L';
        tmp++;
    }
    else if(last_l == 1 && arry[l] > flag){
        ans[i] = 'L';
        tmp++;
    }
    else if(last_r == 1 && arry[r] > flag){
        ans[i] = 'R';
        tmp++;
    }
    ans[i+1] = '\0';
    printf("%d\n", tmp);
    printf("%s\n", ans);

    return 0;
}
