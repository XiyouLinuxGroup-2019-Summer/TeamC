#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int data;
    struct node *next;
}NODE,*node;

int main()
{
    node phead,pend,pnew;
    phead = (NODE *)malloc(sizeof(NODE));
    phead->next = NULL;
    pend = phead;
    for(int i=0;i<10;i++)
    {
        pnew = (NODE *)malloc(sizeof(NODE));
        pnew->data = i;
        pnew->next=NULL;
        pend->next=pnew;
        pend=pnew;
    }

    pnew->next=NULL;
    node ptemp=phead->next;
    while(ptemp)
    {
        printf("%d\n",ptemp->data);
        ptemp=ptemp->next;
    }
    node p,q;
    p=phead->next;
    phead->next=NULL;
    while(p)
    {
        q=p;
        p=p->next;
        q->next = phead->next;
        phead->next=q;
    }
    while(q)
    {
        printf("%d\n",q->data);
        q=q->next;
    }
}
