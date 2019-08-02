/*************************************************************************
	> File Name: io.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月27日 星期六 09时53分29秒
 ************************************************************************/
#include<stdio.h>
#include <string.h>

int main()
{
    char t1[10];
    while(gets(t1))
    {
        int len,i=0;
        char str[200];
        if(strcmp(t1,"ENDOFINPUT")==0)
        {
            break;
        }
        if(strcmp(t1,"START")==0)
        {
            gets(str);
            len=strlen(str);
            while(len--)
            {
                if(str[i]>='F'&&str[i]<='Z')
                {
                    printf("%c",str[i]-5);
                }
                else if(str[i]>='A'&&str[i]<='E')
                {
                    printf("%c",str[i]+21);
                }
                else
                {
                    printf("%c",str[i]);
                }
                i++;
    
            }
            printf("\n");
        }
    }
}
