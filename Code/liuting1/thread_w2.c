//* POSIX 下线程死锁的演示程序 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#define LOOP_TIMES 100

/*用宏PTHREAD_MUTEX_INITIALIZER来初始化 */
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread_worker(void*);
void critical_section(int thread_num, int i);

int main(void)
{
	int rtn, i;
	pthread_t pthread_id = 0; /* 存放子线程的id */
	rtn = pthread_create(&pthread_id,NULL, thread_worker, NULL );
	if(rtn != 0)
	{
		printf("pthread_create ERROR!\n");
		return -1;
	}
	for (i=0; i<LOOP_TIMES; i++)
	{
        pthread_mutex_lock(&mutex1);
        pthread_mutex_lock(&mutex2);
        critical_section(1, i);
        pthread_mutex_unlock(&mutex2);
		pthread_mutex_unlock(&mutex1);
	}
    sleep(1);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    return 0;
}
void* thread_worker(void* p)
{
	int i;
    printf("##\n");
	for (i=0; i<LOOP_TIMES; i++)
	{
        pthread_mutex_lock(&mutex2);
		pthread_mutex_lock(&mutex1);
		critical_section(2, i);
        pthread_mutex_unlock(&mutex2);
		pthread_mutex_unlock(&mutex1);
	}
}
void critical_section(int thread_num, int i)
{
	printf("Thread%d: %d\n", thread_num,i);
}
//1.主线程for()后sleep(1),阻塞住，等待子线程执行完，
//2.或者在主线程销毁两个锁之前，加pthread_join(),回收子线程

//线程是并行加并发的机制

//修改方法：
//1. 删除所有的锁，因为线程调用的是一个函数，并不操作数据。
//2. 删除一个锁，留一个锁,两个锁可相同可不同
//3. 因为是并发加并行机制,所以一切皆有可能，可能在刚在主线程上完一个锁后（主线程的另一个锁还没上），又去另一个线程重复上锁。有可能能正确运行结束，但是大多可能成为死锁。

