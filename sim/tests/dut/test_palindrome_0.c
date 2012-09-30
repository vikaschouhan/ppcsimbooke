#include "common.h"
#include <stdlib.h>

int is_palindrome(char*);
 
int main()
{
   char* ptr  = "Vikiv";        // This is not palindrome.
   char* ptr2 = "VikiV";        // This is palindrome.

   if(is_palindrome(ptr)){
       log_fail();              // log fail, since ptr is not palindrome
   }else{
       log_pass();
   }

   if(is_palindrome(ptr2)){
       log_pass();              // log pass, since ptr is palindrome
   }else{
       log_fail();
   }

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
