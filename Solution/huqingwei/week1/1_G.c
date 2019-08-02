#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int wancheng(int n, int k, int v);

int main()
{
    int n, k;
    //int v = 1;
    scanf("%d %d", &n, &k);
    
    int head = 1;
    int tail = n;
    
    while(head <= tail){
        int mid = (head + tail) / 2;
        if(wancheng(n, k, mid)){
            tail = mid - 1;
        }
        else{
            head = mid + 1;
        }
    }
    //printf("%d\n", wancheng(n, k, v));
    printf("%d\n", head);

    return 0;
}

int wancheng(int n, int k, int v){
    int worked = 0;
    
    worked = v;

    //printf("%d\n", v);
    while(v){
        worked += v/k;
        v /= k;
    }

    if(worked >= n)
        return 1;
    return 0;
}

