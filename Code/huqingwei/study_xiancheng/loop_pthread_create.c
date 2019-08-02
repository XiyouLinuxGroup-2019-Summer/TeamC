#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int  count = 0;
void *myfunc(void *arg){
    //取数据
    //int num = *(int*)arg;
    int num = (int)arg;
    //打印子线程id
    printf("%dth child thread id: %lu\n", num, pthread_self());
    return NULL;
}

int main()
{
    //创建一个子线程
    //线程ID变量
    pthread_t pthid[5];
    for(int i=0; i<5; i++){
        //第4个参数传递的是地址
        //pthread_create(&pthid[i], NULL, myfunc, (void*)&i);
        //传值
        pthread_create(&pthid[i], NULL, myfunc, (void*)i);
        count = i;
    }
    printf("parent thread id: %lu\n", pthread_self());

    for(int i=0; i<5; i++){
        printf("i = %d\n", i);
    }

    sleep(2);
    
    return 0;
}

