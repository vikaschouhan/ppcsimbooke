#ifndef _COMMON_H
#define _COMMON_H

void log_pass();
void log_fail();

// Macros for logging pass/fail status
#define LOG_PASS()  log_pass()
#define LOG_FAIL()  log_fail(); return -1

#endif
