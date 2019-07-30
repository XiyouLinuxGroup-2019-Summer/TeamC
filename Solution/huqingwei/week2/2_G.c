#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct room{
    double java;
    double cat;
    double danjia;
};

int main()
{
    int m = 0, n = 0;
    struct room room[10000];

    while(1){
        scanf("%d %d", &m, &n);
        if(m == -1 && n == -1){
            break;
        }

        int i, j;
        
        for(i=0; i<n; i++){
            scanf("%lf %lf", &room[i].java, &room[i].cat);
            room[i].danjia = room[i].java/room[i].cat;
        }

        for(i=0; i<n-1; i++){
            //printf("a\n");
            for(j=0; j<n-i-1; j++){
                if(room[j].danjia < room[j+1].danjia){
                    struct room temp = room[j+1];
                    room[j+1] = room[j];
                    room[j] = temp;
                }
            }
        }
        
        double sum = 0;
        i=0;
        while(m > 0 && i < n){
            //printf("b\n");
            if(m > room[i].cat){
                sum += room[i].java;
                m -= room[i].cat;
            }
            else{
                sum += room[i].danjia*m;
                m -= room[i].danjia*m;
            }
            i++;
        }
        printf("%.3lf\n", sum);
    }

    return 0;
}
