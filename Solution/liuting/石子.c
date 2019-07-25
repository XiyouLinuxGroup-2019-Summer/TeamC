/*************************************************************************
	> File Name: 石子.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月23日 星期二 21时29分35秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    int C,i;
    scanf("%d",&C);
    for(i=0;i<C;i++)
    {
        int n,m;
    scanf("%d %d",&n,&m);
    
    if(0<n%(m+1)&&n%(m+1)<=m)
    {
        printf("first\n");

    }
    else{
        printf("second\n");
    }
   
    }
}
