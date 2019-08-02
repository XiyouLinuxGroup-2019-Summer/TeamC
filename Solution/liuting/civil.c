/*************************************************************************
	> File Name: civil.c
	> Author: 
	> Mail: 
	> Created Time: 2019年07月24日 星期三 09时01分04秒
 ************************************************************************/

#include<stdio.h>
#include <string.h>
int main()
{
    char Haab[19][10]={"pop","no","zip","zotz","tzec","xul","yoxkin","mol","chen","yax","zac","ceh","mac","kankin","muan","pax","koyab","cumhu","uayet"};
   
    char Tzolkin[20][10]={"imix", "ik", "akbal","kan", "chicchan", "cimi", "manik", "lamat", "muluk", "ok", "chuen", "eb", "ben", "ix", "mem", "cib", "caban", "eznab", "canac", "ahau"};


    int n;
    scanf("%d",&n);
    while(n--)
    {
        int Haab_day,Haab_year,sum, i,month;
        int Tzolkin_year,Tzolkin_month,Tzolkin_day;
        char Haab_month[10];
        scanf("%d. %s %d",&Haab_day,Haab_month,&Haab_year);
        
        for(i=0;i<19;i++)
        {
            if(strcmp(Haab_month,Haab[i])==0)
            {
                month=i;
                break;
            }
        }

        sum=Haab_year*365+month*20+Haab_day;

        Tzolkin_year=sum/260;
        Tzolkin_month=(sum%260)%13+1;
        Tzolkin_day=sum%260%20;
        printf("%d %s %d\n",Tzolkin_month,Tzolkin[Tzolkin_day],Tzolkin_year);
    }
}
