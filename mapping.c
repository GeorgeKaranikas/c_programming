#include <stdio.h>

typedef struct _mystruct {
    int a;
    char c;
} __attribute__((packed)) mystruct, *pmystruct;

int main(void) {
    long rawdata = 1234566;
    printf("%x\n",rawdata);
    unsigned char rawdata2[] = "abcdefghij" ;
    printf("%x\n",*rawdata2);
    mystruct* pmap;
    printf("%p\n",pmap);
    pmap = (pmystruct)(&rawdata);
    printf("%d\n", pmap->a);
    printf("%c\n", pmap->c); //returns nothing
    pmap = (pmystruct)rawdata2;
    printf("%x\n", pmap->a);
    printf("%p\n", &rawdata2);
    //printf("%d\n", pmap->a);
    //printf("%c\n", pmap->c);
    
    return 0;
}