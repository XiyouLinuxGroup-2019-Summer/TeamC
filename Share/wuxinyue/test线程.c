// OSIX 下线程控制的实验程序残缺版 
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
// 线程的个数
#define MAX_THREAD 3 
// unsigned long  long是比long还长的整数 main_counter是一个全局变量
unsigned long long main_counter, counter[MAX_THREAD];
void* thread_worker(void*);
int main(int argc,char* argv[])
{
    int i, rtn, ch;
	// 存放线程id
    pthread_t pthread_id[MAX_THREAD] = {0}; 
    for (i=0; i<MAX_THREAD; i++)
    {
		pthread_create (&pthread_id[i], NULL, thread_worker, ( void *)i);
    }
    do
    {
    // 用户按一次回车执行下面的循环体一次 按q退出 
        unsigned long long sum = 0;
    // 求所有线程的counter的和 
        for (i=0; i<MAX_THREAD; i++)
        {
        // 求所有counter的和 
            sum += counter[i];
            printf("%llu ", counter[i]);

        }
        printf("%llu/%llu", main_counter, sum);

	}while ((ch = getchar()) != 'q');
	return 0;
}
void* thread_worker(void* p)
{
	int thread_num;
	thread_num = p;
    for(;;){
		// 无限循环 
		// 本线程的counter加一
		// printf("%d\n", thread_num);
		// counter[i]的值不一样是因为每个子线程的执行次数是不确定的 谁拿到时间片谁运行
        counter[thread_num]++;
		// 主counter加一
		// 全局变量main_counter因为没有加锁保护，所以它的值可能修改
        main_counter++;
    }
}
// 主线程结束 子线程跟着结束
