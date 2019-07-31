#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *myfunc(void *arg){
    while(1){
        int i, j, k;
        i = 10;
        j = 20;
        k = i+j;
        //printf("k = %d\n", k);
        pthread_testcancel(); //设置一个取消点
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

    for(int i=0; i<5; i++){
        printf("i = %d\n", i);
    }

    sleep(2);
    return 0;
}

