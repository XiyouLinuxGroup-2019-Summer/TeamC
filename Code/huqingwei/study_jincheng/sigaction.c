#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void myfunc(int no){
    printf("hello, world\n");
}

int main()
{
    struct sigaction act;
    //int
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    //act.sa_headler = myfunc;

    sigaction(SIGINT, &act, NULL);

    while(1);

    return 0;
}

