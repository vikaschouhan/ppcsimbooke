// Support file
//
#include <stdio.h>

extern void* __heap_start;
extern void* __heap_end;
extern void* __heap_size;
extern void* __result_start;
extern void* __trace_start;
extern void* __trace_size;
extern void* __trace_end;

// Heap pointers
char* heap_top       = (char*)&__heap_start;
char* heap_bottom    = (char*)&__heap_end;
char* res_start_ptr  = (char*)&__result_start;
char* res_curr_ptr   = (char*)&__result_start;

// trace region pointers
char* trace_top      = (char*)&__trace_start;
char* trace_bottom   = (char*)&__trace_end;

#ifndef NULL
#define NULL  (0)
#endif

// log pass status
void log_pass(){
    unsigned int* ptr = (unsigned int*)res_start_ptr;
    *ptr = 0x80000001;
    ptr++;
    *ptr = 0x900d900d;
    ptr++;
}

void log_fail(){
    unsigned int* ptr = (unsigned int*)res_start_ptr;
    *ptr = 0x80000001;
    ptr++;
    *ptr = 0xbaadbaad;
    ptr++;
}

// Add custom user defined functions


void print_float(float num){
    char c,sign, tmp[66];
    unsigned char* str = tmp;
    unsigned int c0;
    unsigned long long expnt = 10;
    int precision = 10;
    int l;
    float r;
    int i;

    const char *digits="0123456789";

    for(i=0; i<precision; i++){
        expnt *= 10;
    }

    /* calculate left and right parts */
    i = 0;
    l = (int)num;
    r = num - (float)l;

    if(l < 0){
        l = -l;
        //printf("-");
        printf("minus ");
    }

    printf("%u", l);

    *str++ =  '.';
    for(i=0; i<precision; i++){
        c0 = (unsigned int)(r * 10);
        if(c0 < 0 || c0 > 9)
            *str++ = '0';
        else
            *str++ = digits[c0];
        r = r*10 - (unsigned int)c0;
    }

    printf("%s\n", tmp);
}
