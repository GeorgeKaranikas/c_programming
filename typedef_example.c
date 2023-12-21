#include <stdio.h>



typedef struct newstruct
{
    int num1;
    int num2;
    char alpha;
    float exp;

}MYSTRUCT,*PMYSTRUCT;

void main (void){

    int *integer;

    int i = 1;

    integer = &i;

    printf("%d\n",*integer); 

    printf ("%p \n",&(((MYSTRUCT*)0)->alpha));
}