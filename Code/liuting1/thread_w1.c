/*  POSIX 下线程控制的实验程序残缺版 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_THREAD 3 /* 线程的个数 */
unsigned long long main_counter=0, counter[MAX_THREAD];
/* unsigned long  long是比long还长的整数 */
void* thread_worker(void*);
int main(int argc,char* argv[])
{
    int i, rtn, ch;
    pthread_t pthread_id[MAX_THREAD] = {0}; /* 存放线程id*/
    for (i=0; i<MAX_THREAD; i++)
    {
        pthread_create(&pthread_id[i], NULL, thread_worker, (void *)&i);
    }
    do
    {
    /* 用户按一次回车执行下面的循环体一次。按q退出 */
        unsigned long long sum = 0;
    /* 求所有线程的counter的和 */
        for (i=0; i<MAX_THREAD; i++)
        {
        /* 求所有counter的和 */
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
    thread_num =*(int *)p;
/* 在这里填写代码，把main中的i的值传递给thread_num */
    for(;;)
    { /* 无限循环 */
        counter[thread_num]++; /* 本线程的counter加一 */
        main_counter++; /* 主counter 加一 */
    }
}

// 前三列数不相等，线程是并行加并发的关系
//
// 1.三个子线程的运行时间可能不一样，前三列数的关系不确定。（为什么前三列数结果是可能0）
// 斜杠两边的数的关系不一定：
// 1.在子进程函数中，有可能执行了第一句后，他就切换到了别的线程，所以main_counter < sum
//2. 因为子进程是死循环，当主线程中执行sum+=counter[i]以及下面的时候，子线程有可能在继续执行，所以main_counter>sum,一切皆有可能，看操作系统的具体实现。每次运行结果都是不一样的。
//
// 退出：我认为是主线程退出后，子线程被迫退出，这样不好，并未回收子线程所占用的的资源。
