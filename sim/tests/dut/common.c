// Support file
//

extern void* __heap_start;
extern void* __heap_end;
extern void* __heap_size;
extern void* __result_start;

// Heap pointers
char* heap_top       = (char*)&__heap_start;
char* heap_bottom    = (char*)&__heap_end;
char* res_start_ptr  = (char*)&__result_start;
char* res_curr_ptr   = (char*)&__result_start;

#define NULL  (0)

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
