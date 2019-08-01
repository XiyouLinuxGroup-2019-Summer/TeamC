#include<stdio.h>

int comp(const void*a,const void*b)//用来做比较的函数。
{
    return *(int*)b-*(int*)a;
}

int main()
{
    scanf ("%d",&t);
    while ( t-- )
    {
        scanf ( "%d", &n );
        for ( i=1; i<=n; i++ )
            scanf ( "%d", &day[i] );
        for ( i=1; i<=n; i++ )
            scanf ( "%d", &score[i] );
        qsort( score, n, sizeof(int), comp );//调用qsort排序
cm
    }
}
