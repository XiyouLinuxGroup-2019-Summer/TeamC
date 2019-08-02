#include <stdio.h>
#include <string.h>
int main ()
{
    int n, i, day, year, sum;
    char month[10];
    char *s1[19 = {"pop", "no", "zip", "zotz", "tzec", "xul", "yoxkin", "mol", "chen", "yax", "zac", "ceh", "mac", "kankin", "muan", "pax", "koyab", "cumhu", "uayet"};
    char *s2[20] = {"imix", "ik", "akbal", "kan", "chicchan", "cimi", "manik", "lamat", "muluk", "ok", "chuen", "eb", "ben", "ix", "mem", "cib", "caban", "eznab", "canac", "ahau"};   
    scanf("%d", &n);
    printf("%d\n", n); 
    while ( n-- ) {
        scanf("%d. %s %d", &day, month, &year);
        for ( i = 0; i < 19; i++ ) {
            if ( strcmp(*(s1+i),month) == 0 )
                break;
        }
        sum = year*365+i*20+day+1;
        year = sum/260;
        day = sum%13;
        if ( day == 0 )
            day = 13;
        i = sum%20;
        if ( i == 0 )
            i = 20;
        if ( sum%260 == 0 ) {
            printf("%d %s %d\n", day, *(s2+i-1), year-1);
            continue;
        }
        printf("%d %s %d\n", day, *(s2+i-1), year);
    }

    return 0;
}
