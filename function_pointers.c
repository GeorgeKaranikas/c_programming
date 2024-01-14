#include <stdio.h>

int newFunc ( int i){
    printf("Return %d", i );
    return i;
}

int main (){


    int i = 0 ;

    typedef int (*pFunc1) (int);
    int (*pFunc2) (int);
    int pFunc3 (int);

    //pFunc1 p = newFunc;
    //pFunc2 = newFunc;
    //pFunc3(i); errors out
    // pFunc3 = newFunc ; Errors out
    //p(i);

    return 0;
}


