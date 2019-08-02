#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main()
{
    //设置定时器
    struct itimerval new_val;
    //第一触发的时间
    new_val.it_value.tv_sec = 2; //s
    new_val.it_value.tv_usec = 2; //ws
    
    //周期性定时
    new_val.it_interval.tv_sec = 1;
    new_val.it_interval.tv_usec = 0;

    //倒计时2s
    setitimer(ITIMER_REAL, &new_val, NULL);

    while(1){
        printf("hello, world\n");
        sleep(1);
    }

    return 0;
}

