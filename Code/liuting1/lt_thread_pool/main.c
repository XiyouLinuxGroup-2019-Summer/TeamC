/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月05日 星期一 14时41分00秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_pool.h"

void sum(void *args)
{
    int sum = 0;
    for (int i = 0; i < 100; i++)
    {
        sum += i;
    }
    printf("%d\n",sum);
}

int main()
{
    Thread_pool *pool = thread_pool_init(4);
    
    thread_pool_start(pool);
    thread_pool_pushtask(pool, sum, NULL);
    thread_pool_pushtask(pool, sum, NULL);
    thread_pool_pushtask(pool, sum, NULL);
    thread_pool_pushtask(pool, sum, NULL);
    thread_pool_pushtask(pool, sum, NULL);
    thread_pool_pushtask(pool, sum, NULL);
    sleep(2);
    thread_pool_stop(pool);
    thread_pool_destroy(pool);

    return 0;
}
