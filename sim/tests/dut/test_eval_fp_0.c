#include <stdio.h>
#include "common.h"

float Eval(int N)
{
    int i;
    float Sum;

    if(N == 0)
        return 1.0;
    else
    {
        Sum = 0.0;
        for(i=0; i<N; i++)
            Sum += Eval(i);
        return 2.0 * Sum/N + N;
    }
}

int main( )
{
    float val = Eval(10);
    if(FLOAT_EQ(val, 45.437302, 0.0001))
        LOG_PASS();
    else
        LOG_FAIL();
    return 0;
}
