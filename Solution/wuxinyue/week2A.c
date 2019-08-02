#include <stdio.h>
int main()
{
    int m,n;
    double a,b;
    double area,min=99999999;
    scanf("%d %d",&n,&m);
    while(n--)
    {
        scanf("%lf %lf",&a,&b);
        area=(a/b)*m;
        if(area<min)
            min=area;
    }
    printf("%.8lf",min);
    return 0;
}
