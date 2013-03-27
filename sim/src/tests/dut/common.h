#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <math.h>

void log_pass();
void log_fail();

void print_float(float num);

// Macros for logging pass/fail status
#define LOG_PASS()  log_pass()
#define LOG_FAIL()  log_fail(); return -1

#define FLOAT_EQ(a, b, EPSILON)  ((fabs(a-b) < EPSILON) ? 1:0)

#endif
