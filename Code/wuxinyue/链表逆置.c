#include<stdio.h>
#include<stdlib.h>
typedef struct node
{
    int data;
    struct node *next;
}stu;
stu *creat() //函数返回值是一个指针
{
    int n = 0;
    stu *phead,*pend,*pnew;
    phead = NULL;
    pend=phead;
    pnew = (stu *) malloc (sizeof(stu));
    pend = (stu *) malloc (sizeof(stu));
    scanf ("%d", &pnew->data);
    while ( pnew->data !=0 )
    {
        n++;
        if ( n==1 )
        {
            phead = pnew; // 指向新开辟的结点
            pnew->next = NULL;
        }
        else
        {
            pend->next = pnew;
            pnew->next = NULL;
        }
        pend = pnew;
        pnew = ( stu * ) malloc (sizeof(stu));
        scanf ("%d", &pnew->data);
    }
    free ( pnew );
    return phead;
}
stu *reverse ( stu *phead  )
{
    stu *pnew,*ptemp;
    pnew = NULL;
    ptemp = phead;
    while ( ptemp )
    {
        phead = phead->next;
        ptemp->next = pnew;
        pnew = ptemp;
        ptemp = phead;
    }
    phead = pnew;
    return phead;
}
void print ( stu *phead )
{
    stu *ptemp;
    ptemp = phead;
    while ( ptemp )
    {
        printf("%d ", ptemp->data);
        ptemp = ptemp->next;
    }
}
int main()
{
    stu *phead;
    phead = creat();
    phead = reverse(phead);
    print( phead );
}
