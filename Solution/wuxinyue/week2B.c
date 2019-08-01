#include<stdio.h>

int main()
{
	int m;
	scanf("%d",&m);
	int flag;
	if ( m == 1 )
	{
		printf("1\n");
		return 0;
	}
	flag = 1 + m/2;
	printf("%d\n",flag);
}
