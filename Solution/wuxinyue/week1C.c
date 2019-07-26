#include<stdio.h>
#include<stdlib.h>
int comp(const void*a,const void*b)//用来做比较的函数。
{
    return *(int*)a-*(int*)b;
}
int main()
{
    int a[1000000];
    int i,t,n;
    int temp=1;
    int flag=0;
    scanf("%d",&n);
    for(i=0;i<n;i++)
        scanf("%d",&a[i]);
    qsort(a,n,sizeof(int),comp);//调用qsort排序
    for(i=0;i<n;i++)
    {
        if(a[i]>=temp)
        {
            temp++;
            flag++;
        }
    }
    printf("%d\n",flag);
}
