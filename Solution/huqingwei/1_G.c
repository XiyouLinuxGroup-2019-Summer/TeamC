#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int wancheng(int n, int k, int v);
int qiumi(int x, int y);

int main()
{
    int n, k;
    int v = 1;
    scanf("%d %d", &n, &k);
    
    while(!wancheng(n, k, v)){
        v++;
    }

    printf("%d\n", v);

    return 0;
}

int qiumi(int x, int y){
    int sum = 1;
    while(y--){
        sum = x*x;
    }
    return sum;
}

int wancheng(int n, int k, int v){
    int worked = v;
    int worked_once;
    int i = 1;
    while(worked >= n){
        printf("qiumi = %d", qiumi(k, i));
        worked_once = v/qiumi(k, i);
        i++;
        worked += worked_once;
    }

    return 0;
}

