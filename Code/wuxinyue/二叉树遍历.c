#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tree
{
    int num;
    struct tree *left,*right;
};
// 先序 根左右
void a(struct tree *t)
{
    if(t == NULL)
        return;
    printf("%d\t",t->num);
    a(t->left);
    a(t->right);
}

// 中序  左根右
void b(struct tree *t)
{
    if(t == NULL)
        return;
    b(t->left);
    printf("%d\t",t->num);
    b(t->right);
}
// 后序  左右根
void c(struct tree *t)
{
    if(t == NULL)
        return;
    c(t->left);
    c(t->right);
    printf("%d\t",t->num);
}

