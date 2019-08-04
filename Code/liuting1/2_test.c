/*************************************************************************
	> File Name: 2_test.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月03日 星期六 15时08分39秒
 ************************************************************************/

#include<stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

pthread_mutex_t chopstick[6] ;

void get(int *left,int *right,char phi) {
    switch (phi){
        case 'A':
            *left = 5;
            *right = 1;
            break;
        case 'B':
            *left = 1;
            *right = 2;
            break;
        case 'C':
            *left = 2;
            *right = 3;
            break;
        case 'D':
            *left = 3;
            *right = 4;
            break;
        case 'E':
            *left = 4;
            *right = 5;
            break;
    }

}

void *eat_think(void *arg)
{
    //将传入参数赋值给phi
    char phi = *(char *)arg;
    int left,right;
    //地址传递,get函数操作的是left,right的这个函数在栈上的地址;
    get(&left,&right,phi);
    
    for(;;){

        //因为是死循环，阻塞该线程，要不容易这个线程可能会一直循环。
        usleep(10);
        //给左筷子加锁
        pthread_mutex_lock(&chopstick[left]);
        printf("Philosopher %c fetches chopstick %d\n", phi,left);

        //给右筷子尝试加锁以非阻塞的方式，如果加锁不成功，执行if里面的语句，给左筷子解锁。
        if (pthread_mutex_trylock(&chopstick[right]) ==EBUSY){
            pthread_mutex_unlock(&chopstick[left]);
            continue;
        }
        //到了这里，就说明已经加锁成功了给右筷子，使用它。
        printf("Philosopher %c fetches chopstick %d\n", phi,right);
        printf("Philosopher %c is eating.\n",phi);
        //阻塞该线程，让他等会放下筷子，吃饭需要时间。
        usleep(10);
        //筷子使用完成，解锁。
        pthread_mutex_unlock(&chopstick[left]);
        printf("Philosopher %c release chopstick %d\n", phi,left);
        pthread_mutex_unlock(&chopstick[right]);
        printf("Philosopher %c release chopstick %d\n", phi,right);
    }
}

int main()
{
    pthread_t A,B,C,D,E; //5个哲学家
    int i;
    //初始化锁
    for (i = 0; i < 5; i++)
        pthread_mutex_init(&chopstick[i],NULL);

    //5个线程，代表5个哲学家的行为
    //eat_think为函数指针，代表传入函数的地址
   /*最后一个参数是强制转换为void类型，是传入函数中的参数，有可能各种类型，所以强转为void*，而且传入的指针不是指针，里面放的是字符串，而并不是地址*/
    pthread_create(&A,NULL, eat_think, (void *)"A");
    pthread_create(&B,NULL, eat_think, (void *)"B");
    pthread_create(&C,NULL, eat_think, (void *)"C");
    pthread_create(&D,NULL, eat_think, (void *)"D");
    pthread_create(&E,NULL, eat_think, (void *)"E");

    //主线程被挂起，等待5个子线程的结束，用来回收资源。
    pthread_join(A,NULL);
    pthread_join(B,NULL);
    pthread_join(C,NULL);
    pthread_join(D,NULL);
    pthread_join(E,NULL);
    return 0;
}

