#include<stdio.h>

int main()
{
    int n,m,i,temp,j;
    double flag;
    double sum=0,a[1001],a1[1001],a2[1001];
    while ( 1 )
    {
        sum=0;
        scanf ("%d %d",&m,&n);
        if ( n == -1 && m == -1 )
            break;
        for ( i=0; i<n; i++ )
        {
            scanf ("%lf %lf",&a1[i],&a2[i]);
            a[i]=a1[i]/a2[i];
        }
        for ( i=0; i<n-1; i++ )
        {
            for ( j=0; j<=n-1-i; j++ )
            {
                if ( a[j]<a[j+1] )
                {
                    temp=a[j];
                    a[j]=a[j+1];
                    a[j+1]=temp;
                    temp=a1[j];    
                    a1[j]=a1[j+1];  
                    a1[j+1]=temp;
                    temp=a2[j];    
                    a2[j]=a2[j+1];  
                    a2[j+1]=temp;  
                }
            }
        }
        for ( i=0; i<n; i++ )    
        {                      
            if ( m >= a2[i] )   
            {                  
                 sum += a1[i];
                 m = m-a2[i];  
            }                  
            else               
            {                  
                flag = m/a2[i];
                sum += flag * a1[i];
                break; 
            }
        }
        printf("%.3lf\n",sum);
    }
    return 0;
} 
