#include <stdio.h>

int main(){
    int a = 0;
    int b = 1;
    int c = 2;
    int d = 3;
    
    int r = a & 1;
    printf("%d\n", r);
    r = a & 2;
    printf("%d\n", r);
    r = b & 1;
    printf("%d\n", r);
    r = b & 2;
    printf("%d\n", r);
    r = c & 1;
    printf("%d\n", r);
    r = c & 2;
    printf("%d\n", r);
    r = d & 1;
    printf("%d\n", r);
    r = d & 2;
    printf("%d\n", r);
    return 0;
}
