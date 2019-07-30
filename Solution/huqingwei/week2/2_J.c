#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct jiemu{
    int start;
    int end;
};

int main()
{
    int n;
    int i, j, num, end;
    struct jiemu jiemu[1000];

    while(scanf("%d", &n) && n != 0){
        for(i=0; i<n; i++){
            scanf("%d %d", &jiemu[i].start, &jiemu[i].end);
        }
        
        //排序
        for(i=0; i<n-1; i++){
            for(j=0; j<n-i-1; j++){
                if(jiemu[j].end > jiemu[j+1].end){
                    struct jiemu temp = jiemu[j+1];
                    jiemu[j+1] = jiemu[j];
                    jiemu[j] = temp;
                }
            }
        }

        num = 1;
        end = jiemu[0].end;
        for(i=1; i<n; i++){
            if(jiemu[i].start >= end){
                num++;
                end = jiemu[i].end;
            }
        }

        printf("%d\n", num);
    }

    return 0;
}
