#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
//将每一个筷子看做临界区 对每一个筷子设置互斥量
pthread_mutex_t chopstick[6] ;
void get(int *left,int *right,char phi) {
switch (phi){
    //哲学家编号 筷子编号
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
    //哲学家的编号
    char phi = *(char *)arg;
    int left,right;
    // 得到哲学家编号和他左右手所对应的筷子编号
    get(&left,&right,phi);
    for(;;){
        //线程挂起
        usleep(10);
        // 给左边的筷子加锁，只允许当前哲学家使用它，其他人不能使用
        // 也就是相当于给当前线程加锁，只允许当前线程对变量进行操作
        pthread_mutex_lock(&chopstick[left]);
        printf("Philosopher %c fetches chopstick %d\n", phi,left);
        // trylock是尝试加锁 如果当前线层已经加锁 那么返回ebusy.如果没有加锁，
        // 则给这个临界区加锁，只允许当前线程操作
        // 如果哲学家右边的筷子被人使用 那他就不能使用这根筷子 就不能吃饭 只能进行思考
        // 如果哲学家旁边的筷子没有被使用，那么哲学家可以使用右边的筷子，他就可以吃饭
        if (pthread_mutex_trylock(&chopstick[right]) ==EBUSY){
        pthread_mutex_unlock(&chopstick[left]);
        continue;
    }
    printf("Philosopher %c fetches chopstick %d\n", phi,right);
    printf("Philosopher %c is eating.\n",phi);
    // 线程挂起
    usleep(10);
    // 哲学家吃完饭后，放下左筷子，给这个临界区解锁
    pthread_mutex_unlock(&chopstick[left]);
    printf("Philosopher %c release chopstick %d\n", phi,left);
    // 同理
    pthread_mutex_unlock(&chopstick[right]);
    printf("Philosopher %c release chopstick %d\n", phi,right);
}
}
int main(){
pthread_t A,B,C,D,E; //5个哲学家
int i;
for (i = 0; i < 5; i++)
    // 分配互斥量
    pthread_mutex_init(&chopstick[i],NULL);
// 创建5个线程 判断哲学家吃饭还是思考
pthread_create(&A,NULL, eat_think, (void *)"A");
pthread_create(&B,NULL, eat_think, (void *)"B");
pthread_create(&C,NULL, eat_think, (void *)"C");
pthread_create(&D,NULL, eat_think, (void *)"D");
pthread_create(&E,NULL, eat_think, (void *)"E");
// 主线程挂起 等待A这几个线程终止
pthread_join(A,NULL);
pthread_join(B,NULL);
pthread_join(C,NULL);
pthread_join(D,NULL);
pthread_join(E,NULL);
return 0;
}

