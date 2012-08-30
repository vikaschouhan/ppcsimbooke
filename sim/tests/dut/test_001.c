#include "common.h"

int z = 8;

// Add function
int add(int x, int y){
    return x+y;
}

// Nomain function
void nomain(){
    int a = 5;
    int b;
    char* result = malloc(4);   // Allocate memory

    b = add(a, z);
    if(b == 13)
        *((unsigned int*)result) = 0x900d900d;
    else
        *((unsigned int*)result) = 0xbaadbaad;

    mfree(result, 4);    // free memory
    return;
}
