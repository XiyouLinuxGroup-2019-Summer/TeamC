#include<stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

pthread_mutex_t mutex; 
char buff[100];

void *thread1()
{

    int len;
    char data[10];
    sleep(5);
    while(1)
    {
        scanf("%s",data);
        len = strlen(buff);
        if(len<100)
        {

            pthread_mutex_lock(&mutex);
            strcat(buff,data);
            printf("**%s\n",buff);
            pthread_mutex_unlock(&mutex);
        
        }
        else
        {
            sleep(20);
        }

        
    }
}

void *thread2()
{
    int len;
    sleep(10);
    while(1)
    {
        len = strlen(buff);
        if(len-5>0)
        {

            pthread_mutex_lock(&mutex);
            buff[len-5]='\0';
            printf("&&%s\n",buff);
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            sleep(10);
        }
        
    }
    
}

int main()
{
    pthread_t proer,cuser;

    pthread_mutex_init(&mutex,NULL);

    pthread_create(&proer,NULL,thread1,NULL);
    pthread_create(&cuser,NULL,thread2,NULL);
    
    pthread_join(proer,NULL);
    pthread_join(cuser,NULL);
    return 0;
}

