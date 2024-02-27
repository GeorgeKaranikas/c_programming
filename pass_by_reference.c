#include <stdio.h>
#include <windows.h>


int Func(_Inout_ PDWORD dwvar){
    //does something to dword passed by reference 
    printf("dwvar is at %p in Func1 and holds value %d \n ",dwvar,*dwvar);
    *dwvar = 2;
    printf("dwvar changed to %d and is in addr %p \n",dwvar,*dwvar);
    return 0;
}


int main (){

    DWORD var1 ;
    printf("var1 in main in address %p and it has value %d \n",&var1,var1);
    Func(&var1);
    printf("back to main var1 is %d\n",var1);
    return 0;
}