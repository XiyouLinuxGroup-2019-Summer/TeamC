/*************************************************************************
	> File Name: thread_pool.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月05日 星期一 11时37分51秒
 ************************************************************************/
#include <stddef.h>

typedef struct Thread_pool Thread_pool;

Thread_pool *thread_pool_init(size_t thread_count);

void thread_pool_destroy(Thread_pool *pool);
void thread_pool_start(Thread_pool *pool);
void thread_pool_stop(Thread_pool *pool);
void thread_pool_pushtask(Thread_pool *pool, void(*callback)(void *),void *args);
void thread_pool_taskcount(Thread_pool *pool);
