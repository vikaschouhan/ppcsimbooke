// -*- c++ -*-
//
// Super Standard Template Library
//
// Faster and more optimized than stock STL implementation,
// plus includes various customized features
//
// Copyright 1997-2008 Matt T. Yourst <yourst@yourst.com>
//
// This program is free software; it is licensed under the
// GNU General Public License, Version 2.
//
// NOTE   : This file was taken from ptlsim project by Matt T. Yourst.
// Update : Features not used by ppcsim have been stripped off.

#ifndef _SUPERSTL_H_
#define _SUPERSTL_H_

#include "config.h"

namespace superstl {
    typedef __SIZE_TYPE__ size_t;
    typedef unsigned long long W64;
    typedef signed long long W64s;
    typedef unsigned int W32;
    typedef signed int W32s;
    typedef unsigned short W16;
    typedef signed short W16s;
    typedef unsigned char byte;
    typedef unsigned char W8;
    typedef signed char W8s;
#ifdef __x86_64__
    typedef W64 Waddr;
#else
    typedef W32 Waddr;
#endif


#define superstl_sqr(x) ((x)*(x))
#define superstl_cube(x) ((x)*(x)*(x))
#define superstl_bit(x, n) (((x) >> (n)) & 1)

#define superstl_bitmask(l) (((l) == 64) ? (W64)(-1LL) : ((1LL << (l))-1LL))
#define superstl_bits(x, i, l) (((x) >> (i)) & superstl_bitmask(l))
#define superstl_lowbits(x, l) superstl_bits(x, 0, l)
#define superstl_setbit(x,i) ((x) |= (1LL << (i)))
#define superstl_clearbit(x, i) ((x) &= (W64)(~(1LL << (i))))
#define superstl_assignbit(x, i, v) ((x) = (((x) &= (W64)(~(1LL << (i)))) | (((W64)((bool)(v))) << i)));


#define __superstl_stringify_1(x) #x
#define superstl_stringify(x) __superstl_stringify_1(x)

#define superstl_alignto(x) __attribute__ ((aligned (x)))
#define superstl_insection(x) __attribute__ ((section (x)))
#define superstl_packedstruct __attribute__ ((packed))
#define superstl_noinline __attribute__((noinline))

#define superstl_unlikely(x) (__builtin_expect(!!(x), 0))
#define superstl_likely(x) (__builtin_expect(!!(x), 1))
#define superstl_isconst(x) (__builtin_constant_p(x))
#define superstl_getcaller() (__builtin_return_address(0))

#define superstl_foreach(i, N)  for(int i=0; i<(N); i++)

#define SUPERSTL_BITS_PER_WORD ((sizeof(unsigned long) == 8) ? 64 : 32)
#define SUPERSTL_BITVEC_WORDS(n) ((n) < 1 ? 0 : ((n) + SUPERSTL_BITS_PER_WORD - 1)/SUPERSTL_BITS_PER_WORD)

#define SUPERSTL_HI32(x) (W32)((x) >> 32LL)
#define SUPERSTL_LO32(x) (W32)((x) & 0xffffffffLL)
#define SUPERSTL_CONCAT64(hi, lo) ((((W64)(hi)) << 32) + (((W64)(lo)) & 0xffffffffLL))


    inline W32 x86_bsf32(W32 b) { W32 r = 0; asm("bsf %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    inline W64 x86_bsf64(W64 b) { W64 r = 0; asm("bsf %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    inline W32 x86_bsr32(W32 b) { W32 r = 0; asm("bsr %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    inline W64 x86_bsr64(W64 b) { W64 r = 0; asm("bsr %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    
    template <typename T> inline bool x86_bt(T r, T b) { byte c; asm("bt %[b],%[r]; setc %[c]" : [c] "=r" (c) : [r] "r" (r), [b] "r" (b)); return c; }
    template <typename T> inline bool x86_btn(T r, T b) { byte c; asm("bt %[b],%[r]; setnc %[c]" : [c] "=r" (c) : [r] "r" (r), [b] "r" (b)); return c; }
    
    // Return the updated data; ignore the old value
    template <typename T> inline W64 x86_bts(T r, T b) { asm("bts %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    template <typename T> inline W64 x86_btr(T r, T b) { asm("btr %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    template <typename T> inline W64 x86_btc(T r, T b) { asm("btc %[b],%[r]" : [r] "+r" (r) : [b] "r" (b)); return r; }
    
    // Return the old value of the bit, but still update the data
    template <typename T> inline bool x86_test_bts(T& r, T b) { byte c; asm("bts %[b],%[r]; setc %[c]" : [c] "=r" (c), [r] "+r" (r) : [b] "r" (b)); return c; }
    template <typename T> inline bool x86_test_btr(T& r, T b) { byte c; asm("btr %[b],%[r]; setc %[c]" : [c] "=r" (c), [r] "+r" (r) : [b] "r" (b)); return c; }
    template <typename T> inline bool x86_test_btc(T& r, T b) { byte c; asm("btc %[b],%[r]; setc %[c]" : [c] "=r" (c), [r] "+r" (r) : [b] "r" (b)); return c; }
    
    // Full SMP-aware locking with test-and-[set|reset|complement] in memory
    template <typename T> inline bool x86_locked_bts(T& r, T b) {
        byte c;
        asm volatile("lock bts %[b],%[r]; setc %[c]" : [c] "=r" (c), [r] "+m" (r) : [b] "r" (b) : "memory");
        return c;
    }
    template <typename T> inline bool x86_locked_btr(T& r, T b) {
        byte c;
        asm volatile("lock btr %[b],%[r]; setc %[c]" : [c] "=r" (c), [r] "+m" (r) : [b] "r" (b) : "memory");
        return c;
    }
    template <typename T> inline bool x86_locked_btc(T& r, T b) {
        byte c;
        asm volatile("lock btc %[b],%[r]; setc %[c]" : [c] "=r" (c), [r] "+m" (r) : [b] "r" (b) : "memory");
        return c;
    }
    
    template <typename T> inline T bswap(T r) { asm("bswap %[r]" : [r] "+r" (r)); return r; }

    // Return v with groups of N bits swapped
    template <typename T, int N>
    static inline T bitswap(T v) {
        T m =
            (N == 1) ? T(0x5555555555555555ULL) :
            (N == 2) ? T(0x3333333333333333ULL) :
            (N == 4) ? T(0x0f0f0f0f0f0f0f0fULL) : 0;
        
        return ((v & m) << N) | ((v & (~m)) >> N);
    }
    
    template <typename T>
    T reversebits(T v) {
        v = bitswap<T, 1>(v);
        v = bitswap<T, 2>(v);
        v = bitswap<T, 4>(v);
        v = bswap(v);
        return v;
    }

// This is a barrier for the compiler only, NOT the processor!
#define superstl_barrier() asm volatile("": : :"memory")

    template <typename T>
    static inline T xchg(T& v, T newv) {
    	switch (sizeof(T)) {
            case 1: asm volatile("lock xchgb %[newv],%[v]" : [v] "+m" (v), [newv] "+r" (newv) : : "memory"); break;
            case 2: asm volatile("lock xchgw %[newv],%[v]" : [v] "+m" (v), [newv] "+r" (newv) : : "memory"); break;
            case 4: asm volatile("lock xchgl %[newv],%[v]" : [v] "+m" (v), [newv] "+r" (newv) : : "memory"); break;
            case 8: asm volatile("lock xchgq %[newv],%[v]" : [v] "+m" (v), [newv] "+r" (newv) : : "memory"); break;
    	}
    	return newv;
    }

    template <typename T>
    static inline T xadd(T& v, T incr) {
    	switch (sizeof(T)) {
            case 1: asm volatile("lock xaddb %[incr],%[v]" : [v] "+m" (v), [incr] "+r" (incr) : : "memory"); break;
            case 2: asm volatile("lock xaddw %[incr],%[v]" : [v] "+m" (v), [incr] "+r" (incr) : : "memory"); break;
            case 4: asm volatile("lock xaddl %[incr],%[v]" : [v] "+m" (v), [incr] "+r" (incr) : : "memory"); break;
            case 8: asm volatile("lock xaddq %[incr],%[v]" : [v] "+m" (v), [incr] "+r" (incr) : : "memory"); break;
    	}
        return incr;
    }

    template <typename T>
    static inline T cmpxchg(T& mem, T newv, T cmpv) {
    	switch (sizeof(T)) {
            case 1: asm volatile("lock cmpxchgb %[newv],%[mem]" : [mem] "+m" (mem), [cmpv] "+a" (cmpv), [newv] "+r" (newv) : : "memory"); break;
            case 2: asm volatile("lock cmpxchgw %[newv],%[mem]" : [mem] "+m" (mem), [cmpv] "+a" (cmpv), [newv] "+r" (newv) : : "memory"); break;
            case 4: asm volatile("lock cmpxchgl %[newv],%[mem]" : [mem] "+m" (mem), [cmpv] "+a" (cmpv), [newv] "+r" (newv) : : "memory"); break;
            case 8: asm volatile("lock cmpxchgq %[newv],%[mem]" : [mem] "+m" (mem), [cmpv] "+a" (cmpv), [newv] "+r" (newv) : : "memory"); break;
    	}
    
        // Return the old value in the slot (so we can check if it matches newv)
        return cmpv;
    }

    static inline void cpu_pause() { asm volatile("pause" : : : "memory"); }
    
    static inline void prefetch(const void* x) { asm volatile("prefetcht0 (%0)" : : "r" (x)); }
    
    static inline void cpuid(int op, W32& eax, W32& ebx, W32& ecx, W32& edx) {
    	asm("cpuid" : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "0" (op));
    }
    
    static inline W64 rdtsc() {
        W32 lo, hi;
        asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
        return ((W64)lo) | (((W64)hi) << 32);
    }

    template <typename T>
    static inline T x86_ror(T r, int n) { asm("ror %%cl,%[r]" : [r] "+q" (r) : [n] "c" ((byte)n)); return r; }
    
    template <typename T>
    static inline T x86_rol(T r, int n) { asm("rol %%cl,%[r]" : [r] "+q" (r) : [n] "c" ((byte)n)); return r; }

#ifndef __x86_64__
    // Need to emulate this on 32-bit x86
    template <>
    static inline W64 x86_ror(W64 r, int n) {
        return (r >> n) | (r << (64 - n));
    }
#endif

    template <typename T>
    static inline T dupb(const byte b) { return T(b) * T(0x0101010101010101ULL); }
    
    template <int n> struct lg { static const int value = 1 + lg<n/2>::value; };
    template <> struct lg<1> { static const int value = 0; };
#define superstl_log2(v) (lg<(v)>::value)

    template <int n> struct lg10 { static const int value = 1 + lg10<n/10>::value; };
    template <> struct lg10<1> { static const int value = 0; };
    template <> struct lg10<0> { static const int value = 0; };
#define superstl_log10(v) (lg10<(v)>::value)

    template <int N, typename T>
    static inline T foldbits(T a) {
        if (N == 0) return 0;
        
        const int B = (sizeof(T) * 8);
        const int S = (B / N) + ((B % N) ? 1 : 0);
        
        T z = 0;
        superstl_foreach (i, S) {
            z ^= a;
            a >>= N;
        }
        
        return superstl_lowbits(z, N);
    }


    template <bool b00,     bool b01 = 0, bool b02 = 0, bool b03 = 0,
              bool b04 = 0, bool b05 = 0, bool b06 = 0, bool b07 = 0,
              bool b08 = 0, bool b09 = 0, bool b10 = 0, bool b11 = 0,
              bool b12 = 0, bool b13 = 0, bool b14 = 0, bool b15 = 0,
              bool b16 = 0, bool b17 = 0, bool b18 = 0, bool b19 = 0,
              bool b20 = 0, bool b21 = 0, bool b22 = 0, bool b23 = 0,
              bool b24 = 0, bool b25 = 0, bool b26 = 0, bool b27 = 0,
              bool b28 = 0, bool b29 = 0, bool b30 = 0, bool b31 = 0,
              bool b32 = 0, bool b33 = 0, bool b34 = 0, bool b35 = 0,
              bool b36 = 0, bool b37 = 0, bool b38 = 0, bool b39 = 0,
              bool b40 = 0, bool b41 = 0, bool b42 = 0, bool b43 = 0,
              bool b44 = 0, bool b45 = 0, bool b46 = 0, bool b47 = 0,
              bool b48 = 0, bool b49 = 0, bool b50 = 0, bool b51 = 0,
              bool b52 = 0, bool b53 = 0, bool b54 = 0, bool b55 = 0,
              bool b56 = 0, bool b57 = 0, bool b58 = 0, bool b59 = 0,
              bool b60 = 0, bool b61 = 0, bool b62 = 0, bool b63 = 0>
    struct constbits {
        static const W64 value =
        (((W64)b00)  <<  0) +
        (((W64)b01)  <<  1) +
        (((W64)b02)  <<  2) +
        (((W64)b03)  <<  3) +
        (((W64)b04)  <<  4) +
        (((W64)b05)  <<  5) +
        (((W64)b06)  <<  6) +
        (((W64)b07)  <<  7) +
        (((W64)b08)  <<  8) +
        (((W64)b09)  <<  9) +
        (((W64)b10)  << 10) +
        (((W64)b11)  << 11) +
        (((W64)b12)  << 12) +
        (((W64)b13)  << 13) +
        (((W64)b14)  << 14) +
        (((W64)b15)  << 15) +
        (((W64)b16)  << 16) +
        (((W64)b17)  << 17) +
        (((W64)b18)  << 18) +
        (((W64)b19)  << 19) +
        (((W64)b20)  << 20) +
        (((W64)b21)  << 21) +
        (((W64)b22)  << 22) +
        (((W64)b23)  << 23) +
        (((W64)b24)  << 24) +
        (((W64)b25)  << 25) +
        (((W64)b26)  << 26) +
        (((W64)b27)  << 27) +
        (((W64)b28)  << 28) +
        (((W64)b29)  << 29) +
        (((W64)b30)  << 30) +
        (((W64)b31)  << 31) +
        (((W64)b32)  << 32) +
        (((W64)b33)  << 33) +
        (((W64)b34)  << 34) +
        (((W64)b35)  << 35) +
        (((W64)b36)  << 36) +
        (((W64)b37)  << 37) +
        (((W64)b38)  << 38) +
        (((W64)b39)  << 39) +
        (((W64)b40)  << 40) +
        (((W64)b41)  << 41) +
        (((W64)b42)  << 42) +
        (((W64)b43)  << 43) +
        (((W64)b44)  << 44) +
        (((W64)b45)  << 45) +
        (((W64)b46)  << 46) +
        (((W64)b47)  << 47) +
        (((W64)b48)  << 48) +
        (((W64)b49)  << 49) +
        (((W64)b50)  << 50) +
        (((W64)b51)  << 51) +
        (((W64)b52)  << 52) +
        (((W64)b53)  << 53) +
        (((W64)b54)  << 54) +
        (((W64)b55)  << 55) +
        (((W64)b56)  << 56) +
        (((W64)b57)  << 57) +
        (((W64)b58)  << 58) +
        (((W64)b59)  << 59) +
        (((W64)b60)  << 60) +
        (((W64)b61)  << 61) +
        (((W64)b62)  << 62) +
        (((W64)b63)  << 63);
        
        operator W64() const { return value; }
    };

    
    static const W64 expand_8bit_to_64bit_lut[256] superstl_alignto(8) = {
        0x0000000000000000ULL,   0x00000000000000ffULL,   0x000000000000ff00ULL,   0x000000000000ffffULL,
        0x0000000000ff0000ULL,   0x0000000000ff00ffULL,   0x0000000000ffff00ULL,   0x0000000000ffffffULL,
        0x00000000ff000000ULL,   0x00000000ff0000ffULL,   0x00000000ff00ff00ULL,   0x00000000ff00ffffULL,
        0x00000000ffff0000ULL,   0x00000000ffff00ffULL,   0x00000000ffffff00ULL,   0x00000000ffffffffULL,
        0x000000ff00000000ULL,   0x000000ff000000ffULL,   0x000000ff0000ff00ULL,   0x000000ff0000ffffULL,
        0x000000ff00ff0000ULL,   0x000000ff00ff00ffULL,   0x000000ff00ffff00ULL,   0x000000ff00ffffffULL,
        0x000000ffff000000ULL,   0x000000ffff0000ffULL,   0x000000ffff00ff00ULL,   0x000000ffff00ffffULL,
        0x000000ffffff0000ULL,   0x000000ffffff00ffULL,   0x000000ffffffff00ULL,   0x000000ffffffffffULL,
        0x0000ff0000000000ULL,   0x0000ff00000000ffULL,   0x0000ff000000ff00ULL,   0x0000ff000000ffffULL,
        0x0000ff0000ff0000ULL,   0x0000ff0000ff00ffULL,   0x0000ff0000ffff00ULL,   0x0000ff0000ffffffULL,
        0x0000ff00ff000000ULL,   0x0000ff00ff0000ffULL,   0x0000ff00ff00ff00ULL,   0x0000ff00ff00ffffULL,
        0x0000ff00ffff0000ULL,   0x0000ff00ffff00ffULL,   0x0000ff00ffffff00ULL,   0x0000ff00ffffffffULL,
        0x0000ffff00000000ULL,   0x0000ffff000000ffULL,   0x0000ffff0000ff00ULL,   0x0000ffff0000ffffULL,
        0x0000ffff00ff0000ULL,   0x0000ffff00ff00ffULL,   0x0000ffff00ffff00ULL,   0x0000ffff00ffffffULL,
        0x0000ffffff000000ULL,   0x0000ffffff0000ffULL,   0x0000ffffff00ff00ULL,   0x0000ffffff00ffffULL,
        0x0000ffffffff0000ULL,   0x0000ffffffff00ffULL,   0x0000ffffffffff00ULL,   0x0000ffffffffffffULL,
        0x00ff000000000000ULL,   0x00ff0000000000ffULL,   0x00ff00000000ff00ULL,   0x00ff00000000ffffULL,
        0x00ff000000ff0000ULL,   0x00ff000000ff00ffULL,   0x00ff000000ffff00ULL,   0x00ff000000ffffffULL,
        0x00ff0000ff000000ULL,   0x00ff0000ff0000ffULL,   0x00ff0000ff00ff00ULL,   0x00ff0000ff00ffffULL,
        0x00ff0000ffff0000ULL,   0x00ff0000ffff00ffULL,   0x00ff0000ffffff00ULL,   0x00ff0000ffffffffULL,
        0x00ff00ff00000000ULL,   0x00ff00ff000000ffULL,   0x00ff00ff0000ff00ULL,   0x00ff00ff0000ffffULL,
        0x00ff00ff00ff0000ULL,   0x00ff00ff00ff00ffULL,   0x00ff00ff00ffff00ULL,   0x00ff00ff00ffffffULL,
        0x00ff00ffff000000ULL,   0x00ff00ffff0000ffULL,   0x00ff00ffff00ff00ULL,   0x00ff00ffff00ffffULL,
        0x00ff00ffffff0000ULL,   0x00ff00ffffff00ffULL,   0x00ff00ffffffff00ULL,   0x00ff00ffffffffffULL,
        0x00ffff0000000000ULL,   0x00ffff00000000ffULL,   0x00ffff000000ff00ULL,   0x00ffff000000ffffULL,
        0x00ffff0000ff0000ULL,   0x00ffff0000ff00ffULL,   0x00ffff0000ffff00ULL,   0x00ffff0000ffffffULL,
        0x00ffff00ff000000ULL,   0x00ffff00ff0000ffULL,   0x00ffff00ff00ff00ULL,   0x00ffff00ff00ffffULL,
        0x00ffff00ffff0000ULL,   0x00ffff00ffff00ffULL,   0x00ffff00ffffff00ULL,   0x00ffff00ffffffffULL,
        0x00ffffff00000000ULL,   0x00ffffff000000ffULL,   0x00ffffff0000ff00ULL,   0x00ffffff0000ffffULL,
        0x00ffffff00ff0000ULL,   0x00ffffff00ff00ffULL,   0x00ffffff00ffff00ULL,   0x00ffffff00ffffffULL,
        0x00ffffffff000000ULL,   0x00ffffffff0000ffULL,   0x00ffffffff00ff00ULL,   0x00ffffffff00ffffULL,
        0x00ffffffffff0000ULL,   0x00ffffffffff00ffULL,   0x00ffffffffffff00ULL,   0x00ffffffffffffffULL,
        0xff00000000000000ULL,   0xff000000000000ffULL,   0xff0000000000ff00ULL,   0xff0000000000ffffULL,
        0xff00000000ff0000ULL,   0xff00000000ff00ffULL,   0xff00000000ffff00ULL,   0xff00000000ffffffULL,
        0xff000000ff000000ULL,   0xff000000ff0000ffULL,   0xff000000ff00ff00ULL,   0xff000000ff00ffffULL,
        0xff000000ffff0000ULL,   0xff000000ffff00ffULL,   0xff000000ffffff00ULL,   0xff000000ffffffffULL,
        0xff0000ff00000000ULL,   0xff0000ff000000ffULL,   0xff0000ff0000ff00ULL,   0xff0000ff0000ffffULL,
        0xff0000ff00ff0000ULL,   0xff0000ff00ff00ffULL,   0xff0000ff00ffff00ULL,   0xff0000ff00ffffffULL,
        0xff0000ffff000000ULL,   0xff0000ffff0000ffULL,   0xff0000ffff00ff00ULL,   0xff0000ffff00ffffULL,
        0xff0000ffffff0000ULL,   0xff0000ffffff00ffULL,   0xff0000ffffffff00ULL,   0xff0000ffffffffffULL,
        0xff00ff0000000000ULL,   0xff00ff00000000ffULL,   0xff00ff000000ff00ULL,   0xff00ff000000ffffULL,
        0xff00ff0000ff0000ULL,   0xff00ff0000ff00ffULL,   0xff00ff0000ffff00ULL,   0xff00ff0000ffffffULL,
        0xff00ff00ff000000ULL,   0xff00ff00ff0000ffULL,   0xff00ff00ff00ff00ULL,   0xff00ff00ff00ffffULL,
        0xff00ff00ffff0000ULL,   0xff00ff00ffff00ffULL,   0xff00ff00ffffff00ULL,   0xff00ff00ffffffffULL,
        0xff00ffff00000000ULL,   0xff00ffff000000ffULL,   0xff00ffff0000ff00ULL,   0xff00ffff0000ffffULL,
        0xff00ffff00ff0000ULL,   0xff00ffff00ff00ffULL,   0xff00ffff00ffff00ULL,   0xff00ffff00ffffffULL,
        0xff00ffffff000000ULL,   0xff00ffffff0000ffULL,   0xff00ffffff00ff00ULL,   0xff00ffffff00ffffULL,
        0xff00ffffffff0000ULL,   0xff00ffffffff00ffULL,   0xff00ffffffffff00ULL,   0xff00ffffffffffffULL,
        0xffff000000000000ULL,   0xffff0000000000ffULL,   0xffff00000000ff00ULL,   0xffff00000000ffffULL,
        0xffff000000ff0000ULL,   0xffff000000ff00ffULL,   0xffff000000ffff00ULL,   0xffff000000ffffffULL,
        0xffff0000ff000000ULL,   0xffff0000ff0000ffULL,   0xffff0000ff00ff00ULL,   0xffff0000ff00ffffULL,
        0xffff0000ffff0000ULL,   0xffff0000ffff00ffULL,   0xffff0000ffffff00ULL,   0xffff0000ffffffffULL,
        0xffff00ff00000000ULL,   0xffff00ff000000ffULL,   0xffff00ff0000ff00ULL,   0xffff00ff0000ffffULL,
        0xffff00ff00ff0000ULL,   0xffff00ff00ff00ffULL,   0xffff00ff00ffff00ULL,   0xffff00ff00ffffffULL,
        0xffff00ffff000000ULL,   0xffff00ffff0000ffULL,   0xffff00ffff00ff00ULL,   0xffff00ffff00ffffULL,
        0xffff00ffffff0000ULL,   0xffff00ffffff00ffULL,   0xffff00ffffffff00ULL,   0xffff00ffffffffffULL,
        0xffffff0000000000ULL,   0xffffff00000000ffULL,   0xffffff000000ff00ULL,   0xffffff000000ffffULL,
        0xffffff0000ff0000ULL,   0xffffff0000ff00ffULL,   0xffffff0000ffff00ULL,   0xffffff0000ffffffULL,
        0xffffff00ff000000ULL,   0xffffff00ff0000ffULL,   0xffffff00ff00ff00ULL,   0xffffff00ff00ffffULL,
        0xffffff00ffff0000ULL,   0xffffff00ffff00ffULL,   0xffffff00ffffff00ULL,   0xffffff00ffffffffULL,
        0xffffffff00000000ULL,   0xffffffff000000ffULL,   0xffffffff0000ff00ULL,   0xffffffff0000ffffULL,
        0xffffffff00ff0000ULL,   0xffffffff00ff00ffULL,   0xffffffff00ffff00ULL,   0xffffffff00ffffffULL,
        0xffffffffff000000ULL,   0xffffffffff0000ffULL,   0xffffffffff00ff00ULL,   0xffffffffff00ffffULL,
        0xffffffffffff0000ULL,   0xffffffffffff00ffULL,   0xffffffffffffff00ULL,   0xffffffffffffffffULL,
    };    

    static const unsigned char popcountlut8bit[] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

    static const byte lsbindexlut8bit[256] = {
        0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
    };

    static inline int popcount8bit(byte x) {
        return popcountlut8bit[x];
    }
    
    static inline int lsbindex8bit(byte x) {
        return lsbindexlut8bit[x];
    }
    
    static inline int popcount(W32 x) {
        return (popcount8bit(x >> 0) + popcount8bit(x >> 8) + popcount8bit(x >> 16) + popcount8bit(x >> 24));
    }
    
    static inline int popcount64(W64 x) {
        return popcount(SUPERSTL_LO32(x)) + popcount(SUPERSTL_HI32(x));
    }

    // LSB index:

    // Operand must be non-zero or result is undefined:
    inline unsigned int lsbindex32(W32 n) { return x86_bsf32(n); }
    
    inline int lsbindexi32(W32 n) {
        int r = lsbindex32(n);
        return (n ? r : -1);
    }

#ifdef __x86_64__
    inline unsigned int lsbindex64(W64 n) { return x86_bsf64(n); }
#else
    inline unsigned int lsbindex64(W64 n) {
        unsigned int z;
        W32 lo = SUPERSTL_LO32(n);
        W32 hi = SUPERSTL_HI32(n);

        int ilo = lsbindex32(lo);
        int ihi = lsbindex32(hi) + 32;

        return (lo) ? ilo : ihi;
    }
#endif

    inline unsigned int lsbindexi64(W64 n) {
        int r = lsbindex64(n);
        return (n ? r : -1);
    }
    
    // static inline unsigned int lsbindex(W32 n) { return lsbindex32(n); }
    inline unsigned int lsbindex(W64 n) { return lsbindex64(n); }
    
    // MSB index:
    
    // Operand must be non-zero or result is undefined:
    inline unsigned int msbindex32(W32 n) { return x86_bsr32(n); }
    
    inline int msbindexi32(W32 n) {
        int r = msbindex32(n);
        return (n ? r : -1);
    }

#ifdef __x86_64__
    inline unsigned int msbindex64(W64 n) { return x86_bsr64(n); }
#else
    inline unsigned int msbindex64(W64 n) {
        unsigned int z;
        W32 lo = SUPERSTL_LO32(n);
        W32 hi = SUPERSTL_HI32(n);

        int ilo = msbindex32(lo);
        int ihi = msbindex32(hi) + 32;

        return (hi) ? ihi : ilo;
    }
#endif

    inline unsigned int msbindexi64(W64 n) {
        int r = msbindex64(n);
        return (n ? r : -1);
    }
    
    // static inline unsigned int msbindex(W32 n) { return msbindex32(n); }
    inline unsigned int msbindex(W64 n) { return msbindex64(n); }

#define superstl_percent(x, total) (100.0 * ((float)(x)) / ((float)(total)))

    inline int modulo_span(int lower, int upper, int modulus) {
        int result = (upper - lower);
        if (upper < lower) result += modulus;
        return result;
    }
    
    inline int add_index_modulo(int index, int increment, int bufsize) {
        // Only if power of 2: return (index + increment) & (bufsize-1);
        index += increment;
        if (index < 0) index += bufsize;
        if (index >= bufsize) index -= bufsize;
        return index;
    }

    inline bool modulo_ranges_intersect(int a0, int a1, int b0, int b1, int size) {
    
        int idx = 
          ((a0 <= a1) << 0) |
          ((a1 <= b0) << 1) |
          ((b0 <= b1) << 2) |
          ((b1 <= a0) << 3) |
          ((a0 != b0) << 4) |
          ((a0 != b1) << 5) |
          ((a1 != b0) << 6) | 
          ((a1 != b1) << 7);
        
        static const byte lut[256] = {
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
          1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0
        };
        
        return lut[idx];
    }

//
// Express bitstring constant as octal but convert
// to binary for easy Verilog-style expressions:
//
#define bitseq(x) \
    (superstl_bit((W64)x,  0*3) <<  0) + \
    (superstl_bit((W64)x,  1*3) <<  1) + \
    (superstl_bit((W64)x,  2*3) <<  2) + \
    (superstl_bit((W64)x,  3*3) <<  3) + \
    (superstl_bit((W64)x,  4*3) <<  4) + \
    (superstl_bit((W64)x,  5*3) <<  5) + \
    (superstl_bit((W64)x,  6*3) <<  6) + \
    (superstl_bit((W64)x,  7*3) <<  7) + \
    (superstl_bit((W64)x,  8*3) <<  8) + \
    (superstl_bit((W64)x,  9*3) <<  9) + \
    (superstl_bit((W64)x, 10*3) << 10) + \
    (superstl_bit((W64)x, 11*3) << 11) + \
    (superstl_bit((W64)x, 12*3) << 12) + \
    (superstl_bit((W64)x, 13*3) << 13) + \
    (superstl_bit((W64)x, 14*3) << 14) + \
    (superstl_bit((W64)x, 15*3) << 15) + \
    (superstl_bit((W64)x, 16*3) << 16) + \
    (superstl_bit((W64)x, 17*3) << 17) + \
    (superstl_bit((W64)x, 18*3) << 18) + \
    (superstl_bit((W64)x, 19*3) << 19) + \
    (superstl_bit((W64)x, 20*3) << 20) + \
    (superstl_bit((W64)x, 21*3) << 21)

 
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __x86_64__
#define __builtin_ctzl(t) lsbindex64(t)
#define __builtin_clzl(t) msbindex64(t)
#else
#define __builtin_ctzl(t) lsbindex32(t)
#define __builtin_clzl(t) msbindex32(t)
#endif
   
    template<size_t N>
    struct bitvecbase {
        typedef unsigned long T;

        T w[N];

        bitvecbase() { resetop(); }

        bitvecbase(const bitvecbase<N>& vec) { superstl_foreach (i, N) w[i] = vec.w[i]; }

        bitvecbase(unsigned long long val) {
            resetop();
            w[0] = val;
        }

        static size_t wordof(size_t index) { return index / SUPERSTL_BITS_PER_WORD; }
        static size_t byteof(size_t index) { return (index % SUPERSTL_BITS_PER_WORD) / __CHAR_BIT__; }
        static size_t bitof(size_t index) { return index % SUPERSTL_BITS_PER_WORD; }
        static T maskof(size_t index) { return (static_cast<T>(1)) << bitof(index); }

        T& getword(size_t index) { return w[wordof(index)]; }
        T getword(size_t index) const { return w[wordof(index)]; }
        T& hiword() { return w[N - 1]; }
        T hiword() const { return w[N - 1]; }

        void andop(const bitvecbase<N>& x) {
            for (size_t i = 0; i < N; i++) w[i] &= x.w[i];
        }

        void orop(const bitvecbase<N>& x) {
            superstl_foreach (i, N) w[i] |= x.w[i];
        }

        void xorop(const bitvecbase<N>& x) {
            superstl_foreach (i, N) w[i] ^= x.w[i];
        }

        void shiftleftop(size_t shift) {
            if superstl_likely (shift) {
                const size_t wshift = shift / SUPERSTL_BITS_PER_WORD;
                const size_t offset = shift % SUPERSTL_BITS_PER_WORD;

                if superstl_unlikely (offset == 0) {
                    for (size_t i = N - 1; i >= wshift; --i) { w[i] = w[i - wshift]; }
                } else {
                    const size_t suboffset = SUPERSTL_BITS_PER_WORD - offset;
                    for (size_t i = N - 1; i > wshift; --i) { w[i] = (w[i - wshift] << offset) | (w[i - wshift - 1] >> suboffset); }
                    w[wshift] = w[0] << offset;
                }

                // memset(w, static_cast<T>(0), wshift);
                superstl_foreach (i, wshift) { w[i] = 0; }
            }
        }

        void shiftrightop(size_t shift) {
            if superstl_likely (shift) {
                const size_t wshift = shift / SUPERSTL_BITS_PER_WORD;
                const size_t offset = shift % SUPERSTL_BITS_PER_WORD;
                const size_t limit = N - wshift - 1;

                if superstl_unlikely (offset == 0) {
                    for (size_t i = 0; i <= limit; ++i) { w[i] = w[i + wshift]; }
                } else {
                    const size_t suboffset = SUPERSTL_BITS_PER_WORD - offset;
                    for (size_t i = 0; i < limit; ++i) { w[i] = (w[i + wshift] >> offset) | (w[i + wshift + 1] << suboffset); }
                    w[limit] = w[N-1] >> offset;
                }

                //memset(w + limit + 1, static_cast<T>(0), N - (limit + 1));
                superstl_foreach (i, N - (limit + 1)) { w[limit + 1 + i] = 0; }
            }
        }

        void maskop(size_t count) {
            T m =
                (!count) ? 0 :
                (count % SUPERSTL_BITS_PER_WORD) ? ((T(1) << bitof(count)) - T(1)) :
                T(-1);

            w[wordof(count)] &= m;

            for (size_t i = wordof(count)+1; i < N; i++) {
                w[i] = 0;
            }
        }

        void invertop() {
            superstl_foreach (i, N) w[i] = ~w[i];
        }

        void setallop() {
            superstl_foreach (i, N) w[i] = ~static_cast<T>(0);
        }

        void resetop() { memset(w, 0, N * sizeof(T)); }

        bool equalop(const bitvecbase<N>& x) const {
            T t = 0;
            superstl_foreach (i, N) { t |= (w[i] ^ x.w[i]); }
            return (t == 0);
        }

        bool nonzeroop() const {
            T t = 0;
            superstl_foreach (i, N) { t |= w[i]; }
            return (t != 0);
        }

        size_t popcountop() const {
            size_t result = 0;

            superstl_foreach (i, N)
                result += popcount64(w[i]);

            return result;
        }

        unsigned long integerop() const { return w[0]; }

        void insertop(size_t i, size_t n, T v) {
            T& lw = w[wordof(i)];
            T lm = (superstl_bitmask(n) << bitof(i));
            lw = (lw & ~lm) | ((v << i) & lm);

            if superstl_unlikely ((bitof(i) + n) > SUPERSTL_BITS_PER_WORD) {
                T& hw = w[wordof(i+1)];
                T hm = (superstl_bitmask(n) >> (SUPERSTL_BITS_PER_WORD - bitof(i)));
                hw = (hw & ~hm) | ((v >> (SUPERSTL_BITS_PER_WORD - bitof(i))) & hm);
            }
        }

        void accumop(size_t i, size_t n, T v) {
            w[wordof(i)] |= (v << i);

            if superstl_unlikely ((bitof(i) + n) > SUPERSTL_BITS_PER_WORD)
                w[wordof(i+1)] |= (v >> (SUPERSTL_BITS_PER_WORD - bitof(i)));
        }

        // find index of first "1" bit starting from low end
        size_t lsbop(size_t notfound) const {
            superstl_foreach (i, N) {
                T t = w[i];
                if superstl_likely (t) return (i * SUPERSTL_BITS_PER_WORD) + __builtin_ctzl(t);
            }
            return notfound;
        }

        // find index of last "1" bit starting from high end
        size_t msbop(size_t notfound) const {
            for (int i = N-1; i >= 0; i--) {
                T t = w[i];
                if superstl_likely (t) return (i * SUPERSTL_BITS_PER_WORD) + __builtin_clzl(t);
            }
            return notfound;
        }

        // assume value is nonzero
        size_t lsbop() const {
            return lsbop(0);
        }

        // assume value is nonzero
        size_t msbop() const {
            return msbop(0);
        }

        // find the next "on" bit that follows "prev"

        size_t nextlsbop(size_t prev, size_t notfound) const {
            // make bound inclusive
            ++prev;

            // check out of bounds
            if superstl_unlikely (prev >= N * SUPERSTL_BITS_PER_WORD)
                return notfound;

            // search first word
            size_t i = wordof(prev);
            T t = w[i];

            // mask off bits below bound
            t &= (~static_cast<T>(0)) << bitof(prev);

            if superstl_likely (t != static_cast<T>(0))
                return (i * SUPERSTL_BITS_PER_WORD) + __builtin_ctzl(t);

            // check subsequent words
            i++;
            for ( ; i < N; i++ ) {
                t = w[i];
                if superstl_likely (t != static_cast<T>(0))
                    return (i * SUPERSTL_BITS_PER_WORD) + __builtin_ctzl(t);
            }
            // not found, so return an indication of failure.
            return notfound;
        }
    };

    template <>
    struct bitvecbase<1> {
        typedef unsigned long T;
        T w;

        bitvecbase(void): w(0) {}
        bitvecbase(unsigned long long val): w(val) {}

        static size_t wordof(size_t index) { return index / SUPERSTL_BITS_PER_WORD; }
        static size_t byteof(size_t index) { return (index % SUPERSTL_BITS_PER_WORD) / __CHAR_BIT__; }
        static size_t bitof(size_t index) { return index % SUPERSTL_BITS_PER_WORD; }
        static T maskof(size_t index) { return (static_cast<T>(1)) << bitof(index); }

        T& getword(size_t) { return w; }
        T getword(size_t) const { return w; }
        T& hiword() { return w; }
        T hiword() const { return w; }
        void andop(const bitvecbase<1>& x) { w &= x.w; }
        void orop(const bitvecbase<1>& x)  { w |= x.w; }
        void xorop(const bitvecbase<1>& x) { w ^= x.w; }
        void shiftleftop(size_t __shift) { w <<= __shift; }
        void shiftrightop(size_t __shift) { w >>= __shift; }
        void invertop() { w = ~w; }
        void setallop() { w = ~static_cast<T>(0); }
        void resetop() { w = 0; }
        bool equalop(const bitvecbase<1>& x) const { return w == x.w; }
        bool nonzeroop() const { return (!!w); }
        size_t popcountop() const { return popcount64(w); }
        unsigned long integerop() const { return w; }
        size_t lsbop() const { return __builtin_ctzl(w); }
        size_t msbop() const { return __builtin_clzl(w); }
        size_t lsbop(size_t notfound) const { return (w) ? __builtin_ctzl(w) : notfound; }
        size_t msbop(size_t notfound) const { return (w) ? __builtin_clzl(w) : notfound; }
        void maskop(size_t count) {
            T m =
                (!count) ? 0 :
                (count < SUPERSTL_BITS_PER_WORD) ? ((T(1) << bitof(count)) - T(1)) :
                T(-1);

            w &= m;
        }

        void insertop(size_t i, size_t n, T v) {
            T m = (superstl_bitmask(n) << bitof(i));
            w = (w & ~m) | ((v << i) & m);
        }

        void accumop(size_t i, size_t n, T v) {
            w |= (v << i);
        }

        // find the next "on" bit that follows "prev"
        size_t nextlsbop(size_t __prev, size_t notfound) const {
            ++__prev;
            if superstl_unlikely (__prev >= ((size_t) SUPERSTL_BITS_PER_WORD))
                return notfound;

            T x = w >> __prev;
            if superstl_likely (x != 0)
                return __builtin_ctzl(x) + __prev;
            else
                return notfound;
        }
    };

    template <>
    struct bitvecbase<0> {
        typedef unsigned long T;

        bitvecbase() { }
        bitvecbase(unsigned long long) { }

        static size_t wordof(size_t index) { return index / SUPERSTL_BITS_PER_WORD; }
        static size_t byteof(size_t index) { return (index % SUPERSTL_BITS_PER_WORD) / __CHAR_BIT__; }
        static size_t bitof(size_t index) { return index % SUPERSTL_BITS_PER_WORD; }
        static T maskof(size_t index) { return (static_cast<T>(1)) << bitof(index); }

        T& getword(size_t) const { return *new T;  }
        T hiword() const { return 0; }
        void andop(const bitvecbase<0>&) { }
        void orop(const bitvecbase<0>&)  { }
        void xorop(const bitvecbase<0>&) { }
        void shiftleftop(size_t) { }
        void shiftrightop(size_t) { }
        void invertop() { }
        void setallop() { }
        void resetop() { }
        bool equalop(const bitvecbase<0>&) const { return true; }
        bool nonzeroop() const { return false; }
        size_t popcountop() const { return 0; }
        void maskop(size_t count) { }
        void accumop(int i, int n, T v) { }
        void insertop(int i, int n, T v) { }
        unsigned long integerop() const { return 0; }
        size_t lsbop() const { return 0; }
        size_t msbop() const { return 0; }
        size_t lsbop(size_t notfound) const { return notfound; }
        size_t msbop(size_t notfound) const { return notfound; }
        size_t nextlsbop(size_t, size_t) const { return 0; }
    };

    // Helper class to zero out the unused high-order bits in the highest word.
    template <size_t extrabits>
    struct bitvec_sanitizer {
        static void sanitize(unsigned long& val) {
            val &= ~((~static_cast<unsigned long>(0)) << extrabits);
        }
    };

    template <>
    struct bitvec_sanitizer<0> {
        static void sanitize(unsigned long) { }
    };

    template<size_t N>
    class bitvec: private bitvecbase<SUPERSTL_BITVEC_WORDS(N)> {
        private:
        typedef bitvecbase<SUPERSTL_BITVEC_WORDS(N)> base_t;
        typedef unsigned long T;

        bitvec<N>& sanitize() {
            bitvec_sanitizer<N % SUPERSTL_BITS_PER_WORD>::sanitize(this->hiword());
            return *this;
        }

        public:
        class reference {
            friend class bitvec;

            T *wp;
            T bpos;

            // left undefined
            reference();

            public:
            inline reference(bitvec& __b, size_t index) {
              wp = &__b.getword(index);
              bpos = base_t::bitof(index);
            }

            ~reference() { }

            // For b[i] = x;
            inline reference& operator =(bool x) {
                // Optimized, x86-specific way:
                if (superstl_isconst(x) & superstl_isconst(bpos)) {
                    // Most efficient to just AND/OR with a constant mask:
                    *wp = ((x) ? (*wp | base_t::maskof(bpos)) : (*wp & (~base_t::maskof(bpos))));
                } else {
                    // Use bit set or bit reset x86 insns:
                    T b1 = x86_bts(*wp, bpos);
                    T b0 = x86_btr(*wp, bpos);
                    *wp = (x) ? b1 : b0;
                }
                /*
                // Optimized, branch free generic way:
                *wp = (__builtin_constant_p(x)) ?
                  ((x) ? (*wp | base_t::maskof(bpos)) : (*wp & (~base_t::maskof(bpos)))) :
                  (((*wp) & (~base_t::maskof(bpos))) | ((static_cast<T>((x != 0))) << base_t::bitof(bpos)));
                */
                return *this;
            }

            // For b[i] = b[j];
            inline reference& operator =(const reference& j) {
                // Optimized, x86-specific way:
                // Use bit set or bit reset x86 insns:
                T b1 = x86_bts(*wp, bpos);
                T b0 = x86_btr(*wp, bpos);
                *wp = (x86_bt(*j.wp, j.bpos)) ? b1 : b0;
                /*
                // Optimized, branch free generic way:
                *wp = (__builtin_constant_p(x)) ?
                  (((*(j.wp) & base_t::maskof(j.bpos))) ? (*wp | base_t::maskof(bpos)) : (*wp & (~base_t::maskof(bpos)))) :
                  (((*wp) & (~base_t::maskof(bpos))) | ((static_cast<T>((((*(j.wp) & base_t::maskof(j.bpos))) != 0))) << base_t::bitof(bpos)));
                */
                return *this;
            }

            // For b[i] = 1;
            inline reference& operator++(int postfixdummy) {
                if (superstl_isconst(bpos))
                    *wp |= base_t::maskof(bpos);
                else *wp = x86_bts(*wp, bpos);
                return *this;
            }

            // For b[i] = 0;
            inline reference& operator--(int postfixdummy) {
                if (superstl_isconst(bpos))
                    *wp &= ~base_t::maskof(bpos);
                else *wp = x86_btr(*wp, bpos);
                return *this;
            }

            // Flips the bit
            bool operator~() const {
                //return (*(wp) & base_t::maskof(bpos)) == 0;
                return x86_btn(*wp, bpos);
            }

            // For x = b[i];
            inline operator bool() const {
                return x86_bt(*wp, bpos);
            }

            // For b[i].invert();
            inline reference& invert() {
                *wp = x86_btc(*wp, bpos);
                return *this;
            }

            bool testset() { return x86_test_bts(*wp, bpos); }
            bool testclear() { return x86_test_btr(*wp, bpos); }
            bool testinv() { return x86_test_btc(*wp, bpos); }

            bool atomicset() { return x86_locked_bts(*wp, bpos); }
            bool atomicclear() { return x86_locked_btr(*wp, bpos); }
            bool atomicinv() { return x86_locked_btc(*wp, bpos); }
        };

        friend class reference;

        bitvec() { }

        bitvec(const bitvec<N>& vec): base_t(vec) { }

        bitvec(unsigned long long val): base_t(val) { sanitize(); }

        bitvec<N>& operator&=(const bitvec<N>& rhs) {
            this->andop(rhs);
            return *this;
        }

        bitvec<N>& operator|=(const bitvec<N>& rhs) {
            this->orop(rhs);
            return *this;
        }

        bitvec<N>& operator^=(const bitvec<N>& rhs) {
            this->xorop(rhs);
            return *this;
        }

        bitvec<N>& operator <<=(int index) {
            if superstl_likely (index < N) {
                this->shiftleftop(index);
                this->sanitize();
            } else this->resetop();
            return *this;
        }

        bitvec<N>& operator>>=(int index) {
            if superstl_likely (index < N) {
                this->shiftrightop(index);
                this->sanitize();
            } else this->resetop();
            return *this;
        }

        bitvec<N> rotright(int index) const {
            return ((*this) >> index) | ((*this) << (N - index));
        }

        bitvec<N> rotleft(int index) const {
            return ((*this) << index) | ((*this) >> (N - index));
        }

        bitvec<N>& set(size_t index) {
            this->getword(index) |= base_t::maskof(index);
            return *this;
        }

        bitvec<N>& reset(size_t index) {
            this->getword(index) &= ~base_t::maskof(index);
            return *this;
        }

        bitvec<N>& assign(size_t index, int val) {
            if (val)
                this->getword(index) |= base_t::maskof(index);
            else
                this->getword(index) &= ~base_t::maskof(index);
            return *this;
        }

        bitvec<N>& invert(size_t index) {
            this->getword(index) ^= base_t::maskof(index);
            return *this;
        }

        bool test(size_t index) const {
            return (this->getword(index) & base_t::maskof(index)) != static_cast<T>(0);
        }

        bitvec<N>& setall() {
            this->setallop();
            this->sanitize();
            return *this;
        }

        bitvec<N>& reset() {
            this->resetop();
            return *this;
        }

        bitvec<N>& operator++(int postfixdummy) { return setall(); }
        bitvec<N>& operator--(int postfixdummy) { return reset(); }

        bitvec<N>& invert() {
            this->invertop();
            this->sanitize();
            return *this;
        }

        bitvec<N> operator ~() const { return bitvec<N>(*this).invert(); }

        reference operator [](size_t index) { return reference(*this, index); }

        bool operator [](size_t index) const { return test(index); }

        bool operator *() const { return nonzero(); }
        bool operator !() const { return iszero(); }

        unsigned long integer() const { return this->integerop(); }

        // Returns the number of bits which are set.
        size_t popcount() const { return this->popcountop(); }

        // Returns the total number of bits.
        size_t size() const { return N; }

        bool operator ==(const bitvec<N>& rhs) const { return this->equalop(rhs); }
        bool operator !=(const bitvec<N>& rhs) const { return !this->equalop(rhs); }
        bool nonzero() const { return this->nonzeroop(); }
        bool iszero() const { return !this->nonzeroop(); }
        bool allset() const { return (~(*this)).iszero(); }
        bool all() const { return allset(N); }

        bitvec<N> operator <<(size_t shift) const { return bitvec<N>(*this) <<= shift; }

        bitvec<N> operator >>(size_t shift) const { return bitvec<N>(*this) >>= shift; }

        size_t lsb() const { return this->lsbop(); }
        size_t msb() const { return this->msbop(); }
        size_t lsb(int notfound) const { return this->lsbop(notfound); }
        size_t msb(int notfound) const { return this->msbop(notfound); }
        size_t nextlsb(size_t prev, int notfound = -1) const { return this->nextlsbop(prev, notfound); }

        bitvec<N> insert(int i, int n, T v) const {
            bitvec<N> b(*this);
            b.insertop(i, n, v);
            b.sanitize();
            return b;
        }

        bitvec<N> accum(size_t i, size_t n, T v) const {
            bitvec<N> b(*this);
            b.accumop(i, n, v);
            return b;
        }

        bitvec<N> mask(size_t count) const {
            bitvec<N> b(*this);
            b.maskop(count);
            return b;
        }

        bitvec<N> operator %(size_t b) const {
            return mask(b);
        }

        bitvec<N> extract(size_t index, size_t count) const {
            return (bitvec<N>(*this) >> index) % count;
        }

        bitvec<N> operator ()(size_t index, size_t count) const {
            return extract(index, count);
        }

        bitvec<N> operator &(const bitvec<N>& y) const {
            return bitvec<N>(*this) &= y;
        }

        bitvec<N> operator |(const bitvec<N>& y) const {
            return bitvec<N>(*this) |= y;
        }

        bitvec<N> operator ^(const bitvec<N>& y) const {
            return bitvec<N>(*this) ^= y;
        }

        bitvec<N> remove(size_t index, size_t count = 1) {
            return (((*this) >> (index + count)) << index) | ((*this) % index);
        }

        template <int S> bitvec<S> subset(int i) const {
            return bitvec<S>((*this) >> i);
        }

        bitvec<N> swap(size_t i0, size_t i1) {
            bitvec<N>& v = *this;
            bool t = v[i0];
            v[i0] = v[i1];
            v[i1] = t;
            return v;
        }

        // This introduces ambiguity:
        // explicit operator unsigned long long() const { return integer(); }

        std::ostream& print(std::ostream& os) const {
            superstl_foreach (i, N) {
                os << (((*this)[i]) ? '1' : '0');
            }
            return os;
        }

        std::ostream& printhl(std::ostream& os) const {
            for (int i = N-1; i >= 0; i--) {
                os << (((*this)[i]) ? '1' : '0');
            }
            return os;
        }

    };

    template <size_t N>
    static inline std::ostream& operator <<(std::ostream& os, const bitvec<N>& v) {
        return v.print(os);
    }

    template <int size, typename T>
    static inline T vec_min_index(T* list, const bitvec<size>& include) {
        int minv = std::numeric_limits<T>::max;
        int mini = 0;
        superstl_foreach (i, size) {
            T v = list[i];
            bool ok = (v < minv) & include[i];
            minv = (ok) ? v : minv;
            mini = (ok) ? i : mini;
        }
        return mini;
    }

    template <int size, typename T, typename I>
    static inline void vec_make_sorting_permute_map(I* permute, T* list) {
        bitvec<size> include;
        include++;

        int n = 0;
        while (*include) {
            int mini = vec_min_index<size>(list, include);
            include[mini] = 0;
            assert(n < size);
            permute[n++] = mini;
        }
    }

#undef __builtin_ctzl
#undef __builtin_clzl


    // selflistlink class
    // Double linked list without pointer: useful as root
    // of inheritance hierarchy for another class to save
    // space, since object pointed to is implied
    //
    class selflistlink {
        public:
        selflistlink* next;
        selflistlink* prev;
        public:
        void reset() { next = NULL; prev = NULL; }
        selflistlink() { reset(); }
  
        selflistlink* unlink() {
            if superstl_likely (prev) prev->next = next;
            if superstl_likely (next) next->prev = prev;
            prev = NULL;
            next = NULL;
            return this;
        }
  
        selflistlink* replacewith(selflistlink* newlink) {
            if superstl_likely (prev) prev->next = newlink;
            if superstl_likely (next) next->prev = newlink;
            newlink->prev = prev;
            newlink->next = next;
            return newlink;
        }
  
        void addto(selflistlink*& root) {
            // THIS <-> root <-> a <-> b <-> c
            this->prev = (selflistlink*)&root;
            this->next = root;
            if superstl_likely (root) root->prev = this;
            // Do not touch root->next since it might not even exist
            root = this;
        }
  
        bool linked() const {
            return (next || prev);
        }
  
        bool unlinked() const {
            return !linked();
        }
    };
  
    static inline std::ostream& operator <<(std::ostream& os, const selflistlink& link) {
        os << "[prev " << link.prev << ", next " << link.next << "]";
        return os;
    }
  
    class selfqueuelink {
        public:
        selfqueuelink* next;
        selfqueuelink* prev;
        public:
        void reset() { next = this; prev = this; }
        selfqueuelink() { }
  
        selfqueuelink& unlink() {
            // No effect if next = prev = this (i.e., unlinked)
            next->prev = prev;
            prev->next = next;
            prev = this;
            next = this;
            return *this;
        }
  
        void addhead(selfqueuelink& root) {
            addlink(&root, root.next);
        }
  
        void addhead(selfqueuelink* root) {
            addhead(*root);
        }
  
        void addto(selfqueuelink& root) {
            addhead(root);
        }
  
        void addto(selfqueuelink* root) {
            addto(*root);
        }
  
        void addtail(selfqueuelink& root) {
            addlink(root.prev, &root);
        }
  
        void addtail(selfqueuelink* root) {
            addtail(*root);
        }
  
        selfqueuelink* removehead() {
            if superstl_unlikely (empty()) return NULL;
            selfqueuelink* link = next;
            link->unlink();
            return link;
        }
  
        selfqueuelink* removetail() {
            if superstl_unlikely (empty()) return NULL;
            selfqueuelink* link = prev;
            link->unlink();
            return link;
        }
  
        selfqueuelink* head() const {
            return next;
        }
  
        selfqueuelink* tail() const {
            return prev;
        }
  
        bool empty() const {
            return (next == this);
        }
  
        bool unlinked() const {
            return ((!prev && !next) || ((prev == this) && (next == this)));
        }
  
        bool linked() const {
            return !unlinked();
        }
  
        operator bool() const { return (!empty()); }
  
        protected:
        void addlink(selfqueuelink* prev, selfqueuelink* next) {
            next->prev = this;
            this->next = next;
            this->prev = prev;
            prev->next = this;
        }
    };

    template <typename T, int N>
    struct ChunkList {
        struct Chunk;

        struct Chunk {
            selflistlink link;
            bitvec<N> freemap;

            // Formula: (CHUNK_SIZE - sizeof(ChunkHeader<T>)) / sizeof(T);
            T data[N];

            Chunk() { link.reset(); freemap++; }

            bool full() const { return (!freemap); }
            bool empty() const { return freemap.allset(); }

            int add(const T& entry) {
                if superstl_unlikely (full()) return -1;
                int idx = freemap.lsb();
                freemap[idx] = 0;
                data[idx] = entry;
                return idx;
            }

            bool contains(T* entry) const {
                int idx = entry - data;
                return ((idx >= 0) & (idx < lengthof(data)));
            }

            bool remove(int idx) {
                data[idx] = 0;
                freemap[idx] = 1;

                return empty();
            }

            struct Iterator {
                Chunk* chunk;
                size_t i;

                Iterator() { }

                Iterator(Chunk* chunk_) {
                    reset(chunk_);
                }

                void reset(Chunk* chunk_) {
                    this->chunk = chunk_;
                    i = 0;
                }

                T* next() {
                    for (;;) {
                        if superstl_unlikely (i >= lengthof(chunk->data)) return NULL;
                        if superstl_unlikely (chunk->freemap[i]) { i++; continue; }
                        return &chunk->data[i++];
                    }
                }
            };

            int getentries(T* a, int limit) {
                Iterator iter(this);
                T* entry;
                int n = 0;
                while (entry = iter.next()) {
                    if superstl_unlikely (n >= limit) return n;
                    a[n++] = *entry;
                }

                return n;
            }
        };

        struct Locator {
            Chunk* chunk;
            int index;

            void reset() { chunk = NULL; index = 0; }
        };

        selflistlink* head;
        int elemcount;

        ChunkList() { head = NULL; elemcount = 0; }

        bool add(const T& entry, Locator& hint) {
            Chunk* chunk = (Chunk*)head;

            while (chunk) {
                prefetch(chunk->link.next);
                int index = chunk->add(entry);
                if superstl_likely (index >= 0) {
                    hint.chunk = chunk;
                    hint.index = index;
                    elemcount++;
                    return true;
                }
                chunk = (Chunk*)chunk->link.next;
            }

            Chunk* newchunk = new Chunk();
            newchunk->link.addto(head);

            int index = newchunk->add(entry);
            assert(index >= 0);

            hint.chunk = newchunk;
            hint.index = index;
            elemcount++;

            return true;
        }

        bool remove(const Locator& locator) {
            locator.chunk->remove(locator.index);
            elemcount--;

            if (locator.chunk->empty()) {
                locator.chunk->link.unlink();
                delete locator.chunk;
            }

            return empty();
        }

        void clear() {
            Chunk* chunk = (Chunk*)head;

            while (chunk) {
                Chunk* next = (Chunk*)chunk->link.next;
                prefetch(next);
                delete chunk;
                chunk = next;
            }

            elemcount = 0;
            head = NULL;
        }

        int count() { return elemcount; }

        bool empty() { return (elemcount == 0); }

        ~ChunkList() {
            clear();
        }

        struct Iterator {
            Chunk* chunk;
            Chunk* nextchunk;
            int i;

            Iterator() { }

            Iterator(ChunkList<T, N>* chunklist) {
                reset(chunklist);
            }

            void reset(ChunkList<T, N>* chunklist) {
                chunk = (Chunk*)chunklist->head;
                nextchunk = (chunk) ? (Chunk*)chunk->link.next : NULL;
                i = 0;
            }

            T* next() {
                for (;;) {
                    if superstl_unlikely (!chunk) return NULL;

                    if superstl_unlikely (i >= lengthof(chunk->data)) {
                        chunk = nextchunk;
                        if superstl_unlikely (!chunk) return NULL;
                        nextchunk = (Chunk*)chunk->link.next;
                        prefetch(nextchunk);
                        i = 0;
                    }

                    if superstl_unlikely (chunk->freemap[i]) { i++; continue; }

                    return &chunk->data[i++];
                }
            }
        };

        int getentries(T* a, int limit) {
            Iterator iter(this);
            T* entry;
            int n;
            while (entry = iter.next()) {
                if superstl_unlikely (n >= limit) return n;
                a[n++] = *entry;
            }

            return n;
        }
    };


    //
    // Index References (indexrefs) work exactly like pointers but always
    // index into a specific structure. This saves considerable space and
    // can allow aliasing optimizations not possible with pointers.
    //

    template <typename T, typename P = W32, Waddr base = 0, int granularity = 1>
    struct shortptr {
        P p;

        shortptr() { }

        shortptr(const T& obj) {
            *this = obj;
        }

        shortptr(const T* obj) {
            *this = obj;
        }

        shortptr<T, P, base, granularity>& operator =(const T& obj) {
            p = (P)((((Waddr)&obj) - base) / granularity);
            return *this;
        }

        shortptr<T, P, base, granularity>& operator =(const T* obj) {
            p = (P)((((Waddr)obj) - base) / granularity);
            return *this;
        }

        T* get() const {
            return (T*)((p * granularity) + base);
        }

        T* operator ->() const {
            return get();
        }

        T& operator *() const {
            return *get();
        }

        operator T*() const { return get(); }

        shortptr<T, P, base, granularity>& operator ++() {
            (*this) = (get() + 1);
            return *this;
        }

        shortptr<T, P, base, granularity>& operator --() {
            (*this) = (get() - 1);
            return *this;
        }
    };

}

#endif
