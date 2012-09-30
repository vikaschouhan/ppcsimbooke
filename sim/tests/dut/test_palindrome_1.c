#include "common.h"
#include <stdlib.h>

int is_palindrome(char*);
 
int main()
{
   char* str_palin[] = { "abcba", "qwertytrewq", "zxcvbvcxz" };
   char* str_nonpalin[] = { "asdfghj", "poqwert" };
 
   for(int i=0; i<(sizeof(str_palin)/sizeof(char*)); i++){
       if(!is_palindrome(str_palin[i])){
           log_fail();
           return -1;
       }
   }

   for(int i=0; i<(sizeof(str_nonpalin)/sizeof(char*)); i++){
       if(is_palindrome(str_nonpalin[i])){
           log_fail();
           return -1;
       }
   }
   
   log_pass();

   return 0;
}
 
int is_palindrome(char *str)
{
    int len = 5;
    int i;

    for(i=0; i<len/2; i++){
        if(str[i] != str[len-i-1])
            return 0;
    }

    return 1;
}
