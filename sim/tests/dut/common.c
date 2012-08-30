// Support file
//

extern void* __heap_start;
extern void* __heap_end;
extern void* __heap_size;

// Heap pointers
char* heap_top      = (char*)&__heap_start;
char* heap_bottom   = (char*)&__heap_end;
char* heap_curr_ptr = (char*)&__heap_start;

#define NULL  (0)

// Allocate memory
char *malloc(int size){
    if(heap_curr_ptr + size > heap_bottom){
        return NULL;
    }
    char* ptr = heap_curr_ptr;
    heap_curr_ptr += size;
    return ptr;
}

// Free memory
void mfree(char* ptr, int size){
    if(heap_curr_ptr - size < heap_top){
        heap_curr_ptr = heap_top;
    }
    heap_curr_ptr -= size;
    return;
}
