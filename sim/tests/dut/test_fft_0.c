/*----------------------------------------------
   fft_test.c - demonstration program for fft.c
  ----------------------------------------------*/

/******************************************************************************
 * This program demonstrates how to use the file fft.c to calculate an FFT    *
 * of given time-domain samples, as well as to calculate an inverse FFT       *
 * (IFFT) of given frequency-domain samples.  First, N complex-valued time-   *
 * domain samples x, in rectangular form (Re x, Im x), are read from a        *
 * specified file; the 2N values are assumed to be separated by whitespace.   *
 * Then, an N-point FFT of these samples is found by calling the function     *
 * fft, thereby yielding N complex-valued frequency-domain samples X in       *
 * rectangular form (Re X, Im X).  Next, an N-point IFFT of these samples is  *
 * is found by calling the function ifft, thereby recovering the original     *
 * samples x.  Finally, the calculated samples X are saved to a specified     *
 * file, if desired.                                                          *
 ******************************************************************************/

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include "fft.c"

int main()
{
  int i;                    /* generic index */
  int N;                    /* number of points in FFT */
  double (*x)[2];           /* pointer to time-domain samples */
  double (*X)[2];           /* pointer to frequency-domain samples */
  double (*Xexp)[2];        /* pointer expected frequency-domain samples */
  double epsilon;           /* tolerance factor */

  /* Initialize parameters */
  N=2;

  /* Check that N = 2^n for some integer n >= 1. */
  if(N >= 2)
    {
      i = N;
      while(i==2*(i/2)) i = i/2;  /* While i is even, factor out a 2. */
    }  /* For N >=2, we now have N = 2^n iff i = 1. */
  if(N < 2 || i != 1)
    {
      printf(", which does not equal 2^n for an integer n >= 1.");
      exit(EXIT_FAILURE);
    }

  /* Allocate time- and frequency-domain memory. */
  x    = malloc(2 * N * sizeof(double));
  X    = malloc(2 * N * sizeof(double));
  Xexp = malloc(2 * N * sizeof(double));

  /* Initialize time domain samples */
  x[0][0] = 1.2 ; x[0][1] = 3.4;
  x[1][0] = 5.6 ; x[1][1] = 0.4;

  /* Initialize freq domain expected samples */
  Xexp[0][0] = 6.8  ; Xexp[0][1] = 3.8 ;
  Xexp[1][0] = -4.4 ; Xexp[1][1] = 3.0 ;

  epsilon = 0.1;

  /* Calculate FFT. */
  fft(N, x, X);

  /* check results */
  for(i=0; i<N; i++){
      if(!(FLOAT_EQ(X[i][0], Xexp[i][0], epsilon)  && FLOAT_EQ(X[i][1], Xexp[i][1], epsilon))){
          LOG_FAIL();
      }
  }

  /* Print time-domain samples and resulting frequency-domain samples. */
#if 0
  printf("\nx(n):");
  for(i=0; i<N; i++) printf("\n   n=%d: %12f %12f", i, x[i][0], x[i][1]);
  printf("\nX(k):");
  for(i=0; i<N; i++) printf("\n   k=%d: %12f %12f", i, X[i][0], X[i][1]);

  printf("\n");
#endif

  /* Free memory. */
  free(x);
  free(X);

  LOG_PASS();

}
