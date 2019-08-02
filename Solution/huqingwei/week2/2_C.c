#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int i, j;
    int num, days;
    char *str;
    int tmp = 0;
    int sum = 0;

    str = (char*)malloc(sizeof(char)*1000);

    scanf("%d %d", &num, &days);

    for(i=0; i<days; i++){
        scanf("%s", str);
        int flag = 0;
        for(j=0; j<num; j++){
            if(str[j] == '0'){
                flag = 1;
            }
        }
        if(flag == 1){
            tmp++;
        }
        else{
            tmp = 0;
        }
        if(tmp > sum){
            sum = tmp;
        }
        
    }
    printf("%d\n", sum);
}
