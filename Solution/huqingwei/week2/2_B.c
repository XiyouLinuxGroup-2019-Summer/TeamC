#include <stdio.h>
#include <stdlib.h>

typedef long long ll;

int main()
{
    ll n;
    int i, tmp = 1;

    scanf("%lld", &n);
    
    i = 1;
    //int quanzhi = 0;
    //int temp = 1;
    for(i=1; i<=n/2; i++){
        tmp++;
    }
        
    printf("%d\n", tmp);

    return 0;
}

