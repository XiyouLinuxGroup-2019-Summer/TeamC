/*************************************************************************
	> File Name: queue_test.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月06日 星期二 09时48分25秒
 ************************************************************************/
#include "queue.h"

#include <stdio.h>

int main()
{
    Queue *queue = Queue_init();

    int m=4;
    while(m--)
    {
       Queue_push(queue,(void*)&m);
       Queue_pop(queue);
    }
    Queue_destory(queue,NULL);
}
