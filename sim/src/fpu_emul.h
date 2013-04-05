// fp_emul.h (Floating point emulation facilities)
// 
// Author : Vikas Chouhan (presentisgood@gmail.com)
// Copyright 2012.
// 
// This file is part of ppc-sim library.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.
// 
// It is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file; see the file COPYING.  If not, write to the
// Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef    _FPU_EMUL_H_
#define    _FPU_EMUL_H_

#include "config.h"

namespace fp_emul{
    // FP operation type
    enum fp_op {
        fp_op_add = 1,
        fp_op_sub,
        fp_op_mul,
        fp_op_div,
        fp_op_d2s,
        fp_op_s2d,
        fp_op_c2ui,
        fp_op_c2uf,
        fp_op_inv
    };

    static const char* fp_op_str[] = { "INV", "fp_add", "fp_sub", "fp_mul", "fp_div", "fp_d2s", "fp_s2d" };

    // FP operand type
    enum fp_operand {
        fp_operand_inf = 1,
        fp_operand_norm,
        fp_operand_denorm,
        fp_operand_nan,
        fp_operand_zero,
        fp_operand_amax,
        fp_operand_bmax,
        fp_operand_a,
        fp_operand_b,
        fp_operand_minus_a,
        fp_operand_minus_b,
        fp_operand_minus_amax,
        fp_operand_minus_bmax,
        fp_operand_max,
        fp_operand_inv
    };

    static const char* fp_operand_str[] = { "INV", "inf", "norm", "denorm", "NAN", "zero", "amax", "bmax", "a", "b",
        "-a", "-b", "-amax", "-bmax", "max" };

    // one of these (if enabled), decides the final sign of result
    enum fp_sign_type {
        fp_sign_type_1 = 1,
        fp_sign_type_2,
        fp_sign_type_3,
        fp_sign_type_5,
        fp_sign_type_6,
        fp_sign_type_inv         // invalid
    };

    // Define floating point traits
    //
    template<typename T>
    struct fp_traits {};

    template<>
    struct fp_traits<uint32_t>{
        typedef uint32_t  fp_type;

        //individual components
        static const fp_type sign_mask  = 0x80000000;                     // sign mask
        static const fp_type exp_mask   = 0x7f800000;                     // biased exponent mask
        static const fp_type mant_mask  = ~(sign_mask | exp_mask);        // mantissa mask
        static const fp_type exp_max    = exp_mask >> RSHIFT<exp_mask>::VAL;

        static const fp_type plus_inf   = 0x7f800000;
        static const fp_type minus_inf  = 0xff800000;
        static const fp_type inf        = plus_inf;

        static const fp_type plus_zero  = 0x00000000;
        static const fp_type minus_zero = 0x80000000;
        static const fp_type zero       = plus_zero;

        static const fp_type plus_max   = 0x7f7fffff;            // maximum normalized +ve single precision no
        static const fp_type minus_max  = 0xff7fffff;            // maximum normalized -ve single precision no
        static const fp_type max        = plus_max;

        static const fp_type plus_min   = 0x00800000;            // minimum normalized +ve single precision no
        static const fp_type minus_min  = 0x80800000;            // minimum normalized -ve single precision no
        static const fp_type min        = plus_min;

        // NAN is defined as masks, wherein all exponent bits are 1, & significant is a non-zero value.
        // sign bit can be either 0 or 1.
        static const fp_type nan_mask     = plus_inf;

        static inline bool is_inf(uint32_t x)    { return ((x & plus_inf) == plus_inf); }
        static inline bool is_nan(uint32_t x)    { return ((x & ~nan_mask) && ((x & nan_mask) == nan_mask)); }
        static inline bool is_denorm(uint32_t x) { return !(x & exp_mask) && (x & mant_mask); }
        static inline bool is_zero(uint32_t x)   { return !(x & ~minus_zero); }
        static inline bool is_norm(uint32_t x)   { fp_type expn = (x & exp_mask); return (expn > 0 and expn < exp_max); }
    };

    template<>
    struct fp_traits<uint64_t>{
        typedef uint64_t fp_type;

        // individual components
        static const fp_type sign_mask  = 0x8000000000000000ULL;       // sign mask
        static const fp_type exp_mask   = 0x7ff0000000000000ULL;       // biased exponent mask
        static const fp_type mant_mask  = ~(sign_mask | exp_mask);     // mantissa mask
        static const fp_type exp_max    = exp_mask >> RSHIFT<exp_mask>::VAL;

        static const fp_type plus_inf   = 0x7ff0000000000000ULL;
        static const fp_type minus_inf  = 0xfff0000000000000ULL;
        static const fp_type inf        = plus_inf;

        static const fp_type plus_zero  = 0x0000000000000000ULL;
        static const fp_type minus_zero = 0x8000000000000000ULL;
        static const fp_type zero       = plus_zero;

        static const fp_type plus_max   = 0x7fefffffffffffffULL;
        static const fp_type minus_max  = 0xffefffffffffffffULL;
        static const fp_type max        = plus_max;

        static const fp_type plus_min   = 0x0010000000000000ULL;
        static const fp_type minus_min  = 0x8010000000000000ULL;
        static const fp_type min        = plus_min;

        static const fp_type nan_mask     = plus_inf;

        static inline bool is_inf(uint64_t x)    { return ((x & plus_inf) == plus_inf); }
        static inline bool is_nan(uint64_t x)    { return ((x & ~nan_mask) && ((x & nan_mask) == nan_mask)); }
        static inline bool is_denorm(uint64_t x) { return !(x & exp_mask) && (x & mant_mask); }
        static inline bool is_zero(uint64_t x)   { return !(x & ~minus_zero); }
        static inline bool is_norm(uint64_t x)   { fp_type expn = (x & exp_mask); return (expn > 0 and expn < exp_max); }
    };

#define MP(a, b)   std::make_pair(a, b)
   
    typedef std::pair<fp_operand, fp_operand>     fp_res_lut_type1;
    typedef std::pair<fp_operand, fp_sign_type>   fp_res_lut_type2;
    static const std::map<fp_op, std::map<fp_res_lut_type1, fp_res_lut_type2> >  fp_map =
        { {
              fp_op_add,
              {
                    { MP(fp_operand_inf,    fp_operand_inf),        MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_nan),        MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_denorm),     MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_zero),       MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_norm),       MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_inf),        MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_nan),        MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_denorm),     MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_zero),       MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_norm),       MP(fp_operand_amax, fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_inf),        MP(fp_operand_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_nan),        MP(fp_operand_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_denorm),     MP(fp_operand_zero, fp_sign_type_1)   },       //1
                    { MP(fp_operand_denorm, fp_operand_zero),       MP(fp_operand_zero, fp_sign_type_1)   },       //1
                    { MP(fp_operand_denorm, fp_operand_norm),       MP(fp_operand_b,    fp_sign_type_inv) },       //4
                    { MP(fp_operand_zero,   fp_operand_inf),        MP(fp_operand_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_zero,   fp_operand_nan),        MP(fp_operand_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_zero,   fp_operand_denorm),     MP(fp_operand_zero, fp_sign_type_1)   },       //1
                    { MP(fp_operand_zero,   fp_operand_zero),       MP(fp_operand_zero, fp_sign_type_1)   },       //1
                    { MP(fp_operand_zero,   fp_operand_norm),       MP(fp_operand_b,    fp_sign_type_inv) },       //4
                    { MP(fp_operand_norm,   fp_operand_inf),        MP(fp_operand_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_norm,   fp_operand_nan),        MP(fp_operand_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_norm,   fp_operand_denorm),     MP(fp_operand_a,    fp_sign_type_inv) },       //4
                    { MP(fp_operand_norm,   fp_operand_zero),       MP(fp_operand_a,    fp_sign_type_inv) },       //4
              }
          },
          {
              fp_op_sub,
              {
                    { MP(fp_operand_inf,    fp_operand_inf),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_nan),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_denorm),     MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_zero),       MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_inf,    fp_operand_norm),       MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_inf),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_nan),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_denorm),     MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_zero),       MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_norm),       MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_inf),        MP(fp_operand_minus_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_nan),        MP(fp_operand_minus_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_2)   },
                    { MP(fp_operand_denorm, fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_2)   },
                    { MP(fp_operand_denorm, fp_operand_norm),       MP(fp_operand_minus_b,    fp_sign_type_inv) },     //4
                    { MP(fp_operand_zero,   fp_operand_inf),        MP(fp_operand_minus_bmax, fp_sign_type_inv) },     //4
                    { MP(fp_operand_zero,   fp_operand_nan),        MP(fp_operand_minus_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_zero,   fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_2)   },
                    { MP(fp_operand_zero,   fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_2)   },
                    { MP(fp_operand_zero,   fp_operand_norm),       MP(fp_operand_minus_b,    fp_sign_type_inv) },
                    { MP(fp_operand_norm,   fp_operand_inf),        MP(fp_operand_minus_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_norm,   fp_operand_nan),        MP(fp_operand_minus_bmax, fp_sign_type_inv) },
                    { MP(fp_operand_norm,   fp_operand_denorm),     MP(fp_operand_a,          fp_sign_type_inv) },       //4
                    { MP(fp_operand_norm,   fp_operand_zero),       MP(fp_operand_a,          fp_sign_type_inv) },       //4

              }
          },
          {
              fp_op_mul,
              {
                    { MP(fp_operand_inf,    fp_operand_inf),        MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_nan),        MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_norm),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_inf),        MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_nan),        MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_norm),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_norm),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_norm),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_inf),        MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_nan),        MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_denorm),     MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_zero),       MP(fp_operand_zero,       fp_sign_type_3) },

              }
          },
          {
              fp_op_div,
              {
                    { MP(fp_operand_inf,    fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_denorm),     MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_zero),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_inf,    fp_operand_norm),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_denorm),     MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_zero),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_nan,    fp_operand_norm),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_denorm),     MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_zero),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_denorm, fp_operand_norm),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_denorm),     MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_zero),       MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_zero,   fp_operand_norm),       MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_inf),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_nan),        MP(fp_operand_zero,       fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_denorm),     MP(fp_operand_max,        fp_sign_type_3) },
                    { MP(fp_operand_norm,   fp_operand_zero),       MP(fp_operand_max,        fp_sign_type_3) },
              }
          },
          {
              fp_op_d2s,
              {
                    { MP(fp_operand_inf,    fp_operand_inv),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_inv),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_inv),        MP(fp_operand_zero,       fp_sign_type_5)   },
                    { MP(fp_operand_zero,   fp_operand_inv),        MP(fp_operand_zero,       fp_sign_type_5)   },
              }
          },
          {
              fp_op_s2d,
              {
                    { MP(fp_operand_inf,    fp_operand_inv),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_nan,    fp_operand_inv),        MP(fp_operand_amax,       fp_sign_type_inv) },
                    { MP(fp_operand_denorm, fp_operand_inv),        MP(fp_operand_zero,       fp_sign_type_5),  },
                    { MP(fp_operand_zero,   fp_operand_inv),        MP(fp_operand_zero,       fp_sign_type_5),  },
              }
          },
        };

#undef MP

    // extract operand types
    template<typename T>
    static inline fp_operand get_operand_type(T x){
        if(fp_traits<T>::is_inf(x))          { return fp_operand_inf;    }
        else if(fp_traits<T>::is_zero(x))    { return fp_operand_zero;   }
        else if(fp_traits<T>::is_nan(x))     { return fp_operand_nan;    }
        else if(fp_traits<T>::is_denorm(x))  { return fp_operand_denorm; }
        else                                 { return fp_operand_norm;   }
    }

    // 2 operand version
    template<typename T>
    static inline T get_operand(T &x, T &y, std::pair<fp_operand, fp_sign_type>& v, bool round_to_minus_inf=false){
        T  new_sign_mask;
        T  sign_mask = fp_traits<T>::sign_mask;

        // check if any special override is necessary regarding sign of the result
        switch(v.second){
            case fp_sign_type_inv      : new_sign_mask = 0;
                                         break;
            case fp_sign_type_1        : new_sign_mask = ((x & sign_mask) ^ (y & sign_mask) && !round_to_minus_inf) ? 0:sign_mask;
                                         break;
            case fp_sign_type_2        : new_sign_mask = ((x & sign_mask) ^ (y & sign_mask) && !round_to_minus_inf) ? sign_mask:0;
                                         break;
            case fp_sign_type_3        : new_sign_mask = (x & sign_mask) ^ (y & sign_mask);
                                         break;
            default                    : new_sign_mask = 0;
        }

        // populate default result
        switch(v.first){
            case fp_operand_amax       : return fp_traits<T>::max | (x & fp_traits<T>::sign_mask) | new_sign_mask;
            case fp_operand_bmax       : return fp_traits<T>::max | (y & fp_traits<T>::sign_mask) | new_sign_mask;
            case fp_operand_a          : return x | new_sign_mask;
            case fp_operand_b          : return y | new_sign_mask;
            case fp_operand_zero       : return fp_traits<T>::zero | new_sign_mask;
            case fp_operand_minus_amax : return -(fp_traits<T>::max | (x & fp_traits<T>::sign_mask) | new_sign_mask);
            case fp_operand_minus_bmax : return -(fp_traits<T>::max | (y & fp_traits<T>::sign_mask) | new_sign_mask);
            default                    : return fp_traits<T>::zero;       // should never come here.
        }
    }

    // 1 operand version
    template<typename T>
    static inline T get_operand(T &x, std::pair<fp_operand, fp_sign_type>& v){
        T  new_sign_mask;
        T  sign_mask = fp_traits<T>::sign_mask;

        // check if any special override is necessary regarding sign of the result
        switch(v.second){
            case fp_sign_type_inv      : new_sign_mask = 0;
                                         break;
            case fp_sign_type_5        : new_sign_mask = (x & sign_mask);
                                         break;
            case fp_sign_type_6        : new_sign_mask = (-x & sign_mask);
                                         break;
            default                    : new_sign_mask = 0;
        }

        // populate default result
        switch(v.first){
            case fp_operand_amax       : return fp_traits<T>::max | (x & fp_traits<T>::sign_mask) | new_sign_mask;
            case fp_operand_a          : return x | new_sign_mask;
            case fp_operand_zero       : return fp_traits<T>::zero | new_sign_mask;
            case fp_operand_minus_amax : return -(fp_traits<T>::max | (x & fp_traits<T>::sign_mask) | new_sign_mask);
            default                    : return fp_traits<T>::zero;       // should never come here.
        }
    }

    // get default results (main function)
    // 2 operand version
    template<typename T>
    inline T get_default_results(T& R, fp_op op, T a, T b, x86_mxcsr& m){
        if(m.ie){
            std::pair<fp_operand, fp_sign_type>  res = fp_map.at(op).at(std::make_pair(get_operand_type<T>(a), get_operand_type<T>(b)));
            R = get_operand<T>(a, b, res, m.is_rounding_mode_rm());
        }
        if(m.ue){
            if(m.is_rounding_mode_rnzp()){ R = fp_traits<T>::zero; }
            else                         { R = fp_traits<T>::minus_zero; }
        }
        return R;   // shouldn't fall back to this
    }

    // 1 operand version
    template<typename T>
    inline T get_default_results(T& R, fp_op op, T a, x86_mxcsr& m){
        if(m.ie){
            std::pair<fp_operand, fp_sign_type>  res = fp_map.at(op).at(std::make_pair(get_operand_type<T>(a), fp_operand_inv));
            R = get_operand<T>(a, res);
        }
        return R;   // shouldn't fallback to this
    }
}

#endif    /*  FLOAT_EMUL_H  */
