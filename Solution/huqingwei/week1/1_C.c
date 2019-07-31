#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long ll;

int cmp(const void *a, const void *b){
    return *(int*)a - *(int*)b;
}

int main()
{
    ll *problem;
    ll n;
    ll i, j;

    scanf("%lld", &n);

    problem = (ll*)malloc(sizeof(ll)*n);

    for(i=0; i<n; i++){
        scanf("%lld", problem+i);
    }
    
    //对题组进行排序，从小到大
    qsort(problem, n, sizeof(ll), cmp);

    ll days = 1;
    ll tmp = 0;

    for(i=0; i<n; i++){
        if(*(problem+i) >= days){
            tmp++;
            days++;
        }
    }

    printf("%lld\n", tmp);

    return 0;
}

