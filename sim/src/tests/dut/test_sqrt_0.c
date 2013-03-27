#include "common.h"
#include <math.h>

// main function
int main(){
    double val = sqrt(2);

    // check results
    if(FLOAT_EQ(val, 1.414, 0.001))
        LOG_PASS();
    else
        LOG_FAIL();
    return 0;
}
