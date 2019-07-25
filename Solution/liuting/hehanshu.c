/*************************************************************************
	> File Name: hehanshu.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月23日 星期二 14时06分18秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>


int main()
{
    int t,i,j;
    scanf("%d",&t);
    for(j=0;j<t;j++)
    {
        int n,a[100],j,s=0,sum1=0,sum2=0,su=0;
        scanf("%d",&n);
        for(i=0;i<n;i++)
        {
            scanf("%d",&a[i]);
        }

        for(i=0;i<n;i++)
        {
            if(a[i]%3==0)
            {
                s+=1;
            }
            else{

                a[i]=a[i]%3;
                if(a[i]==1)
                {
                    sum1+=a[i];
                }
                else{

                    sum2+=a[i]/2;
                }
            }

        }
        if(sum1>sum2)
        {
            if((sum1-sum2)>=3)
            {
                su=sum1-sum2;
                su=su/3;
            }
            sum1=sum2;
        }
        else
        {
            
            if((sum2-sum1)>=3)
            {
                su=sum2-sum1;
                su=su/3;
            }
        }
        printf("%d\n",sum1+s+su);
    }
}
