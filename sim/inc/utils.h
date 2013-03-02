/*
  utils.h (utility functions/macros)
  This file contains utility functions/macros which are very general
  in nature and can be used anywhere.

  NOTE: All functions are inline functions

  Author : Vikas Chouhan (presentisgood@gmail.com)
  Copyright 2012.

  This file is part of ppc-sim-booke simulator library.

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
#include <cstring>
#include "log.hpp"

// endianness consts
static const int  EMUL_BIG_ENDIAN        =  0;
static const int  EMUL_LITTLE_ENDIAN     =  1;

// Used for union'ed bitfields
#if (defined __x86_64__)
#define brev(a, b) b a
#else
#define brev(a, b) a b
#endif

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
#define assert_and_throw(x, expv) if(!(x)){ std::cout << #x << " failed in " << __FILE__ << " at " << __LINE__ << std::endl; throw expv; }


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
#define MSG_FUNC_START "Entering " << __PRETTY_FUNCTION__ << " ." << std::endl
#define MSG_FUNC_END   "Exiting " << __PRETTY_FUNCTION__ << " ." << std::endl

// Macro for ensuring that function end message is always printed even in case of abrubt returns
// Void functions don't allow return values
#define RETURN(retval, DEBUGLEVEL)       LOG(#DEBUGLEVEL) << MSG_FUNC_END; return (retval)
#define RETURNVOID(DEBUGLEVEL)           LOG(#DEBUGLEVEL) << MSG_FUNC_END; return

// Log assert and return
#define LASSERT_RET(x, retv, LEVEL)   if(!(x)){                                                                                               \
                                          std::cout << #x << " failed in " << __FILE__ << " at " << __LINE__ << std::endl;                    \
                                          LOG(#LEVEL) << MSG_FUNC_END;                                                                        \
                                          return (retv);                                                                                      \
                                      }
// Log assert and throw
#define LASSERT_THROW(x, expv, LEVEL) if(!(x)){                                                                                               \
                                          std::cout << #x << " failed in " << __FILE__ << " at " << __LINE__ << std::endl;                    \
                                          LOG(#LEVEL) << MSG_FUNC_END;                                                                        \
                                          throw expv;                                                                                         \
                                      }
#define LTHROW(expv, LEVEL)           {                                                                                                       \
                                          LOG(#LEVEL) << MSG_FUNC_END;                                                                        \
                                          throw expv;                                                                                         \
                                      } 

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

// Get host endianness
inline int host_endianness(){
    unsigned int __v0 =  0x1;
    unsigned char *__ptr0 = reinterpret_cast<unsigned char*>(&__v0);
    if(*__ptr0 == __v0)
        return EMUL_LITTLE_ENDIAN;
    else
        return EMUL_BIG_ENDIAN;
}

// Remove all spaces from str
inline int prune_all_spaces(char *str){
    char tmpstr[100];
    int i;
    int ctr = 0;
    if(str == NULL)
        return -1;

    for(i=0; str[i] != '\0'; i++){
        if(str[i] != ' '){ tmpstr[ctr++] = str[i]; }
    }
    tmpstr[ctr++] = '\0';
    strncpy(str, tmpstr, ctr);
    return 1;
}

// find pointer to first instance of token and return the remaining string ( don't remove the delimiting character )
inline char *find_tok(char **str, char delim){
    static char tmp_str[100];
    char *tmp_str_ptr = tmp_str;
    for(; ((**str != delim) && (**str != '\0'));){
        *tmp_str_ptr++ = *(*str)++;
    }
    *tmp_str_ptr = '\0';
    return tmp_str;
}

// Get left shift values for a bitfield mask
// x is the mask
template <uint64_t x> struct RSHIFT{
    enum {
        VAL = (((x & 0x1) ? 0:1) + RSHIFT< ((x & 0x1) ? 0:(x>>1)) >::VAL),
    };
};

template <> struct RSHIFT<0>{
    enum {
        VAL = 0,
    };
};

// Run time variant
template<typename T>
inline int rshift(T x){
    int res = 0;
    while(!(x & 0x1)){ res++; x>>=1; }
    return res;
}

// bitfied insertion
template<typename T>
inline T ins_bf(T src, T val, T mask){
    src |= (val << rshift<T>(mask)) & mask;
    return src;
}

// bitfield extraction
template<typename T>
inline T extr_bf(T src, T mask){
    return (src & mask) >> rshift<T>(mask);
}

// extract bit (Big endian PowerPC notation)
template<typename T>
inline bool extr_bit(T src, int bp){
    return (src >> (sizeof(T)*8 - 1 - bp)) & 0x1;
}

// generate bitmask starting from bit position bp to last lsb (Big Endian PowerPC notation)
// NOTE : As a special case, this function can take bp=sizeof(T)*8
//        (remember gen_bmask_rng calling this fn with args (x+1)/(y+1) !!!)
template<typename T>
inline T gen_bmask(int bp){
    if(bp){
        return (1ULL << (sizeof(T)*8 - bp)) - 1;
    }else
        return 0xffffffffffffffffULL;
}

// generate bitmask from bitpos x to bitpos y (Big Endian PowerPC notation)
template<typename T>
inline T gen_bmask_rng(int x, int y){
    return (x < y) ? (gen_bmask<T>(x) ^ gen_bmask<T>(y+1)) : ~(gen_bmask<T>(x+1) ^ gen_bmask<T>(y));
}

// typesafe sign extension function
template<typename T_tgt, typename T_src>
inline T_tgt sign_exts(T_src x){ return static_cast<T_tgt>(x); }

// sign extension of a subfield of passed number with MSB=x, LSB=y to a T type
// sgn = 1 for sign extension
// sgn = 0 for zero extension
template<typename T, bool sgn=1>
inline T sign_exts_f(T n, int x, int y){
    T mask     = gen_bmask_rng<T>(x, y);
    bool sgn_b = extr_bit<T>(n, x);
    int rshft  = rshift<T>(mask);
    T mask_s   = ~((1ULL << (y - x + 1)) - 1);
    T val      = ((n & mask) >> rshft);
    val        = (sgn_b & sgn) ? (val | mask_s):val;
    return val;
}

// rotate left
// NOTE : In Power ISA , rotl64 is defined as rotation of 64 bit "n" by x bits
//        & rotl32 is defined as rotation of (n | n) where "n" is a 32 bit no
//        by x bits ( | is concatenation operator). In my opinion, below generic
//        function works exactly the same.
template<typename T>
inline T rotl(T n, int x){
    n = (n << x) | (n >> (sizeof(T)*8 - x));
    return n;
}


// bit field extraction / insertion macros (64 bit std)
#define  EBF(src, mask)                  extr_bf<uint64_t>(src, mask)        // extract Bit field
#define  IBF(src, val, mask)             ins_bf<uint64_t>(src, val, mask)    // Insert Bit field

// Concatenating macros
#define CAT2(x,y)           x##y
#define CAT3(x,y,z)         x##y##z
#define CAT(x,y)            CAT2(x,y)

//---- would only work on little endian x86 machines
#if defined __ORDER_LITTLE_ENDIAN__
template<typename T>
inline T read_buff(uint8_t *buff, int endianness){
    T n = *(reinterpret_cast<T *>(buff));
    if(endianness == EMUL_BIG_ENDIAN)  asm("bswap %[n]" : [n] "+q" (n)::);
    return n;
}
template<typename T>
inline void write_buff(uint8_t *buff, T value, int endianness){
    if(endianness == EMUL_BIG_ENDIAN)
        asm("bswap %[value]" : [value] "+q" (value)::);
    *(reinterpret_cast<T *>(buff)) = value;
}
#ifndef __x86_64__
template<>
inline uint64_t read_buff<uint64_t>(uint8_t *buff, int endianness){
    if(endianness == EMUL_BIG_ENDIAN)
        return  (static_cast<uint64_t>(buff[0]) << 56) |
                (static_cast<uint64_t>(buff[1]) << 48) |
                (static_cast<uint64_t>(buff[2]) << 40) |
                (static_cast<uint64_t>(buff[3]) << 32) |
                (static_cast<uint64_t>(buff[4]) << 24) |
                (static_cast<uint64_t>(buff[5]) << 16) |
                (static_cast<uint64_t>(buff[6]) <<  8) |
                (static_cast<uint64_t>(buff[7]) <<  0) ;
    else
        return *(reinterpret_cast<uint64_t *>(buff));
}
template<>
inline void write_buff<uint64_t>(uint8_t *buff, uint64_t value, int endianness){
    if(endianness == EMUL_BUG_ENDIAN){
        buff[0] = (value >> 56);
        buff[1] = (value >> 48);
        buff[2] = (value >> 40);
        buff[3] = (value >> 32);
        buff[4] = (value >> 24);
        buff[5] = (value >> 16);
        buff[6] = (value >>  8);
        buff[7] = (value >>  0);
    }else{
        *(reinterpret_cast<uint64_t *>(buff)) = value;
    }
}
#endif
template<>
inline uint16_t read_buff<uint16_t>(uint8_t *buff, int endianness){
    if(endianness == EMUL_BIG_ENDIAN)
        return (static_cast<uint16_t>(buff[0]) << 8) |
               (static_cast<uint16_t>(buff[1]) << 0) ;
    else
        return *(reinterpret_cast<uint16_t *>(buff));
}
template<>
void write_buff<uint16_t>(uint8_t* buff, uint16_t value, int endianness){
    if(endianness == EMUL_BIG_ENDIAN){
        buff[0] = (value >> 8);
        buff[1] = (value >> 0);
    }else{
        *(reinterpret_cast<uint16_t *>(buff)) = value;
    }
}
#else
#error "Machine should be Little Endian x86/x86_64"
#endif


// ----------------------------------------- x86 fncs returning flags----------------------------------------
// x86 functions
struct x86_flags {
    bool cf;   // carry flag
    bool zf;   // zero flag
    bool sf;   // sign
    bool of;   // overflow

    x86_flags() : cf(false), zf(false), sf(false), of(false) {}
};
std::ostream& operator<<(std::ostream& ostr, x86_flags f){
    ostr << ": cf=" << f.cf << ", zf=" << f.zf << ", sf=" << f.sf << ", of=" << f.of << " ";
    return ostr;
}

#define def_x86_alu_op1(name, x86_op, zaps, oc)                          \
template<typename T>                                                     \
inline T name(T ra, x86_flags& f){                                       \
    x86_flags f0;                                                        \
    asm(                                                                 \
            #x86_op " %[ra]; setc %[cf]; seto %[of];"                    \
            "sets %[sf]; setz %[zf];"                                    \
            : [ra] "+a" (ra), [cf] "=q" (f0.cf), [of] "=q" (f0.of),      \
              [zf] "=q" (f0.zf), [sf] "=q" (f0.sf)                       \
            :                                                            \
            :                                                            \
       );                                                                \
    if(zaps & oc) { f=f0; }                                              \
    else if(zaps) { f.zf=f0.zf; f.sf=f0.sf; }                            \
    else if(oc)   { f.cf=f0.cf; f.of=f0.of; }                            \
    return ra;                                                           \
}

#define def_x86_alu_op2(name, x86_op, zaps, oc)                          \
template<typename T>                                                     \
inline T name(T ra, T rb, x86_flags& f){                                 \
    x86_flags f0;                                                        \
    asm(                                                                 \
            #x86_op " %[rb], %[ra]; setc %[cf]; seto %[of];"             \
            "sets %[sf]; setz %[zf];"                                    \
            : [ra] "+q" (ra), [cf] "=q" (f0.cf), [of] "=q" (f0.of),      \
              [zf] "=q" (f0.zf), [sf] "=q" (f0.sf)                       \
            : [rb] "q" (rb)                                              \
            :                                                            \
       );                                                                \
    if(zaps & oc) { f=f0; }                                              \
    else if(zaps) { f.zf=f0.zf; f.sf=f0.sf; }                            \
    else if(oc)   { f.cf=f0.cf; f.of=f0.of; }                            \
    return ra;                                                           \
}

// All flags are set for neg, add & sub (i.e zaps=oc=true)
// only zaps are set for logical instrs (and, or, xor etc)
def_x86_alu_op1(x86_neg, neg, true, true)
def_x86_alu_op2(x86_add, add, true, true)
def_x86_alu_op2(x86_sub, sub, true, true)
def_x86_alu_op2(x86_and, and, true, false)
def_x86_alu_op2(x86_or,  or,  true, false)
def_x86_alu_op2(x86_xor, xor, true, false)


#define def_x86_mul_op(name, x86_op)                                    \
template<typename T>                                                    \
inline T name(T ra, T rb, x86_flags& f, bool high=0){                   \
    register T rd = 0;                                                  \
    asm(                                                                \
            #x86_op " %[rb]; setc %[cf]; seto %[of];"                   \
            : "+a" (ra), "=d" (rd), [cf] "=q" (f.cf), [of] "=q" (f.of)  \
            : [rb] "q" (rb)                                             \
            :                                                           \
       );                                                               \
    return (high) ? rd:ra;                                              \
}

def_x86_mul_op(x86_mul, mul)
def_x86_mul_op(x86_imul, imul)

#define def_x86_mulf_op(name, x86_op)                                   \
template<typename D, typename S>                                        \
inline D name(S ra, S rb, x86_flags& f){                                \
    register S rd = 0;                                                  \
    asm(                                                                \
            #x86_op " %[rb]; setc %[cf]; seto %[of];"                   \
            : "+a" (ra), "=d" (rd), [cf] "=q" (f.cf), [of] "=q" (f.of)  \
            : [rb] "q" (rb)                                             \
            :                                                           \
       );                                                               \
    return (D(rd) << sizeof(S)*8) | D(ra);                              \
}

def_x86_mulf_op(x86_mulf, mul)
def_x86_mulf_op(x86_imulf, imul)

#define def_x86_div_op(name, x86_op)                                    \
template<typename T>                                                    \
inline T name(T ra, T rb, bool rem=0){                                  \
    register T rd = 0;                                                  \
    asm(                                                                \
            #x86_op " %[rb];"                                           \
            : "+a" (ra), "=d" (rd)                                      \
            : [rb] "q" (rb)                                             \
            :                                                           \
       );                                                               \
    return (rem) ? rd:ra;                                               \
}

def_x86_div_op(x86_div, div)
def_x86_div_op(x86_idiv, idiv)

#define NEGW              x86_neg<uint32_t>
#define ADDW              x86_add<int32_t>
#define SUBW              x86_sub<int32_t>
#define MULW              x86_imul<int32_t>
#define MULUW             x86_mul<uint32_t>
#define MULWF             x86_imulf<int64_t, int32_t>
#define MULUWF            x86_mulf<uint64_t, uint32_t>
#define DIVW              x86_idiv<int32_t>
#define DIVUW             x86_div<uint32_t>
#define ANDW              x86_and<uint32_t>
#define ORW               x86_or<uint32_t>
#define XORW              x86_xor<uint32_t>

#define ADD64             x86_add<int64_t>   // 64 bit ADD
#define SUB64             x86_sub<int64_t>   // 64 bit SUB

#endif
