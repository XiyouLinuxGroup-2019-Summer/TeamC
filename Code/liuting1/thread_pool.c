#include <pthread.h>

//队列
typedef struct WorkFun
{
    void *(process)(void *arg);
    void *arg;
    struct WorkFun *next;
}Work;

//线程池数据结构
typedef struct threadpoll
{
    pthread_t *threads;       //线程队列
    Work *tasks;

    pthread_mutex_t mutex;      //
    pthread_cond_t  cond;

    int stop;
    int thread_max_num;
    int cur_task;
}Pool;

void initPool(void *t_num)
{
    pool =(Pool*)malloc(sizeof(Pool));

    pthread_mutex_init(&(pool->mutex),NULL);
    pthread_cond_init(&(pool->cond),NULL);

    poll->tasks = NULL;
    poll->threads = (pthread_t)malloc(sizeof(pthread_t)*t_num);

    pool->stop = 0;
    pool->cur_task = 0；
    pool->thread_max_num = t_num;

    for (i = 0; i < t_num; i++)
    {
        pthread_creat (&(pool->threads[i]), NULL, run, NULL);
    }
}

void *run()
{
    
    while(!pool->stop)
    {
        pthread_wait_lock(&(pool->mutex));
    }
    if(pool->stop)
    {
        pthread_mutex_unlock(&(pool->mutex));
        pthread_exit(NULL);
    }
    poll->cur_tasks--;
    Work *temp =pool->tasks;
    pool->tasks=pool->tasks->next;
    pthread_mutex_unlock(&(pool->mutex));
    temp->process(tmp-arg);
    free(tmp);
}


int append(void *(process)(void *arg),void *arg)
{
    Work *network = (Work)malloc(sizeof(Work));

    network->process = process;
    network->arg=arg;
    network->next=NULL;

    pthread_mutex_lock (&(pool->mutex));

    Work *p=pool->tasks;
    if(p)
    {
        while(p->next)
        {
            p=p->next;
        }
        p->next=network;
    }
    else
    {
        pool->tasks=network;
    }
    pool->cur_task++;
    pthread_cond_signal(&(pool->cond));
    pthread_mutex_unlock(&(pool->mutex));
    return 0;
}   


int Desory()
{
    if(pool->stop)
    {
        return -1;
    }
    pool->stop=1;
    pthread_cond_broadcast(&(pool->cond));

    for(int i=0;i<pool->max_thread_num;++i)
    {
        pthread_join(pool->thread[i],NULL);
    }
    free(pool->threads);

    while(pool->tasks)
    {
        Work *temp=pool->tasks;
        pool->tasks= pool->tasks->next;
        free(temp);
    }

    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destory(&(pool->cond));
    free(pool);
}

