#include "common.h"

int z = 8;

// Add function
int add(int x, int y){
    return x+y;
}

// main function
int main(){
    int a = 5;
    int b;

    b = add(a, z);
    if(b == 13)
        LOG_PASS();
    else
        LOG_FAIL();

    return 0;
}
