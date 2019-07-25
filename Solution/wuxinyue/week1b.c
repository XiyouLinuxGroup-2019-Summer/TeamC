#include<stdio.h>
#include<string.h>
int main(){
    int temp,N,a[3],n,t;
    scanf("%d",&N);
    while(N--){
        int count = 0;
        memset(a,0,sizeof(a));
        scanf("%d",&n);
        for(int i = 0;i < n;i++){
            scanf("%d",&t);
            a[t%3]++;
        }
        if(a[2] < a[1]){temp = a[2];a[2] = a[1];a[1] = temp;}

        count = a[0] + a[1] + (a[2] - a[1])/3;
        printf("%d\n",count);
    }
    return 0;
}


