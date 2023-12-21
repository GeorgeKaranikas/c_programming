#include <stdio.h>

int main(void){

    struct P {
        int a ;
        char b;
    };
    struct P A ;
    printf("%d",A.a);
    return 0;
}