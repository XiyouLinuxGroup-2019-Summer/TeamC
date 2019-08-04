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
        for ( i=0; i<n; i++ )
            scanf ( "%d", &day[i] );
        for ( i=0; i<n; i++ )
            scanf ( "%d", &score[i] );
        for ( i=0; i<1; i++ )
        {
            for ( j=0; j<n; j++ )
            {
                flag[j][i] = day[j];
                flag[j][i+1] = score[j];
            }
        }
        qsort( score, n, sizeof(int), comp );//调用qsort排序
        for ( i=0; i<n; i++ )
        {
            for ( j=0; j<n; j++ )
            {
                if ( score[i] == flag[0][j] /* && flag[j][0] != max */ )
                {
                    if ( flag[j][0] != max )
                    {    
                         max = flag[j][0];
                         printf("%d\n", max);
                         break;
                    }
                    else 
                    {
                         while ( flag[j][0] != 0 )
                         {
                             flag[j][0]--;
                             if (flag[j][0] != max)
                             {
                                 max = flag[j][0];
                                 printf("%d\n", max);
                                 break;
                             }
                    }
                }
            }
        }
    }
}
