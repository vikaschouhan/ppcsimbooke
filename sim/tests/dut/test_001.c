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
    unsigned int* result = result_ptr();  // get result area ptr

    b = add(a, z);
    if(b == 13){
        *result = 0x80000001;
        result++;
        *result = 0x900d900d;
    }
    else{
        *result = 0x80000001;
        result++;
        *result = 0xbaadbaad;
    }

    return;
}
