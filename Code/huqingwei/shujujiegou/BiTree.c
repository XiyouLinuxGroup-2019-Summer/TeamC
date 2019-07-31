#include <stdio.h>
#include <stdlib.h>

typedef char Item;

typedef struct BiTNode{
	Item item;
	struct BiTNode *lchild, *rchild;
}BiTNode, *BiTree;

void CreateBiTree(BiTree *T){
	char ch;
	scanf("%c", &ch);
	
	if(ch == ' ')
		*T = NULL;
	else{
		*T = (BiTNode*)malloc(sizeof(BiTNode));
		(*T)->item = ch;
		CreateBiTree(&(*T)->lchild);
		CreateBiTree(&(*T)->rchild);
	}
}

void InOrderTraverse(BiTree T, int level){
	if(T){
		printf("%c", T->item);
		InOrderTraverse(T->lchild, level+1);
		InOrderTraverse(T->rchild, level+1);
	}
}

void zhongxu(BiTree T, int level){
	if(T){
		zhongxu(T->lchild, level);
		printf("%c", T->item);
		zhongxu(T->rchild, level);
	}
} 

void houxu(BiTree T){
	if(T){
		houxu(T->lchild);
		houxu(T->rchild);
		printf("%c", T->item);
	}
} 

int main()
{
	int level = 1;
	BiTree T = NULL;
	
	CreateBiTree(&T);
	InOrderTraverse(T, level);
	printf("\n");
	zhongxu(T, level);
	printf("\n");
	houxu(T); 
	
	return 0;
} 

