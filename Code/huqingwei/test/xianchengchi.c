#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define num 10

struct xiancheng_pool{

};

typedef xiancheng_pool Item;

//线程池结构体
struct pool{
    Item item;
    struct pool *next;
};

pthread_mutex_t mutex;  //创建互斥锁
pthread_cond_t cond;//创建条件变量

void *work(void *arg);

int main()
{
    pthread_t xian[num]; 
    int i;
    
    //初始化互斥锁和条件变量
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //创建线程
    for(i=0; i<num; i++)
    {
        pthread_create(&xian[i], NULL, work, NULL);
    }
    
    //销毁锁，条件变量
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    //回收线程
    for(i=0; i<num; i++)
    {
        pthread_join(xian[i], NULL);
    }

    return 0;
}
