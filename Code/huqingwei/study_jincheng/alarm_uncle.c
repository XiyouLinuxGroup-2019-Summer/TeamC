#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <string.h>

int main()
{
    alarm(1);

    int i = 0;

    while(1){
        printf("%d\n", i++);
    }

    return 0;
}

