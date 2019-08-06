// POSIX 下进程控制的实验程序残缺版 
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
// 允许建立的子进程个数最大值 
#define MAX_CHILD_NUMBER 10
// 子进程睡眠时间 
#define SLEEP_INTERVAL 2
int proc_number=0;
// 子进程的自编号，从0开始 
void do_something();

int main(int argc, char* argv[])
{
    // 子进程个数 
	int child_proc_number = MAX_CHILD_NUMBER;
	int i, ch;
	pid_t  child_pid;
	// 存放每个进程的id
	pid_t pid[10]={0};
	//printf("%d %s", argc, argv[1]);
	// 进不来if循坏 此时argc等于1 
    // 命令行参数第一个参数表示子进程个数
	if (argc > 1)
    {
		printf("%d %s", argc, argv[1]);
		// 将字符串转换为一个整形数
        child_proc_number = atoi(argv[1]);
        child_proc_number= (child_proc_number > 10) ? 10 :child_proc_number;

    }
    for (i=0; i<child_proc_number; i++) {
		child_pid = fork();
	// 父进程把子进程的id保存到pid[i]
		if (child_pid != 0 && child_pid!= -1) {
			pid[i] = child_pid;
		}
		if (child_pid == 0){
			// prov_number表示是创建的第几个进程在执行
			proc_number = i;
			do_something();
		}
    }
    // 让用户选择杀死进程，数字表示杀死该进程，q退出 
	while ((ch = getchar()) != 'q')
    {
	// 用于检查传入的字符是否是十进制数 只要不等于0 杀死该进程
        if (isdigit(ch))
        {
    // 填写代码，向pid[ch-'0']发信号SIGTERM，
			kill (pid[ch-'0'], SIGTERM);
        }
    }
	// 当pid=0时 发送信号sig给当前进程所属进程组里的所有进程
	kill (0,SIGTERM);
	return;
}
void do_something()
{
    for(;;)
    {
        printf("This is process No.%d and its pid is %d\n",proc_number,  getpid());
		// 主动休眠
		sleep(SLEEP_INTERVAL);
    }
}
