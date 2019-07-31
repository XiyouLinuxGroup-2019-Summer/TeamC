#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int number = 100;

void *myfunc(void *arg){
    //打印子线程id
    printf("child thread id: %lu\n", pthread_self());
    for(int i=0; i<5; i++){
        printf("child i = %d\n", i);
        if(i == 2){
            //子线程退出
            //exit(1);
            //int number = 100;
            pthread_exit(&number);
            //return NULL;
        }
    }
    return NULL;
}

int main()
{
    //创建一个子线程
    //线程ID变量
    pthread_t pthid;
    //返回错误号
    int ret = pthread_create(&pthid, NULL, myfunc, NULL);
    if(ret != 0){
        printf("error number: %d\n", ret);
        //打印错误信息
        printf("%s\n", strerror(ret));
    }
    printf("parent thread id: %lu\n", pthread_self());

    /*for(int i=0; i<5; i++){
        printf("parent i = %d\n", i);
    }*/

    //阻塞等待子线程退出，并且回收pcb
    void *ptr = NULL;
    pthread_join(pthid, &ptr);
    printf("number = %d\n", *(int*)ptr);

    int i = 0;
    while(i < 10){
        printf("parent i = %d\n", i++);
    }

    return 0;
}

