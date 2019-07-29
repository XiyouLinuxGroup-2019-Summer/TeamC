#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int Item;

struct node{
    Item item;
    struct node *next;
};

struct node *CreatList(struct node *ph);
void PrintList(struct node *ph);
struct node *NiZhiList(struct node *ph);
//void NiZhiList(struct node *head);

int main()
{
    struct node *head = NULL;

    head = CreatList(head);
    PrintList(head);

    head = NiZhiList(head);
    PrintList(head);

    return 0;
}

struct node *CreatList(struct node *ph){
    struct node *current, *prev;
    int ch;

    while(scanf("%d", &ch) == 1 && ch != 0){
        current = (struct node*)malloc(sizeof(struct node));
        if(ph == NULL)
            ph = current;
        else
            prev->next = current;
        current->next = NULL;

        current->item = ch;
        prev = current;
    }
    return ph;
}

void PrintList(struct node *ph){
    struct node *current;

    current = ph;
    while(current != NULL){
        //printf("c\n");
        printf("%d", current->item);
        current = current->next;
    }
    printf("\n");

    return;
}

struct node *NiZhiList(struct node *ph){
    struct node *current, *prev, *Next;

    current = ph;
    prev = NULL;
    while(current != NULL){
        Next = current->next;
        current->next = prev;
        prev = current;
        current = Next;
    }
    return prev;
}

/*void NiZhiList(struct node *head){
    struct node *p = head;
    struct node *p1 = p->next;
    struct node *p2 = p1->next;

    if(p == NULL || p->next == NULL){
        return;
    }

    while(p2->next == NULL){
        if(p == head){
            p->next = NULL;
            p1->next = p;
            p = p1;
            p2 = p1->next;
        }
        else{
            p1->next = p;
            p = p1;
            p1 = p2;
            p2 = p1->next;
        }
    }
    p1->next = p;
    p2->next = p1;
    head = p2;
    return;
}*/
