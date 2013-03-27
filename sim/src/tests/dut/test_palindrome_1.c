#include "common.h"
#include <stdlib.h>
#include <string.h>

int is_palindrome(char*);
 
int main()
{
   char* str_palin[] = { "abcba", "qwertytrewq", "zxcvbvcxz" };
   char* str_nonpalin[] = { "asdfghj", "poqwert" };
 
   for(int i=0; i<(sizeof(str_palin)/sizeof(str_palin[0])); i++){
       if(!is_palindrome(str_palin[i])){
           LOG_FAIL();
       }
   }
   
   for(int i=0; i<(sizeof(str_nonpalin)/sizeof(str_nonpalin[0])); i++){
       if(is_palindrome(str_nonpalin[i])){
           LOG_FAIL();
       }
   }
   
   LOG_PASS();
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
