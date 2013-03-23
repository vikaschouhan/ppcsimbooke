#include <stdio.h>
#include "common.h"

typedef float Matrix[2][2];

void MatrixMultiply(Matrix A, Matrix B, Matrix C, int N)
{
    int i, j, k;

    for(i=0; i<N; i++)  /* Initialization */
        for(j=0; j<N; j++)
            C[i][j] = 0.0;

    for(i=0; i<N; i++)
        for(j=0; j<N; j++)
            for(k=0; k<N; k++)
                C[i][j] += A[i][k] * B[k][j];
}

int main( )
{
    Matrix A = { {1.67, 2.23}, {3.98, 4.1234} };
    Matrix C;

    MatrixMultiply(A, A, C, 2);

    float epsilon = 0.0001;

    if(FLOAT_EQ(C[0][0], 11.664300, epsilon) && FLOAT_EQ(C[0][1], 12.919282, epsilon) &&
       FLOAT_EQ(C[1][0], 23.057732, epsilon) && FLOAT_EQ(C[1][1], 25.877828, epsilon))
        LOG_PASS();
    else
        LOG_FAIL();
    return 0;
}
