#include <stdio.h>

int ok(long long n){
    if(n%2==0 || n%3==0 || n%5==0)
        return 1;
    else
        return 0;
}

int main()
{
    int q;
    long long n;
    int step;

    scanf("%d", &q);

    while(q--){
        step = 0;
        scanf("%lld", &n);
        
        while(ok(n)){

            if(0 == n%2)
                n /= 2;
            else if(0 == n%3)
                n = n*2/3;
            else if(0 == n%5)
                n = n*4/5;
            
            step++;
        }

        if(1 == n)
            printf("%d\n", step);
        else
            printf("-1\n");
    }

    return 0;
}

