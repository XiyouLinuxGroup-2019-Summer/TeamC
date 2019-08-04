// 任务队列
structwork{
    // 任务函数指针
    void *(process)(void *arg);
    // 函数参数
    void *arg;
    // 指向下一个任务
    struct workfunctor;
};
struct pool{
    // 工作线程列表
    pthread_ *pyhreads;
    // 任务队列
    work *tasks;
    // 互斥锁
    pthread_mutex_t mutex;
    // 条件变量
    pthread_cond_t cond;
    // 标志线程池状态
    int stop;
    // 线程池允许的做大活动线程数目
    int thread_max_num;
    // 任务队列当前任务数
    int cur_task;
};
// 初始化线程池
pool pool;
void initpool (size_t t_num){
    pool = (pool *)malloc(sizeof(pool));
    // 初始化条件变量和互斥量
    pthread_mutex_init (&(pool->mutex), NULL);
    pthread_cond_init (&(pool->cond), NULL);
    // 初始化任务队列
    pool->tasks = NULL;
    pool->threads = (pthread_t *) malloc (sizeof(pthread_t)*t_num);
    pool->max_thread_num = t_num;
    pool->cur_tasks = 0;
    pool->stop = 0;
    for (size_t i=0; i<t_num; ++i){
        pthread_create (&(pool->threads[i], NULL, run, NULL));
    }
}
// 工作线程
void *run(void *arg){
    while(!pool->stop){
        pthread_mutex_lock (&(pool->mutex));
        // 若等待队列为0 则处于阻塞状态
        while (pool->cur_tasks == 0 && !pool->stop){
            pthread_cond_wait (&(poo1->cond),&(pool->mutex));
        }
        // 销毁线程池
        if (pool->stop){
            pthread_mutex_unlock (&(pool->mutex));
            pthread_exit (NULL);
        }
        // 从任务列表中取出任务函数
        pool->cur_tasks--;
        work *temp = pool->tasks;
        pool->tasks = pool->tasks->next;
        pthread_mutex_unlock (&(pool->mutex));
        // 开始执行任务
        temp->process (temp->arg);
        free (temp);
    }
}
// 添加任务
int append(void *(*process)(void *arg),void *arg)
{
    work *newwork = (work *)malloc(sizeof(work));
    newwork->process = process;
    newwork->arg = arg;
    newwork->next = NULL;
    pthread_mutex_lock(&(pool->mutex));
    // 将新任务添加到tasks的末尾
    work *p = pool->tasks;
    if (p){
        while (p->next)
            p = p->next;
        p->naxt = newwork;
    }
    else
        pool->tasks = newwork;
    pool->cur_tasks++;
    // 唤醒线程去执行任务
    pthread_cond_signal (&(pool->cond));
    pthread_mutex_unlock (&(pool->mutex));
    reurn 0;
}
// 销毁线程池
int destory()
{
    if (pool->stop)
        return -1;
    pool->stop = 1;
    // 唤醒所有线程
    pthread_cond_brocast (&(pool->cond));
    // 等待所有线程结束
    for (int i=0; i<pool->max_thread_num; ++i){
        pthread_join (pool->threads[i]. NULL);
    }
    free (pool->threads);
    // 销毁等待队列
    while (pool->tasks){
        work *temp = pool->tasks;
        pool->tasks = pool->tasks->next;
        free (temp);
    }
    // 销毁互斥锁和条件变量
    pthread_mutex_destory (&(pool->mutex));
    pthread_cond_destory (&(pool->cond));
    free (pool);
}
