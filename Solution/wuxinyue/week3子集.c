#include <stdio.h>
// 设置一个数组 数组的下标表示集合中的元素 所以数组只用下标为1 2 3的空间
int set[5];
void powerset(int i,int n){
// 当i>n 时 说明集合中所有的元素都做了选择 开始判断
    if (i>n){
        for (int j=1; j<=n; j++){
            // 如果数组中存放的是1 说明当初尝试时 选择取该元素 即对应的数组下标 所以可以输出
            if (set[j] == 1 ) {
                printf("%d ",j);
            } 
        }
        printf("\n");
    }
    else {
        //如果选择要该元素 对应的数组单元中赋值为1 反之 赋值为0 然后继续向下探索
        set[i] = 1;
        powerset(i+1, n);
        set[i] = 0;
        powerset(i+1, n);
    }
}
int main(){
    int n = 3;
    for (int i=0; i<5; i++){
        set[i] = 0;
    }
    powerset(1,n);
    return 0;
}

