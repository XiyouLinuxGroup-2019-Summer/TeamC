#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

int main()
{
    //手动屏蔽信号
    //自定义信号机
    sigset_t myset;
    //清空集合
    sigemptyset(&myset);
    //添加要阻塞的信号
    sigaddset(&myset, SIGINT); //crtl+c
    sigaddset(&myset, SIGQUIT); //ctrl+反斜杠
    sigaddset(&myset, SIGKILL);

    //自定义集合设置给内核阻塞信号集
    sigprocmask(SIG_BLOCK, &myset, NULL);
    //每隔1s读一次内存的未决信号集
    while(1){
        sigset_t pendset;
        sigpending(&pendset);
        //1-31
        for(int i=1; i<32; i++){
            //对每一个信号一次判断
            if(sigismember(&pendset, i)){
                printf("1");
            }
            else{
                printf("0");
            }
        }
        printf("\n");
        sleep(1);
    }
}
