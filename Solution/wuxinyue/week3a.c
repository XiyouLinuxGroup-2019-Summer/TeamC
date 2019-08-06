#include <stdio.h>
#include <stdlib.h>

#define max 8

int set[max], sum=0;

void show(){
	int i;
	for (i=0; i<max; i++)
		printf("(%d,%d)", i, queen[i]); 
	printf ("\n");
	sum++;
}
int check (int n){
	int i;
	for (i=0; i<n; i++){
		if (set[i] == set[n] || abs(queen[i] - queen[n] == (n-i)))
			return -1;
	}
	retur 0;j
}

