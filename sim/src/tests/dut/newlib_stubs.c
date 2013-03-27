/*
 * newlib_stubs.c
 */
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#undef errno
extern int errno;

/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };

void exit(int status) {
    write(1, "exit", 4);
    while (1) {
        ;
    }
}

int close(int file) {
    return -1;
}
/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
int execve(const char *name, char * const argv[], char * const env[]) {
    errno = ENOMEM;
    return -1;
}
/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */

int fork() {
    errno = EAGAIN;
    return -1;
}
/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int getpid() {
    return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int isatty(int file) {
    switch (file){
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}


/*
 kill
 Send a signal. Minimal implementation:
 */
int kill(int pid, int sig) {
    errno = EINVAL;
    return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */

int link(const char *old, const char *new) {
    errno = EMLINK;
    return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
_off_t lseek(int file, _off_t ptr, int dir) {
    return 0;
}

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
char* heap_end = 0;
void* sbrk(ptrdiff_t incr) {
    extern char* heap_top;
    extern char* heap_bottom;
    char *prev_heap_end;
    
    if (heap_end == 0) {
     heap_end = heap_top;
    }
    prev_heap_end = heap_end;
    
    if (heap_end + incr > heap_bottom) {
        return (caddr_t)0;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */


int read(int file, void *__ptr, size_t len) {
    int n;
    int num = 0;
    char* ptr = __ptr;
    switch (file) {
    case STDIN_FILENO:
        for (n = 0; n < len; n++) {
            /* Empty */
            *ptr++ = '0';
            num++;
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */

int stat(const char *filepath, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */

clock_t times(struct tms *buf) {
    return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
int unlink(const char *name) {
    errno = ENOENT;
    return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
int wait(int *status) {
    errno = ECHILD;
    return -1;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent

 NOTE: for now, write() writes to memory's trace area
*/
int write(int file, const void *ptr, size_t len) {
    extern char* trace_top;
    extern char* trace_bottom;
    static char* trace_ptr = 0;

    if(trace_ptr == 0){
        trace_ptr = trace_top;
    }

    if(trace_ptr + len > trace_bottom){
        trace_ptr = trace_top;
    }

    int n;
    switch (file) {
    case STDOUT_FILENO: /*stdout*/
    case STDERR_FILENO: /* stderr */
        for (n = 0; n < len; n++) {
            *trace_ptr++ = *(unsigned char *)ptr++;
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}

