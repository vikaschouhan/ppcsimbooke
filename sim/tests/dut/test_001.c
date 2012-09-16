#include "common.h"

int z = 8;

// Add function
int add(int x, int y){
    return x+y;
}

// Nomain function
int main(){
    int a = 5;
    int b;

    b = add(a, z);
    if(b == 13)
        log_pass();
    else
        log_fail();

    return 0;
}
