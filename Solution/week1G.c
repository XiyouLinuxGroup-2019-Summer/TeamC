#include <stdio.h>
int k,c;
int area(int num)
{
	int sum=num;
	int a = k;
	while(num/a != 0 && sum < c)
	{
		sum += num/a;
		a *= k;
	}
	if(sum >= c)
		return num;
	else
		return 0;
}
int main()
{
	int t;
	int mid;
	scanf("%d %d",&c,&k);
	int n = c;
	int i = 0;
	while(i <= n)
	{
		mid = (i+n)/2;
		t = area(mid);
		if(t>0)
			n = mid - 1;
		else
			i = mid + 1;
	}
	if(t==0)
		printf("%d\n",mid+1);
	else
		printf("%d\n",mid);
}

