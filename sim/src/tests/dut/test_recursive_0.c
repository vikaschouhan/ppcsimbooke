#include "common.h"
#include <stdio.h>

int F(int X)
{
    if(X == 0)
        return 0;
    else
        return 2 * F(X - 1) + X*X;
}

int main( )
{
    if(F(5) == 141)
        LOG_PASS();
    else
        LOG_FAIL();
    return 0;
}
