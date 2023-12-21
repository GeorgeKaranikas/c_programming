#include <stdio.h>

/* Define some example structs to see how the compiler is using padding 
 avoid padding with #pragma pack(1) or

                                struct name {
                                    ...
                                }__attribute__((packed)); */

typedef struct example_struct_a {   // int + char = 4 + 1 = 5
    int a;
    char b;
} example_a ;

typedef struct example_struct_b {   // char + int = 1 + 4 = 5
    char b;
    int a;
} example_b ;

typedef struct example_struct_c {   // short int + char = 2 + 1 = 3 
    short int a;
    char b;
}__attribute__((packed)) example_c;

typedef struct example_struct_d {  // void * + int = 8 + 4 = 12
    void * c ; 
    int a;
} example_d ; 

typedef struct example_struct_e {  //  double + char + double = 8 + 1 + 8 + 1 = 18

    double d;
    char c;
    double f;
    char g;
}  example_e;                       // real ->  8 + 1 + padding(7) + 8 + 1 +padding (7) = 32


void main (void) {

    printf("size of struct a %d\n" , sizeof(example_a));
    printf("size of struct b %d\n" , sizeof(example_b));
    printf("size of struct c %d\n" , sizeof(example_c));
    printf("size of struct d %d\n" , sizeof(example_d));
    printf("size of struct e %d\n" , sizeof(example_e));
    

}