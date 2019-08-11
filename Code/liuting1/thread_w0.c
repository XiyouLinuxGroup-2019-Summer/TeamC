/*************************************************************************
	> File Name: thread_w0.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月09日 星期五 14时37分58秒
 ************************************************************************/
 
 /*  POSIX 下进程控制的实验程序残缺版 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_CHILD_NUMBER 10
#define SLEEP_INTERVAL 2

int proc_number=0;
void do_something();

int main(int argc, char* argv[])
{

	int child_proc_number = MAX_CHILD_NUMBER;
	int i, ch;
	pid_t  child_pid,a;
	pid_t  pid[10]={0}; 
    if (argc > 1) {
        child_proc_number = atoi(argv[1]);
        child_proc_number= (child_proc_number > 10) ? 10 :child_proc_number;

    }
    for (i=0; i<child_proc_number; i++) {
        if((a = fork()) < 0) {
            printf("fork error!\n");
            exit(-1);
        }
        else if(a == 0) {
            proc_number = i;
            do_something();
        }
        else {
            pid[i] = a;            
        }
    }
	while ((ch = getchar()) != 'q') {
        if (isdigit(ch)) {
             if (isdigit(ch)) {
                kill(pid[ch - '0'],SIGTERM);
            }
        }
    }
    for(int i = 0;i <= proc_number;i++) {
        kill(pid[i],SIGTERM);
    }
    kill(0,SIGTERM);

	return 0;
}
void do_something()
{
    for(;;)
    {
        printf("This is process No.%d and its pid is %d\n",proc_number,  getpid());
        sleep(SLEEP_INTERVAL);
    }
}
