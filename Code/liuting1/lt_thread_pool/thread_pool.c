/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月05日 星期一 11时45分12秒
 ************************************************************************/

#include "thread_pool.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

typedef struct Thread_pool
{
    Queue *queue;
    pthread_t *threads;
    
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    size_t thread_count;
    int is_start;
} Thread_pool;

typedef struct Task
{
    void (*callback)(void *);
    void *args;
} Task;

Thread_pool *thread_pool_init(size_t thread_count)
{
    Thread_pool *pool = (Thread_pool *)malloc(sizeof(Thread_pool));
    pool->queue = Queue_init(); 
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*thread_count);
    pool->thread_count = thread_count;
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);
    pool->is_start = 0;
}

void thread_pool_destroy(Thread_pool *pool)
{
    thread_pool_stop(pool);
    pthread_cond_broadcast(&pool->cond);
    for(int i = 0; i < 2; ++i)
    {
        pthread_join(pool->threads[i], NULL);
    }
    Queue_destory(pool->queue, free);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
}

void *thread_work(void *args)
{
    Thread_pool *pool = (Thread_pool *)args;
    while(pool->is_start)
    {
        pthread_mutex_lock(&pool->mutex);
        while(Queue_size(pool->queue) < 1 && pool->is_start)
        {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        
        }
        if(pool->is_start==0)
        {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(0);
        }
        Task *task = (Task *)Queue_pop(pool->queue);
        pthread_mutex_unlock(&pool->mutex);
        if(task &&  task->callback)
        {
            task->callback(task->args);
            free(task);
        }
    }
}

void thread_pool_start(Thread_pool *pool)
{
    if(pool->is_start)
    {
        return ;
    }
    pool->is_start = 1;
    for(int i = 0; i < pool->thread_count; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, thread_work, (void *)pool);
    }
}


void thread_pool_pushtask(Thread_pool *pool, void(*callback)(void *), void *args)
{
    Task *task = (Task *)malloc(sizeof(task));
    task->callback = callback;
    task->args = args;
    pthread_mutex_lock(&pool->mutex);
    Queue_push(pool->queue, task); 
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->cond);
}

void thread_pool_stop(Thread_pool *pool)
{
    pool->is_start = 0;
}

void thread_pool_taskcount(Thread_pool *pool)
{
    Queue_size(pool->queue);
}





