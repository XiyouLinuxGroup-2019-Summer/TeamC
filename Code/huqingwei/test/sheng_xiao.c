#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

typedef int Item;

//创建互斥锁，条件变量
pthread_mutex_t mutex;
pthread_cond_t cond;

//商品结构体
struct shangpin{
    Item item;
    struct shangpin *next;
};

//声明商品，并初始化
struct shangpin *shangpin_head = NULL;

void *shengchan(void *arg);
void *xiaofei(void *arg);

int main()
{
    //初始化互斥锁，条件变量
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    //创建生产者线程，消费者线程
    pthread_t sheng;
    pthread_t xiao;
    
    pthread_create(&sheng, NULL, shengchan, NULL);
    pthread_create(&xiao, NULL, xiaofei, NULL);


    //销毁互斥锁，条件变量
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    //线程回收
    pthread_join(sheng, NULL);
    pthread_join(xiao, NULL);

    return 0;
}

void *shengchan(void *arg)
{
    //生产者生产商品
    while(1)
    {
        //printf("a\n");
        pthread_mutex_lock(&mutex);
        
        //创建一个商品
        struct shangpin *current, *prev;
        current = (struct shangpin*)malloc(sizeof(struct shangpin));

        if(shangpin_head == NULL)
        {
            shangpin_head = current;
            current->next = NULL;
            printf("shangpin+1\n");
        }
        else
        {
            shangpin_head->next = current;
            current->next = NULL;
            printf("shangpin + 1\n");
        }

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        sleep(1);
    }
}

void *xiaofei(void *arg)
{
    //消费者消费商品
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(shangpin_head == NULL)
        {
            pthread_cond_wait(&cond, &mutex);
            //continue;
        }
        else
        {
            struct shangpin *temp = shangpin_head;
            shangpin_head = temp->next;
            free(temp);
            printf("------shangpin - 1\n");
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}
