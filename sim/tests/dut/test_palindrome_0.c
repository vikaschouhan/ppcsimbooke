#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int is_palindrome(char*);
 
int main()
{
   char* ptr  = "Vikiv";        // This is not palindrome.
   char* ptr2 = "VikiV";        // This is palindrome.

   if(is_palindrome(ptr)){
       LOG_FAIL();              // log fail, since ptr is not palindrome
   }else{
       LOG_PASS();
   }

   if(is_palindrome(ptr2)){
       LOG_PASS();              // log pass, since ptr is palindrome
   }else{
       LOG_FAIL();
   }

   return 0;
}
 
int is_palindrome(char *str)
{
    int len = strlen(str);
    int i;

    for(i=0; i<len/2; i++){
        if(str[i] != str[len-i-1])
            return 0;
    }

    return 1;
}
