#include <stdio.h>
#include<memory.h>
int main()
{
    int n;
    int i;
    int a[200010];
    char b[200010];
    while(~(scanf("%d",&n)))
    {
        memset(&a,0,sizeof(a));
        memset(b,0,sizeof(b));
        int sum = 0;
        for(i = 0; i < n; i++)
            scanf("%d",&a[i]);
        
        int k = n - 1;
        int j = 0;
        int m = 0;  
        for(i = 0; i <= k; i++)
        {
            if((a[i] <= a[k] && a[i] > m) || (a[i] > a[k] && a[k] < m && a[i] >= m)) 
            {
                sum++;
                b[j] = 'L';
                j++;
                //strcat(c,"L");
                m = a[i];
            }
            else if((a[i] > a[k] && a[k] > m) || (a[i] < a[k] && a[i] < m && a[k] > m))
            {
                sum++;
                b[j] = 'R';
                j++;
                //strcat(c,"R");
                m = a[k];
                k--;
                i--;
            }
            else
                break;
        }
        printf("%d\n%s\n",sum,b);
    }
    return 0;
}
