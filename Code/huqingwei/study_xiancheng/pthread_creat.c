#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *myfunc(void *arg){
    //打印子线程id
    printf("child thread id: %lu\n", pthread_self());
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

