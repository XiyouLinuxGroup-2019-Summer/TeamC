#include <stdio.h>
#include <string.h>

int main()
{
    int n;
    int day, year; //Haab的年月日
    char mouth[10];
    char Haab_mouth[19][10] = {"pop", "no", "zip", "zotz", "tzec", "xul", "youkin", "mol", "chen", "yax", "zac", "ceh", "mac", "kankin", "muan", "pax", "koyab", "cumhu", "uayet"};

    int day1, year1; //Toz的年月日
    char mouth1[10];
    char Toz_mouth[52][10] = {"imix", "ik", "akbal", "kan", "chicchan", "cimi", "manik", "lamat", "muluk", "ok", "chuen", "eb", "ben", "ix", "mem", "cib", "caban", "eznab", "canac", "ahau",
                              "imix", "ik", "akbal", "kan", "chicchan", "cimi", "manik", "lamat", "muluk", "ok", "chuen", "eb", "ben", "ix", "mem", "cib", "caban", "eznab", "canac", "ahau",
                              "imix", "ik", "akbal", "kan", "chicchan", "cimi", "manik", "lamat", "muluk", "ok", "chuen", "eb"};

    scanf("%d", &n);

    while(n--){
        scanf("%d", &day);
        scanf("%s", mouth);
        scanf("%d", &year);
    
        int i = 0; //查找是第几个月
        while(strcmp(mouth, Haab_mouth[i]) != 0){
            i++;
        }
        printf("i = %d\n", i);
    
        i += 1;
        long long days = 0; //统计一共有多少天
        days = year * 365;
        while(i--){
            //printf("a\n");
            if(i == 19){
                days += 5;
            }
            else{
                days += 20;
            }
        }
        days += day;
        printf("days = %lld\n", days);

        //计算Toz纪年法日期
        int zhou_qi; //判断有多少个周期
        int yu_shu;  //余多少天
        zhou_qi = days / 52;
        yu_shu = days % 52;

        year1 = days / 52;  //Toz的年份
        day1 = yu_shu+1;  //Toz的天

        printf("yu_shu = %d\n", yu_shu);

        strcpy(mouth1, Toz_mouth[yu_shu]);

        printf("%d %s %d\n", day1, mouth1, year1);
    }
    return 0;
}

