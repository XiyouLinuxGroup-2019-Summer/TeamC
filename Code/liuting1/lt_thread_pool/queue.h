/*************************************************************************
	> File Name: queue.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月05日 星期一 10时48分18秒
 ************************************************************************/
#include <stddef.h>

typedef struct Queue Queue;

Queue *Queue_init(void);

void Queue_destory(Queue *queue, void(*dataDestory)(void *));
void Queue_push(Queue *queue, void *data);
void *Queue_pop(Queue *queue);
size_t Queue_size(Queue *queue);

