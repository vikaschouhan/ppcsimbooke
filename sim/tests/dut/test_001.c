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
    char* result = result_ptr();  // get result area ptr

    b = add(a, z);
    if(b == 13)
        *((unsigned int*)result) = 0x900d900d;
    else
        *((unsigned int*)result) = 0xbaadbaad;

    return;
}
