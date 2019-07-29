#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct MARKET{
    double a; //a元能买b公斤苹果
    double b;
    double dan_jia;
};

int main()
{
    struct MARKET market[10000];
    int n, m;
    int i, j;

    scanf("%d %d", &n, &m);
    for(i=0; i<n; i++){
        scanf("%lf %lf", &market[i].a, &market[i].b);
        market[i].dan_jia = market[i].a/market[i].b;
    }
   
    for(i=0; i<n-1; i++){
        for(j=0; j<n-i-1; j++){
            if(market[j].dan_jia > market[j+1].dan_jia){
                struct MARKET temp = market[j];
                market[j] = market[j+1];
                market[j+1] = temp;
            }
        }
    }
    
    printf("%.8f\n", m*market[0].dan_jia);

    return 0;
}

