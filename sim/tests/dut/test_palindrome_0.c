#include "common.h"
#include <stdlib.h>

int is_palindrome(char*);
void copy_string(char*, char*);
void reverse_string(char*);
int string_length(char*);
int compare_string(char*, char*);
 
int main()
{
   char* str_palin[] = { "abcbc", "qwertytrewq", "zxcvbvcxz" };
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
 
int is_palindrome(char *string)
{
   int check, length;
   char *reverse;
 
   length = string_length(string);    
   reverse = (char*)malloc(length+1);    
 
   copy_string(reverse, string);
   reverse_string(reverse);
 
   check = compare_string(string, reverse);
 
   free(reverse);
 
   if ( check == 0 )
      return 1;
   else
      return 0;
}
 
int string_length(char *string)
{
   int length = 0;  
 
   while(*string)
   {
      length++;
      string++;
   }
 
   return length;
}
 
void copy_string(char *target, char *source)
{
   while(*source)
   {
      *target = *source;
      source++;
      target++;
   }
   *target = '\0';
}
 
void reverse_string(char *string) 
{
   int length, c;
   char *begin, *end, temp;
 
   length = string_length(string);
 
   begin = string;
   end = string;
 
   for ( c = 0 ; c < ( length - 1 ) ; c++ )
       end++;
 
   for ( c = 0 ; c < length/2 ; c++ ) 
   {        
      temp = *end;
      *end = *begin;
      *begin = temp;
 
      begin++;
      end--;
   }
}
 
int compare_string(char *first, char *second)
{
   while(*first==*second)
   {
      if ( *first == '\0' || *second == '\0' )
         break;
 
      first++;
      second++;
   }
   if( *first == '\0' && *second == '\0' )
      return 0;
   else
      return -1;
}
