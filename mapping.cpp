#include <stdio.h>
#include <stdlib.h>

typedef struct _mystruct {
    int a;
    char c;
}__attribute__((packed)) mystruct, *pmystruct;


int main (void){

    unsigned char rawdata[] = "abcdefghij" ;

    //if ( (rawdata = fopen("./a.exe","rb")) == NULL){
    //    printf("Error opening the file ");
     //   exit(1);
    //}
    mystruct* pmap ;
    pmap = pmystruct(rawdata);
    printf("%x\n",pmap->a);
    printf("%c\n",pmap->c);
    //fclose(rawdata);
    return 0;

}


