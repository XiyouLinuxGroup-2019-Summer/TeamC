#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 10000

int number;

//创建一把互斥锁
pthread_mutex_t mutex;

void *funcA_num(void *arg){
    for(int i=0; i<MAX; i++){
        //访问全局变量之前加锁
        //如果mutex被锁上了，代码阻塞到当前位置
        pthread_mutex_lock(&mutex);
        int cur = number;
        cur++;
        number = cur;
        printf("Thread A, id = %lu, number = %d\n", pthread_self(), number);
        //解锁
        pthread_mutex_unlock(&mutex);
        usleep(10);
    }

    return NULL;
}

void *funcB_num(void *arg){
    for(int i=0; i<MAX; i++){
        //加锁
        pthread_mutex_lock(&mutex);  
        //pthread_mutex_lock(&mutex);  
        int cur = number;
        cur++;
        number = cur;
        printf("Thread B, id = %lu, number = %d\n", pthread_self(), number);
        pthread_mutex_unlock(&mutex);
        usleep(10);
    }

    return NULL;
}

int main()
{
    pthread_t p1, p2;
    //初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    //创建两个子程序
    pthread_create(&p1, NULL, funcA_num, NULL);
    pthread_create(&p2, NULL, funcB_num, NULL);

    //阻塞
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    //释放互斥锁资源
    pthread_mutex_destroy(&mutex);

    return 0;
}

