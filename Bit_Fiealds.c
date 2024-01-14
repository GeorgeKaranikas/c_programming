#include <stdio.h>

struct test {
  
    unsigned int x;
   
    long int y : 63;
   
    unsigned int z;
    
};
 
int main()
{
    // Declare a variable t of type struct test
    struct test t;
    // Pointer to unsigned int, pointing to member x
    unsigned int* ptr1 = &t.x;
    // Pointer to unsigned int, pointing to member z
    unsigned int* ptr2 = &t.z;
    void *ptr = &t;
 
    // Print the difference between the two pointers
    printf("%d\n", ptr2 - ptr1);
    printf("sizeof y = %d \n",sizeof(long int));
    printf("%ld\n", sizeof (t));
    printf("%p\n",ptr1);
    printf("%p\n",ptr2);
    printf("%p\n",ptr);

    

 
    return 0;
}


/*#include <stdio.h>
 
// Space optimized representation of the date
struct date {
    // d has value between 0 and 31, so 5 bits
    // are sufficient
    int d : 31;
 
    // m has value between 0 and 15, so 4 bits
    // are sufficient
    int m : 4;
    
};
 
int main()
{
    printf("Size of date is %lu bytes\n",
           sizeof(struct date));
    
    return 0;
}*/