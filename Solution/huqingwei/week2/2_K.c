#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct xiaodao{
    int x;
    int y;
};

int main()
{
    struct xiaodao xiaodao[100];
    int i, j, n, d;
    int case_num = 1;

    //scanf("%d %d", &n, &d);

    while(scanf("%d %d", &n, &d) && n!=0 && d!=0){
        
        for(i=0; i<n; i++){
            scanf("%d %d", &xiaodao[i].x, &xiaodao[i].y);
        }

        
        int flag = 0;
        for(i=0; i<n; i++){
            if(xiaodao[i].y > d){
                flag = 1;
                printf("Case %d: -1", case_num);
                case_num++;
                break;
            }
        }
        if(flag == 1){
            continue;
        }
        //sort
        for(i=0; i<n-1; i++){
            for(j=0; j<n-i-1; j++){
                if(xiaodao[j].y < xiaodao[j+1].y){
                    struct xiaodao temp = xiaodao[j+1];
                    xiaodao[j+1] = xiaodao[j];
                    xiaodao[j] = temp;
                }
            }
        }

        int tmp = 1;
        int Ox = xiaodao[0].x;
        for(i=1; i<n; i++){
            if((xiaodao[i].x - Ox) > d || (Ox - xiaodao[i].x) > d){
                tmp++;
            }
        }

        printf("Case %d: %d\n", case_num, tmp);
        case_num++;
    }
}
