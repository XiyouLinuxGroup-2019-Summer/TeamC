#include<stdio.h>
#include<stdlib.h>

int comp(const void*a,const void*b)//用来做比较的函数。
{
    return *(int*)a-*(int*)b;
}
int main()
{
    int n,m,sum=0,i,b[1005],a[1005],j=0,count=0;
    scanf ("%d %d",&n,&m);
    for ( i=0; i<n; i++ )
    {
        scanf ("%d",&a[i]);
        if ( a[i]<0 )
        {
            b[j]=a[i]*(-1);
            j++;
            count++;
        }
    }
    qsort(b,j,sizeof(int),comp);//调用qsort排序
    if ( m > count)
    {
        for ( i=0; i<j;i++ )
            sum+=b[i];
    }
    else 
    {
        for ( i=j-1; i>=j-m; i-- )
            sum+=b[i];
    }
    printf("%d\n",sum);

}
