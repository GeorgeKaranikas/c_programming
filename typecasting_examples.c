#include <stdio.h>
#include <string.h>

/* void main (){
    
    int int_array[5]={1,2,3,4,5};
    char chr_array[5]={'a','b','c','d','e'};

    int* int_ptr;
    char* chr_ptr;

    int_ptr=int_array;
    chr_ptr=chr_array;

    for (int i=0; i<5 ; i++ ){
        printf("int number %d in addr 0x%x \n",*int_ptr, int_ptr );
        int_ptr=int_ptr + 1;
        printf("char %c in addr 0x%x \n",*chr_ptr , chr_ptr );
        chr_ptr=chr_ptr + 1;
    }
} */

/*int main (){
    
    int int_array[5]={1,2,3,4,5};
    char chr_array[5]={'a','b','c','d','e'};

    int* int_ptr;
    char* chr_ptr;

    chr_ptr = int_array ;
    int_ptr = chr_array ;

    for (int i=0 ; i<5; i++){
        printf("int number %d in addr %p \n", *((int*) chr_ptr) , chr_ptr );
        chr_ptr = (int*) chr_ptr + 1;
    }

    for (int i=0 ; i<5; i++){
        printf("char %c in addr %p \n", *((char*) int_ptr) , int_ptr );
        int_ptr = (char*) int_ptr + 1;
    }
} */


int main (int argc ,char argv[]){
        
    int int_array[5]={1,2,3,4,5};
    char chr_array[5]={'a','b','c','d','e'};

    unsigned long int strange_ptr ; //using an usigned integer because a pointer is binary data after all
    int* pointer;

    printf("size of unsigned long integer is %d \n",sizeof(unsigned long int));
    printf("size of a pointer is %d \n", sizeof(pointer));

    strange_ptr = (unsigned long int) int_array;
    for (int i=0 ; i<5 ; i++){
        printf("int value is %d in adrr %p \n" , *((unsigned long int*) strange_ptr), strange_ptr);
        /*strange_ptr =  (int*) strange_ptr + 1; */ //it`ll complain at complilation time
        strange_ptr = strange_ptr + sizeof(int);
    }

}


/*int main (){

    int int_array[5]={1,2,3,4,5};
    char chr_array[5]={'a','b','c','d','e'};

    void* void_ptr ; //using a void type pointer

    void_ptr = int_array;
    for (int i=0 ; i<5 ; i++){
        printf("int value is %d in adrr %p \n" , *((int*) void_ptr), void_ptr);
        void_ptr = (int*) void_ptr + 1;
    }

    void_ptr = chr_array ;
    for (int i=0 ; i<5 ; i++){
        printf("char value is %c in adrr %p \n" , *((char*) void_ptr), void_ptr);
        void_ptr = (char*) void_ptr + 1;
    }
    
}*/