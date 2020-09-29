#include <stdio.h>
void swap(int *x, int *y){
    int tmp;
    tmp = *x; *x = *y; *y = tmp;
}

int main(){
    int a = 5, b = 4;
    swap(&a, &b);
    printf("%d %d", a,b);
}