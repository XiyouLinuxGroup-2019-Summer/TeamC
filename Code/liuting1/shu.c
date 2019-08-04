/*************************************************************************
	> File Name: shu.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月03日 星期六 09时32分26秒
 ************************************************************************/

#include<stdio.h>


//前序遍历
void Pre_tree(Bitree T)
{
    if (T == NULL)
    {
        return ;
    }

    printf ("%c",T->data);
    Pre_tree (T->lchild);
    Pre_tree (T->rchild);
}

//中序遍历
void Pre_tree(Bitree T)
{
    if (T == NULL)
    {
        return ;
    }

    Pre_tree (T->lchild);
    printf ("%c",T->data);
    Pre_tree (T->rchild);
}

//后序遍历
void Pre_tree(Bitree T)
{
    if (T == NULL)
    {
        return ;
    }

    Pre_tree (T->lchild);
    Pre_tree (T->rchild);
    printf ("%c",T->data);
}

