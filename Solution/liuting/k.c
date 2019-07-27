/*************************************************************************
	> File Name: k.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月26日 星期五 23时44分22秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>



int main()
{
    long long n,i,j=0;
    int flag1=0,flag2=0;
    scanf("%lld\n",&n);
    char a[n],b[n];
    scanf("%[^\n]",a);
    if(a==NULL)
    {
        printf("\n\n");
        return 0;
    }
    for(i=0;i<n;i++)
    {
        if(a[i]==a[i+1])
        {
            flag1++;
        }
        else
        {
            b[j++]=a[i++];
            b[j++]=a[i];

        }
        if(i==n-2)
        {
            if((n-flag1)%2==1)
            {
                flag2++;
                i++;
            }
        }
    }
    b[j]='\0';
    printf("%d\n",flag1+flag2);
    printf("%s\n",b);
}
