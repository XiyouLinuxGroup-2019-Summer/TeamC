/*************************************************************************
	> File Name: 13.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月29日 星期一 19时35分08秒
 ************************************************************************/

#include<stdio.h>
#include <math.h>
int main()
{
    int n,d; //n为敌人数量，d为天数
    scanf("%d %d",&n,&d);
    char a[d][n];
    int i,j,flag=0,max=0,t;
    for(i=0;i<d;i++)
    {
        scanf("%s",a[i]);
    }
    for(i=0;i<d;i++)
    {
        t=flag;
        for(j=0;j<n;j++)
        {
            if(a[i][j]=='0')
            {
                flag++;
                max=max>flag?max:flag;
                break;
            }
        }
        if(t==flag)
        {
            flag=0;
        }
        
    }
    printf("%d\n",max);

}
