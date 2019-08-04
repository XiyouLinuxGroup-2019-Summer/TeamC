#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int *thread ( int *n )
{
    *n=*n+1;
    pthread_t newthid;
    newthid = pthread_self();
    printf ("this is a new ,threaed id =%u n=%d\n",newthid,*n);
    return NULL;
}
int main()
{
    int i,n=3;
    pthread_t thid;
    printf ("main thread ,ID is %u\n",pthread_self());// 打印主线程的id
    for ( i=1; i<=3; i++ )
    {
         if ( pthread_create(&thid, NULL, (void *)thread,&n)!=0 )
         {
             printf ("thread creation failed\n");
             exit(1);
         }
    }
   // sleep(1);//主线程休眠，防止子线程来不及运行要退出
   // exit(0);
    pthread_exit(0);//在主函数中调用或者是返回了exit函数退出主线程 则整个进程都将终止 
    // 而如果是调用pthread_exit函数 则仅仅是主线程消亡 进程不会终止 其余线程也不会消亡
}

