#include <stdio.h>
#include <pthread.h>
int produce[1000];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *func()
{
        int i;
        pthread_detach(pthread_self());
        pthread_mutex_lock(&mutex);
        for(i = 0; i < 1000; i++)
        {
            if(produce[i] == 0)
                    produce[i]++;
        }
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        return NULL;
}
int main()
{
        int i,o=0;
        pthread_t tid;
        for(;;){
                pthread_create(&tid, NULL, func, NULL);
                pthread_mutex_lock(&mutex);
                while(produce[0]==0){
                        pthread_cond_wait(&cond, &mutex);
                }
                while(produce[0] != 0){
                    for(i = 0; i<1000; i++){
                        if(produce[i] == 1){
                                printf("%d\n",o++);
                                produce[i] = 0;
                        }
                    }
                }
                pthread_mutex_unlock(&mutex);
        }
        return 0;
}

