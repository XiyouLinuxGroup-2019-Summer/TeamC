/*************************************************************************
	> File Name: 11.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月29日 星期一 16时57分57秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    int n,i;
    double m,min;
    scanf("%d %lf",&n,&m);
    double a[n],b[n];

    for(i=0;i<n;i++)
    {
        scanf("%lf %lf",&a[i],&b[i]);
        a[i]=a[i]/b[i];
    }
    min=a[0];
    for(i=1;i<n;i++)
    {
        min=min<a[i]?min:a[i];
    }
    printf("%.8lf\n",min*m);
}
