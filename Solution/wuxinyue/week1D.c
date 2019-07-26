#include<stdio.h>
#include<string.h>
char a[10000],s[16];  
int main()
{
    int temp,i;
    while(gets(s))
    {
        if(strcmp(s,"ENDOFINPUT")==0)
            break;
        if(strcmp(s,"START")==0)
        {
              gets(a);
            temp=strlen(a);
            for(i=0;i<temp;i++)
            {
                if(a[i]>='A' && a[i]<'F')
                {
                    a[i]=86+(a[i]-'A');
                }
                else if(a[i]>='F' && a[i]<='Z')
                {
                    a[i]=a[i]-5;
                }
            }
            printf("%s\n",a);
        }
    }
    return 0;
}
