#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int i;
    char a[100];
    
    gets(a);
    while((strcmp(a, "ENDOFINPUT")) != 0){
        if(strcmp(a, "START") == 0 || strcmp(a, "END") == 0){
            gets(a);
            continue;
        }
        else{
            for(i=0; i<strlen(a); i++){
                if(a[i]>='F' && a[i]<='Z'){
                    printf("%c", a[i]-5);
                }
                else if(a[i] == 'A'){
                    printf("V");
                }
                else if(a[i] == 'B'){
                    printf("W");
                }
                else if(a[i] == 'C'){
                    printf("X");
                }
                else if(a[i] == 'D'){
                    printf("Y");
                }
                else if(a[i] == 'E'){
                    printf("Z");
                }
                else{
                    printf("%c", a[i]);
                }
            }
            gets(a);
        }
    }
    return 0;
}

