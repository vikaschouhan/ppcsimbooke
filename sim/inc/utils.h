/*
  utils.h ( utility functions/macros )
  This file contains utility functions/macros which are very general
  in nature and can be used anywhere.

  NOTE: All functions are inline functions

  Author : Vikas Chouhan ( presentisgood@gmail.com )
  Copyright 2012.

  This file is part of ppc-sim library bundled with test_gen_ppc.

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  It is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with this file; see the file COPYING.  If not, write to the
  Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
  MA 02110-1301, USA.
*/
#ifndef _UTILS_H_
#define _UTILS_H_

#include <cmath>
#include <sstream>
#include <iostream>
#include "log.hpp"

// Cast to appropriate types
#define u32(arg)  ((uint32_t)(arg))
#define s32(arg)  ((int32_t)(arg))
#define u64(arg)  ((uint64_t)(arg))
#define s64(arg)  ((int64_t)(arg))

/* Assertion macro which takes an expression and a return value.
 * It checks the statement and if false, returns the return value.
 */
// C like return assertion
#define assert_ret(x, retv)    if(!(x)){ std::cout << #x << " failed in " << __FILE__ << " at " << __LINE__ << std::endl; return (retv); }
// C++ specific exception assertion
#define assert_and_throw(x, expv) if(!(x)){ std::cout << #x << "failed in " << __FILE__ << " at " << __LINE__ << std::endl; throw expv; }

#define log2(x) static_cast<unsigned>(log2(static_cast<double>(static_cast<unsigned>(x))))
#define log4(x) static_cast<unsigned>(log2(static_cast<double>(static_cast<unsigned>(x)))/2)
#define pow4(x) ((1 << static_cast<unsigned>(x)) << static_cast<unsigned>(x))
#define pow2(x) (1 << static_cast<unsigned>(x))

/* Check if number is power of 4 */
#define IF_POWER_OF_4(x) ((!(x))?1:((x) == pow4(log4(x))))
#define if_pow_4(x) IF_POWER_OF_4(x)
/* Check if a number if power of 2 (fast algorithm) */
#define IF_POWER_OF_2(x) ((!(x))?1:(((x) & ((x)-1)) == 0))
#define if_pow_2(x) IF_POWER_OF_2(x)

// Min. page size
#define MIN_PGSZ  (0x1000)
#define MIN_PGSZ_SHIFT (12)

// Round x to next higher page size ( booke )
#define rnd_booke_pgsz_h(x) (((x)<MIN_PGSZ)?MIN_PGSZ:((if_pow_4(x))?(x):(MIN_PGSZ*pow4(log4((x)/MIN_PGSZ)+1))))
// Round x to previous lower page size ( booke )
#define rnd_booke_pgsz_l(x) (((x)<MIN_PGSZ)?MIN_PGSZ:((if_pow_4(x))?(x):(MIN_PGSZ*pow4(log4((x)/MIN_PGSZ)))))

// Define appropriate macros
#ifdef CONFIG_BOOKE
#define rnd_pgsz_h(x) rnd_booke_pgszh(x)
#define rnd_pgsz_l(x) rnd_booke_pgszl(x)
#else
#define rnd_pgsz_h(x)
#define rnd_pgsz_l(x)
#endif

// Round x to next & previous page sizes
#define rnd_pgsz_p(x) ((x) & ~(MIN_PGSZ-1))
#define rnd_pgsz_n(x) ((rnd_pgsz_p(x) == (x)) ? (x) : (rnd_pgsz_p(x)+MIN_PGSZ))
#define rnd_pgsz_n_plus_1(x) (((rnd_pgsz_p(x) == (x)) ? (x) : rnd_pgsz_p(x)) + MIN_PGSZ)

//min / max
#define min(x, y) (((x) < (y))?(x):(y))
#define max(x, y) (((x) > (y))?(x):(y))

#define BAR0 "------------------------------------------------------------------------"

// Messages for logging purposes
// These print the beginning and end of function calls
// NOTE:  These require GCC compiler to work properly
#define MSG_FUNC_START "Entering " << __PRETTY_FUNCTION__ << " ."
#define MSG_FUNC_END   "Exiting " << __PRETTY_FUNCTION__ << " ."

// Macro for ensuring that function end message is always printed even in case of abrubt returns
// Void functions don't allow return values
#define RETURN(retval, DEBUGLEVEL)       LOG(#DEBUGLEVEL) << MSG_FUNC_END; return (retval)
#define RETURNVOID(DEBUGLEVEL)           LOG(#DEBUGLEVEL) << MSG_FUNC_END; return

// Convert a string to an integer
// We don't support octal and binary ints for time being
inline int64_t str_to_int(std::string arg){
    std::istringstream istr;
    istr.str(arg);
    int64_t value;
    if(arg.substr(0,2) == "0x" || arg.substr(0,2) == "0X" ||
            arg.substr(1,2) == "0x" || arg.substr(1,2) == "0X")
        istr >> std::hex >> value;
    else if(arg.substr(0,2) == "0b" || arg.substr(0,2) == "0B" ||
            arg.substr(1,2) == "0b" || arg.substr(1,2) == "0B")
        //istr >> std::bin >> value;
        // there is no std::bin so we will have to implement it
        // overselves
        ;
    else
        istr >> std::dec >> value;
    return value;
}

// Add and subtract are essentially same, so only add can be used for both.
// Check overflows
#define check_add_of_s32(arg0, arg1)      (s32(arg0) > INT32_MAX  - s32(arg1))?1:0
#define check_add_of_s64(arg0, arg1)      (s64(arg0) > INT64_MAX  - s64(arg1))?1:0
#define check_add_of_u32(arg0, arg1)      (u32(arg0) > UINT32_MAX - u32(arg1))?1:0
#define check_add_of_u64(arg0, arg1)      (u64(arg0) > UINT64_MAX - u64(arg1))?1:0

// Check underflows
#define check_add_uf_s32(arg0, arg1)      (s32(arg0) < INT32_MIN  - s32(arg1))?1:0
#define check_add_uf_s64(arg0, arg1)      (s64(arg0) < INT64_MIN  - s64(arg1))?1:0
#define check_add_uf_u32(arg0, arg1)      (u32(arg0) < 0          - u32(arg1))?1:0
#define check_add_uf_u64(arg0, arg1)      (u64(arg0) < 0          - u64(arg1))?1:0


#endif
