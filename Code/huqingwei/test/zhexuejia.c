/*由五个哲学家围成一圈，在每个哲学家之间放一个筷子，
 当一个哲学家左右手都抢到筷子的时候，就可以吃一口饭，然后把筷子放下，继续抢筷子*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

pthread_mutex_t chopstick[6] ; //创建六把互斥锁

void get(int *left,int *right,char phi) {  //每个哲学家左右手是几号筷子
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
    char phi = *(char *)arg;
    int left,right;
    get(&left,&right,phi);
    for(;;){
        usleep(10);
        //互斥锁上锁，相当与一个哲学家抢到了左手边的筷子
        pthread_mutex_lock(&chopstick[left]);
        printf("Philosopher %c fetches chopstick %d\n", phi,left); //看哪个哲学家的左手抢到了筷子
        //该哲学家抢到了左手边的筷子，就尝试区抢右手边的筷子，如果抢到了，给右边的筷子上锁，没有就继续抢
        if (pthread_mutex_trylock(&chopstick[right]) ==EBUSY){
            pthread_mutex_unlock(&chopstick[left]);
            continue;
        }
        printf("Philosopher %c fetches chopstick %d\n", phi,right); //这个哲学家的右手抢到了筷子
        printf("Philosopher %c is eating.\n",phi); //这个哲学家开始吃一口
        usleep(10);
        //该哲学家放下左手的筷子
        pthread_mutex_unlock(&chopstick[left]);
        printf("Philosopher %c release chopstick %d\n", phi,left);
        //互斥锁解锁，该哲学家放下右手的筷子
        pthread_mutex_unlock(&chopstick[right]);
        //提示左右手的筷子都放下了
        printf("Philosopher %c release chopstick %d\n", phi,right);
    }
}

int main(){
    pthread_t A,B,C,D,E; //5个哲学家
    int i;
    for (i = 0; i < 5; i++)
        pthread_mutex_init(&chopstick[i],NULL);  //初始化五把锁；
    /*创建5个线程*/
    pthread_create(&A,NULL, eat_think, (void *)"A");
    pthread_create(&B,NULL, eat_think, (void *)"B");
    pthread_create(&C,NULL, eat_think, (void *)"C");
    pthread_create(&D,NULL, eat_think, (void *)"D");
    pthread_create(&E,NULL, eat_think, (void *)"E");

    /*销毁线程*/
    pthread_join(A,NULL);
    pthread_join(B,NULL);
    pthread_join(C,NULL);
    pthread_join(D,NULL);
    pthread_join(E,NULL);
    
    return 0;
}
