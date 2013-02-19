/*
 * ------------------------------------------------------------------------------------
 *  PowerPC BookE RTL description.
 * ------------------------------------------------------------------------------------
 *
 *  Acknowledgement :
 *  This file format was taken from a similar file from "gxemu" project by
 *  Anders Gavare, but it doesn't contain any of the code in that file.
 *
 *  -----------------------------------------------------------------------------------
 *  Author : Vikas Chouhan (presentisgood@gmail.com)
 *  Copyright (C) 2012.
 *  
 *  e500v2 is taken as a reference, but the code is generic and can be used for higher
 *  booke 64 bit cores.
 *
 *  NOTE:
 *  This file is not a regular C/C++ file ( although it looks like it :) ).
 *  It's just a template for defining instructions'
 *  RTL(register transfer logic) implementation. An external utility parses
 *  this template to generate a new header/C++ file before direct use.
 *
 *  RTL is written in accordance with Power ISA 2.06 provided by power.org and
 *  PowerPC e500 core ref. manual provided by Freescale.
 *
 */

/* ---------
 * Defines 
 * ---------
 */

//
// NOTE: 1. CPU and IC should come from the parsing utility.
//       2. update_xer() updates all bits of XER. Some freescale ppc instrs only update CA, others only update
//          SO and OV at a time. To update only XER[CA] use update_xer_ca()
//
// TERMINOLOGIES :
//       1. byte means 8 bits.
//       2. half-word means 16 bits.
//       3. word means 32 bits.
//       4. double/double-word means 64 bits.

// Global defines for this template file
//
#undef  PPCREG
#undef  PPCREGATTR
#undef  PPCREGN
#undef  PPCREGNATTR

#undef  SPR
#undef  XER 
#undef  MSR
#undef  PMR 
#undef  CR 
#undef  update_cr0
#undef  update_crF  
#undef  update_crf  
#undef  update_xer  
#undef  get_xer_so  
#undef  get_crf     
#undef  get_crF     
#undef  host_flags  
#undef  dummy_flags 

//#define Rshift(bmask)            ({int res=0; while(!(bmask & 0x1)){ res++; bmask>>=1; } res;})
//#ifndef EBMASK
//#define EBMASK(reg, bmask)       (((reg)  & (bmask)) >> Rshift(bmask))
//#endif

// Alias to CPU functions
#define PPCREG(regid)            (CPU->reg(regid)->value.u64v)
#define PPCREGN(reg_name)        (CPU->regn(reg_name)->value.u64v)

// MSR bits
#define MSR_CM                   ((PPCREG(REG_MSR) & MSR_CM) ? 1 : 0)

#define GPR(gprno)               PPCREG(REG_GPR0 + gprno)
#define SPR(sprno)               PPCREG(REG_SPR0 + sprno)
#define XER                      SPR(SPRN_XER) 
#define MSR                      PPCREG(REG_MSR)
#define ACC                      PPCREG(REG_ACC)
#define SPEFSCR                  PPCREG(REG_SPEFSCR)
#define PMR(pmrno)               PPCREG(REG_PMR0 + pmrno) 
#define CR                       PPCREG(REG_CR)
#define LR                       PPCREG(REG_LR)
#define CTR                      PPCREG(REG_CTR)
#define SRR0                     PPCREG(REG_SRR0)
#define SRR1                     PPCREG(REG_SRR1)
#define CSRR0                    PPCREG(REG_CSRR0)
#define CSRR1                    PPCREG(REG_CSRR1)
#define MCSRR0                   PPCREG(REG_MCSRR0)
#define MCSRR1                   PPCREG(REG_MCSRR1)
#define PC                       CPU->m_pc
#define NIP                      CPU->m_nip
#define update_cr0               CPU->update_cr0
#define update_cr0_host          CPU->update_cr0_host
#define update_crF               CPU->update_crF
#define update_crf               CPU->update_crf
#define update_xer               CPU->update_xer
#define update_xer_host          CPU->update_xer_host
#define update_xer_so_ov         CPU->update_xer_so_ov
#define update_xer_ca            CPU->update_xer_ca
#define update_xer_ca_host       CPU->update_xer_ca_host
#define get_xer_so               CPU->get_xer_so
#define get_crf                  CPU->get_crf
#define get_crF                  CPU->get_crF
#define get_xerF                 CPU->get_xerF
#define get_xerf                 CPU->get_xerf
#define get_xer_so               CPU->get_xer_so
#define get_xer_ca               CPU->get_xer_ca
#define HOST_FLAGS               CPU->host_flags

// Host flags
#define GET_CF_H()               CPU->host_flags.cf       // x86 carry flag
#define GET_OF_H()               CPU->host_flags.of       // x86 overflow flag

// Generic macros
#define UPDATE_CA()               update_xer_ca_host()
#define UPDATE_SO_OV()            update_xer_host()
#define UPDATE_SO_OV_V            update_xer_so_ov 
#define UPDATE_CR0()              update_cr0_host()
#define GET_CA()                  get_xer_ca()
#define GET_SO()                  get_xer_so()
#define UPDATE_CR0_V              update_cr0

#define UPDATE_CYCLES(n)         CPU->m_ncycles += n

// load/store macros
//
#define LOAD8(addr)              CPU->read8(addr)
#define LOAD16(addr)             CPU->read16(addr)
#define LOAD32(addr)             CPU->read32(addr)
#define LOAD64(addr)             CPU->read64(addr)

#define STORE8(addr, value)      CPU->write8(addr, value)
#define STORE16(addr, value)     CPU->write16(addr, value)
#define STORE32(addr, value)     CPU->write32(addr, value)
#define STORE64(addr, value)     CPU->write64(addr, value)

// Reservation macros
#define SET_RESV(ea, size)       CPU->set_resv(ea, size)
#define CHECK_RESV(ea, size)     CPU->check_resv(ea, size)
#define CLEAR_RESV(ea)           CPU->clear_resv(ea)

// TLB macros
#define TLBWE()                  CPU->m_l2tlb.tlbwe(PPCREG(REG_MAS0), \
                                     PPCREG(REG_MAS1), PPCREG(REG_MAS2), PPCREG(REG_MAS3), PPCREG(REG_MAS7), PPCREG(REG_HID0))
#define TLBRE()                  CPU->m_l2tlb.tlbre(PPCREG(REG_MAS0), \
                                     PPCREG(REG_MAS1), PPCREG(REG_MAS2), PPCREG(REG_MAS3), PPCREG(REG_MAS7), PPCREG(REG_HID0))
#define TLBIVAX(ea)              CPU->m_l2tlb.tlbive(ea)
#define TLBSX(ea)                CPU->m_l2tlb.tlbse(ea, PPCREG(REG_MAS0), \
                                     PPCREG(REG_MAS1), PPCREG(REG_MAS2), PPCREG(REG_MAS3), PPCREG(REG_MAS6), PPCREG(REG_MAS7), PPCREG(REG_HID0))

// In case of register operands, their pointers are instead passed in corresponding ARG parameter.
#define ARG_BASE                 IC->arg

#define REG0_P                   (ARG_BASE[0].p)
#define REG1_P                   (ARG_BASE[1].p)
#define REG2_P                   (ARG_BASE[2].p)
#define REG3_P                   (ARG_BASE[3].p)
#define REG4_P                   (ARG_BASE[4].p)

#define REG0                     PPCREG(REG0_P)
#define REG1                     PPCREG(REG1_P)
#define REG2                     PPCREG(REG2_P)
#define REG3                     PPCREG(REG3_P)
#define REG4                     PPCREG(REG4_P)

#define ARG0                     (ARG_BASE[0].v)
#define ARG1                     (ARG_BASE[1].v)
#define ARG2                     (ARG_BASE[2].v)
#define ARG3                     (ARG_BASE[3].v)
#define ARG4                     (ARG_BASE[4].v)

// target mode , UT -> unsigned target, st -> signed target
// If target_bits == 32
//     UMODE -> uint32_t
//     SMODE -> int32_t
// else if target_bits == 64
//     UMODE -> uint64_t
//     SMODE -> int64_t
// endif    
#define UT(arg)   static_cast<UMODE>((arg))
#define ST(arg)   static_cast<SMODE>((arg))
#define U64(arg)  static_cast<uint64_t>((arg))
#define S64(arg)  static_cast<int64_t>((arg))
#define U32(arg)  static_cast<uint32_t>((arg))
#define S32(arg)  static_cast<int32_t>((arg))
#define U16(arg)  static_cast<uint16_t>((arg))
#define S16(arg)  static_cast<int16_t>((arg))
#define U8(arg)   static_cast<uint8_t>((arg))
#define S8(arg)   static_cast<int8_t>((arg))
#define BOOL(arg) static_cast<bool>((arg))

// for sign extension ( extending sign bits from source type to destination type )
// sign_exts<> function comes from utils.h
#define EXTS_B2N(val)              sign_exts<SMODE, int8_t >(val)               // sign extension : byte to native
#define EXTS_H2N(val)              sign_exts<SMODE, int16_t>(val)               // sign extension : half word to native
#define EXTS_W2N(val)              sign_exts<SMODE, int32_t>(val)               // sign extension : word to native

#define EXTS_H2W(val)              sign_exts<int32_t, int16_t>(val)             // sign extension : half to word
#define EXTS_B2W(val)              sign_exts<int32_t, int8_t >(val)             // sign extension : byte to word
#define EXTS_B2H(val)              sign_exts<int16_t, int8_t >(val)             // sign extension : byte to half-word

#define EXTS_B2D(val)              sign_exts<int64_t, int8_t >(val)             // sign extension : byte to double-word
#define EXTS_H2D(val)              sign_exts<int64_t, int16_t>(val)             // sign extension : half-word to double-word
#define EXTS_W2D(val)              sign_exts<int64_t, int32_t>(val)             // sign extension : word to double-word

// special case
#define EXTS_5B_2_32B(val)         (((val) & 0x10) ? ((val) | 0xffffffe0):(val))           // sign extension : 5 bits -> 32 bits
#define EXTS_5B_2_64B(val)         (((val) & 0x10) ? ((val) | 0xffffffffffffffe0):(val))   // sign extension : 5 bits -> 64 bits

#define EXTS_BF2D                  sign_exts_f<uint64_t, 1>
#define EXTS_BF2W                  sign_exts_f<uint32_t, 1>
#define EXTZ_BF2D                  sign_exts_f<uint64_t, 0>
#define EXTZ_BF2W                  sign_exts_f<uint32_t, 0>

// zero extension
#define EXTZ_B2N(val)              sign_exts<UMODE, uint8_t >(val)
#define EXTZ_H2N(val)              sign_exts<UMODE, uint16_t>(val)
#define EXTZ_W2N(val)              sign_exts<UMODE, uint32_t>(val)

#define EXTZ_H2W(val)              sign_exts<uint32_t, uint16_t>(val)
#define EXTZ_B2W(val)              sign_exts<uint32_t, uint8_t >(val)
#define EXTZ_B2H(val)              sign_exts<uint16_t, uint8_t >(val)

#define EXTZ_B2D(val)              sign_exts<uint64_t, uint8_t >(val)
#define EXTZ_H2D(val)              sign_exts<uint64_t, uint16_t>(val)
#define EXTZ_W2D(val)              sign_exts<uint64_t, uint32_t>(val)

// rotation macros
#define ROTL64(x, y)               rotl<uint64_t>(x, y)             // x=64bit, y=int
#define ROTL32(x, y)               rotl<uint32_t>(x, y)             // x=32bit, y=int

// mask
#define BITMASK(x)                 gen_bmask<uint64_t>(x)             // 64 bit bitmask for bitpos x
#define MASK(x, y)                 gen_bmask_rng<uint64_t>(x, y)      // Generate mask of 1's from x to y

// 32_63 (x should be 64 bits)
// extracted bits would be casted to appropriate data type
#define B_32_63(x)                 U32((x) & 0xffffffff)                    // get bits 32:63
#define B_48_63(x)                 U16((x) & 0xffff)                        // get bits 48:63
#define B_56_63(x)                 U8((x) & 0xff)                           // get bits 56:63
#define B_0_31(x)                  U32(((x) >> 32) & 0xffffffff)            // get bits 0:31
#define B_16_31(x)                 U16(((x) >> 32) & 0xffff)                // get bits 16:31
#define B_24_31(x)                 U8(((x) >> 32) & 0xff)                   // get bits 24:31
#define B_32_47(x)                 U16((x >> 16)   & 0xffff)                // get bits 32:47
#define B_0_15(x)                  U16((x >> 48)   & 0xffff)                // get bits 0:15
#define B_N(x, n)                  BOOL(((x) >> (63 - (n))) & 0x1)          // get bit n

// special cases
#define B_26_31(x)                 (((x) >> 32) & 0x3f)
#define B_27_31(x)                 (((x) >> 32) & 0x1f)
#define B_58_63(x)                 ((x) & 0x3f)
#define B_59_63(x)                 ((x) & 0x1f)

// Byte reversing macros
#define SWAPB32(data)              ((((data >> 24) & 0xff) <<  0) ||  \
                                    (((data >> 16) & 0xff) <<  8) ||  \
                                    (((data >>  8) & 0xff) << 16) ||  \
                                    (((data >>  0) & 0xff) << 24) )
#define SWAPB16(data)              ((((data >>  8) & 0xff) <<  0) ||  \
                                    (((data >>  0) & 0xff) <<  8) )

// PPC Exceptions
#define  PPC_EXCEPT         sim_except_ppc

// These functions are defined in utils.h
#define X86_ADDW(arg1,    arg2)                      ADDW(arg1,   arg2, HOST_FLAGS)
#define X86_SUBW(arg1,    arg2)                      ADDW(arg1,   arg2, HOST_FLAGS)
#define X86_ANDW(arg1,    arg2)                      ANDW(arg1,   arg2, HOST_FLAGS)
#define X86_ORW(arg1,     arg2)                       ORW(arg1,   arg2, HOST_FLAGS)
#define X86_XORW(arg1,    arg2)                      XORW(arg1,   arg2, HOST_FLAGS)
#define X86_NEGW(arg1         )                      NEGW(arg1,   HOST_FLAGS)
#define X86_DIVW(arg1,    arg2)                      DIVW(arg1,   arg2)
#define X86_DIVUW(arg1,   arg2)                      DIVUW(arg1,  arg2)
#define X86_MULW_L(arg1,  arg2)                      MULW(arg1,   arg2, HOST_FLAGS, 0)
#define X86_MULW_H(arg1,  arg2)                      MULW(arg1,   arg2, HOST_FLAGS, 1)
#define X86_MULUW_L(arg1, arg2)                      MULUW(arg1,  arg2, HOST_FLAGS, 0)
#define X86_MULUW_H(arg1, arg2)                      MULUW(arg1,  arg2, HOST_FLAGS, 1)
#define X86_MULWF(arg1,   arg2)                      MULWF(arg1,  arg2, HOST_FLAGS)
#define X86_MULUWF(arg1,  arg2)                      MULUWF(arg1, arg2, HOST_FLAGS)

#define X86_ADD64(arg1,   arg2)                      ADD64(arg1,  arg2, HOST_FLAGS)           // 64bit ADD
#define X86_SUB64(arg1,   arg2)                      SUB64(arg1,  arg2, HOST_FLAGS)           // 64bit SUB

// START
// ------------------------------------- INTEGER ARITHMETIC -------------------------
// mnemonics :
//             add    ( add., addo, addo. )
//             addc   ( addc., addco, addco. )
//             adde   ( adde., addeo, addeo. )
//             addi
//             addic  ( addic. )
//             addis
//             addme  ( addme., addmeo, addmeo. )
//             addze  ( addze., addzeo, addzeo. )
//             subf   ( subf., subfo, subfo. )
//             subfc  ( subfc., subfco, subfco. )
//             subfe  ( subfe., subfeo, subfeo. )
//             subfic
//             subfme ( subfme., subfmeo, subfmeo. )
//             subfze ( subfze., subfzeo, subfzeo. )
//             divw   ( divw., divwo, divwo. )
//             divwu  ( divwu., divwuo, divwuo. )
//             mulhw  ( mulhw. )
//             mulhwu ( mulhwu. )
//             mulli
//             mullw  ( mullw., mullwo, mullwo. )

X(add)
{
#define add_code(rD, rA, rB)           \
    rD = X86_ADDW(rA, rB);

    add_code(REG0, REG1, REG2);
}
X(add.)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X(addo)
{
    add_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X(addo.)
{
    add_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

// These instrs always update XER[CA] flag
X(addc)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
}
X(addc.)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_CR0();
}
X(addco)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X(addco.)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

/* Add extended : rA + rB + CA */
X(adde)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X(adde.)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}
X(addeo)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X(addeo.)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X(addi)
{
    SMODE tmp = (int16_t)ARG2;
    if(ARG1){ add_code(REG0, REG1, tmp);        }
    else    { REG0 = (int16_t)ARG2;             }
}

X(addic)
{
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X(addic.)
{
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}

X(addis)
{
    SMODE tmp = (((int16_t)ARG2) << 16);
    if(ARG1){ add_code(REG0, REG1, tmp);        }
    else    { REG0 = ((int16_t)ARG2) << 16;     }
}

// XER[CA] is always altered
X(addme)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X(addme.)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}
X(addmeo)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X(addmeo.)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

// XER[CA] is always altered
X(addze)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X(addze.)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}
X(addzeo)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X(addzeo.)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X(subf)
{
#define subf_code(rD, rA, rB)            \
    uint64_t subf_tmp = rB + 1;          \
    uint64_t inva = ~rA;                 \
    add_code(rD, inva, subf_tmp)

    subf_code(REG0, REG1, REG2);
}
X(subf.)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X(subfo)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X(subfo.)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

// XER[CA] is altered
X(subfic)
{
    uint64_t tmp = EXTS_H2N(ARG2);
    subf_code(REG0, REG1, tmp);
    UPDATE_CA();
}

// XER[CA] is always altered
X(subfc)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
}
X(subfc.)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_CR0();
}
X(subfco)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X(subfco.)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X(subfe)
{
#define subfe_code(rD, rA, rB)          \
    UMODE tmp = GET_CA() + rB;          \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfe_code(REG0, REG1, REG2);
}
X(subfe.)
{
    subfe_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X(subfeo)
{
    subfe_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X(subfeo.)
{
    subfe_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X(subfme)
{
#define subfme_code(rD, rA)             \
    SMODE tmp = GET_CA() - 1;           \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfme_code(REG0, REG1);
}
X(subfme.)
{
    subfme_code(REG0, REG1);
    UPDATE_CR0();
}
X(subfmeo)
{
    subfme_code(REG0, REG1);
    UPDATE_SO_OV();
}
X(subfmeo.)
{
    subfme_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X(subfze)
{
#define subfze_code(rD, rA)             \
    UMODE tmp = GET_CA();               \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfze_code(REG0, REG1);
}
X(subfze.)
{
    subfze_code(REG0, REG1);
    UPDATE_CR0();
}
X(subfzeo)
{
    subfze_code(REG0, REG1);
    UPDATE_SO_OV();
}
X(subfzeo.)
{
    subfze_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X(divw)
{
#define divw_code(rD, rA, rB)          \
    if(rB){ rD = X86_DIVW(rA, rB); }

    divw_code(REG0, REG1, REG2);
}
X(divw.)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X(divwo)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
}
X(divwo.)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    UPDATE_CR0_V(REG0);
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
}

X(divwu)
{
#define divwu_code(rD, rA, rB)         \
    if(rB){ rD = X86_DIVUW(rA, rB); }

    divwu_code(REG0, REG1, REG2);
}
X(divwu.)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X(divwuo)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }   // Set OV=1 if division by zero
}
X(divwuo.)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    UPDATE_CR0_V(REG0);
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }
}

X(mulhw)
{
#define mulhw_code(rD, rA, rB)            \
    rD = X86_MULW_H(rA, rB)

    mulhw_code(REG0, REG1, REG2);
}
X(mulhw.)
{
    mulhw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}

X(mulli)
{
    uint32_t imm_val = EXTS_H2W(ARG2);          // Sign extend 16 bit IMM to 32 bit
    REG0 = X86_MULW_L(REG1, imm_val);
}

X(mulhwu)
{
#define mulhwu_code(rD, rA, rB)           \
    rD = X86_MULUW_H(rA, rB)

    mulhwu_code(REG0, REG1, REG2);
}
X(mulhwu.)
{
    mulhwu_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);                 // muluw doesn't set SF or ZF, hence we have to set CR0 by value
}

X(mullw)
{
#define mullw_code(rD, rA, rB)            \
    rD = X86_MULW_L(rA, rB)

    mullw_code(REG0, REG1, REG2);
}
X(mullw.)
{
    mullw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X(mullwo)
{
    mullw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X(mullwo.)
{
    mullw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0_V(REG0);
}


// START
// ---------------------------------- INTEGER LOGICAL -------------------------------
// mnemonics :
//               and       ( and. )
//               andi.
//               andis.
//               andc      ( andc. )
//               cntlzw    ( cntlzw. )
//               eqv       ( eqv. )
//               extsb     ( extsb. )
//               extsh.    ( extsh. )
//               nand      ( nand. )
//               nor       ( nor. )
//               or        ( or. )
//               ori
//               oris
//               orc       ( orc. )
//               xor       ( xor. )
//               xori
//               xoris
//

/* and variants */
X(and)
{
#define and_code(rA, rS, rB)             \
    rA = X86_ANDW(rS, rB)

    and_code(REG0, REG1, REG2);
}
X(and.)
{
    and_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X(andc)
{
    UMODE tmp = ~REG2;
    and_code(REG0, REG1, tmp);
}
X(andc.)
{
    UMODE tmp = ~REG2;
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
}

X(andi.)
{
    UMODE tmp = (uint16_t)ARG2;
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
}

X(andis.)
{
    UMODE tmp = (((uint16_t)ARG2) << 16);
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
}

// Byte extend
X(extsb)
{
    REG0 = EXTS_B2N(REG1);
}
X(extsb.)
{
    REG0 = EXTS_B2N(REG1);
    UPDATE_CR0_V(REG0);
}

// Halfword extend
X(extsh)
{
    REG0 = EXTS_H2N(REG1);
}
X(extsh.)
{
    REG0 = EXTS_H2N(REG1);
    UPDATE_CR0_V(REG0);
}

// cntlzw:  Count leading zeroes (32-bit word).
X(cntlzw)
{
#define cntlzw_code(rA, rS)                     \
    uint64_t n = 32;                            \
    while(n < 64){                              \
        if(B_N(rS, n))  break;                  \
        n++;                                    \
    }                                           \
    rA = (n - 32)

    cntlzw_code(REG0, REG1);
}
X(cntlzw.)
{
    cntlzw_code(REG0, REG1);
    UPDATE_CR0_V(REG0);
}

X(eqv)
{
#define eqv_code(rD, rA, rB)                   \
    rD = ~(rA ^ rB)

    eqv_code(REG0, REG1, REG2);
}
X(eqv.)
{
    eqv_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}

X(nand)
{
#define nand_code(rA, rS, rB)                  \
    rA = ~(rS & rB)

    nand_code(REG0, REG1, REG2);
}
X(nand.)
{
    nand_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X(neg)
{
#define neg_code(rD, rA)                        \
    rD = X86_NEGW(rA);

    neg_code(REG0, REG1);
}
X(neg.)
{
    neg_code(REG0, REG1);
    UPDATE_CR0();
}
X(nego)
{
    neg_code(REG0, REG1);
    UPDATE_SO_OV();
}
X(nego.)
{
    neg_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

//
X(nor)
{
#define nor_code(rA, rS, rB)                    \
    rA = ~(rS | rB)

    nor_code(REG0, REG1, REG2);
}
X(nor.)
{
    nor_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}

X(or)
{
#define or_code(rA, rS, rB)                     \
    rA = X86_ORW(rS, rB)

    or_code(REG0, REG1, REG2);
}
X(or.)
{
    or_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X(orc)
{
#define orc_code(rA, rS, rB)                     \
    UMODE tmp = ~rB;                             \
    rA = X86_ORW(rS, tmp)

    orc_code(REG0, REG1, REG2);
}
X(orc.)
{
    orc_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X(ori)
{
    UMODE tmp = (uint16_t)(ARG2);
    or_code(REG0, REG1, tmp);
}

X(oris)
{
    UMODE tmp = (((uint16_t)ARG2) << 16);
    or_code(REG0, REG1, tmp);
}

// xor variants
X(xor)
{
#define xor_code(rA, rS, rB)      \
    rA = X86_XORW(rS, rB)

    xor_code(REG0, REG1, REG2);
}
X(xor.)
{
    xor_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X(xori)
{
    UMODE tmp = ((uint16_t)ARG2);
    xor_code(REG0, REG1, tmp);
}

X(xoris)
{
    UMODE tmp = (((uint16_t)ARG2) << 16);
    xor_code(REG0, REG1, tmp);
}

// START
// ------------------------------- BPU ---------------------------------------------
// BTB instrs
X(bbelr)
{
    // Not implemented
}
X(bblels)
{
    // Not implemented
}


// START
// --------------------------------- branch -----------------------------------------
// mnemonics :
//               b     ( ba, bl, bla )
//               bc    ( bca, bcl, bcla )
//               bclr  ( bclrl )
//               bcctr ( bcctrl )
//

// branch unconditional
// NOTE : LI is already decoded by the disassembler module into proper target addr ( ARG0 ),
//        hence "b" and "ba" are same. 
X(b)
{
#define b_code(tgtaddr)      \
    NIP = tgtaddr

    b_code(ARG0);
}
X(ba)
{
    b_code(ARG0);
}
X(bl)
{
    b_code(ARG0);
    LR = (PC + 4);
}
X(bla)
{
    b_code(ARG0);
    LR = (PC + 4);
}

// branch conditional
X(bc)
{
#define  BO3(BO)    ((BO >> 1) & 0x1)
#define  BO2(BO)    ((BO >> 2) & 0x1)
#define  BO1(BO)    ((BO >> 3) & 0x1)
#define  BO0(BO)    ((BO >> 4) & 0x1)

#define bc_code(BO, BI, tgtaddr)                                                     \
    if(!BO2(BO)) CTR = CTR - 1;                                                      \
    int ctr_ok  = BO2(BO) | ((((MSR_CM) ? CTR: (CTR & 0xffffffff)) != 0) ^ BO3(BO)); \
    int cond_ok = BO0(BO) | (get_crf(BI) == BO1(BO));                                \
    if(ctr_ok & cond_ok) NIP = tgtaddr;

    bc_code(ARG0, ARG1, ARG2);
}
X(bca)
{
    bc_code(ARG0, ARG1, ARG2)
}
X(bcl)
{
#define bcl_code(BO, BI, tgtaddr)                   \
    bc_code(BO, BI, tgtaddr);                       \
    LR = (PC + 4);

    bcl_code(ARG0, ARG1, ARG2);
}
X(bcla)
{
    bcl_code(ARG0, ARG1, ARG2);
}

// branch conditional to LR
X(bclr)
{
#define bclr_code(BO, BI, BH)                                                       \
    if(!BO2(BO)) CTR = CTR - 1;                                                     \
    int ctr_ok = BO2(BO) | ((((MSR_CM) ? CTR: (CTR & 0xffffffff)) != 0) ^ BO3(BO)); \
    int cond_ok = BO0(BO) | (get_crf(BI) == BO1(BO));                               \
    if(ctr_ok & cond_ok) NIP = LR & ~0x3;

    bclr_code(ARG0, ARG1, ARG2);
}
X(bclrl)
{
#define bclrl_code(BO, BI, BH)                       \
    bclr_code(BO, BI, BH);                           \
    LR = (PC + 4);

    bclrl_code(ARG0, ARG1, ARG2);
}

// branch conditional to CTR
X(bcctr)
{
#define bcctr_code(BO, BI, BH)                                                       \
    if(BO0(BO) | (get_crf(BI) == BO1(BO))) NIP = CTR & ~0x3;
    
    bcctr_code(ARG0, ARG1, ARG2);
}
X(bcctrl)
{
#define bcctrl_code(BO, BI, BH)                \
    bcctr_code(BO, BI, BH);                    \
    LR = (PC + 4);

    bcctrl_code(ARG0, ARG1, ARG2);
}


// START
// ------------------------------------- INTEGER COMPARE ------------------------------------
// mnemonics :
//               cmp
//               cmpi
//               cmpl
//               cmpli
//

// cmp crD, L, rA, rB 
X(cmp)
{
#define cmp_code(crD, L, rA, rB)                                       \
    int64_t a, b, c;                                                   \
    uint64_t crX;                                                      \
    if(L == 0){                                                        \
        a = EXTS_W2D(B_32_63(rA));                                     \
        b = EXTS_W2D(B_32_63(rB));                                     \
    }else{                                                             \
        a = rA;                                                        \
        b = rB;                                                        \
    }                                                                  \
    if(a < b)          { c = 0x4; }                                    \
    else if(a > b)     { c = 0x2; }                                    \
    else               { c = 0x1; }                                    \
    crX = (c << 1) | GET_SO();                                         \
    update_crF(crD, crX)

    cmp_code(ARG0, ARG1, REG2, REG3);
}
X(cmpi)
{
#define cmpi_code(crD, L, rA, SIMM)                                    \
    int64_t a, b, c;                                                   \
    uint64_t crX;                                                      \
    if(L == 0){                                                        \
        a = EXTS_W2D(B_32_63(rA));                                     \
    }else{                                                             \
        a = rA;                                                        \
    }                                                                  \
    b = EXTS_H2D(SIMM);                                                \
    if(a < b)          { c = 0x4; }                                    \
    else if(a > b)     { c = 0x2; }                                    \
    else               { c = 0x1; }                                    \
    crX = (c << 1) | GET_SO();                                         \
    update_crF(crD, crX)

    cmpi_code(ARG0, ARG1, REG2, ARG3);
}
X(cmpl)
{
#define cmpl_code(crD, L, rA, rB)                                      \
    uint64_t a, b, c;                                                  \
    uint64_t crX;                                                      \
    if(L == 0){                                                        \
        a = B_32_63(rA);                                               \
        b = B_32_63(rB);                                               \
    }else{                                                             \
        a = rA;                                                        \
        b = rB;                                                        \
    }                                                                  \
    if(a < b)          { c = 0x4; }                                    \
    else if(a > b)     { c = 0x2; }                                    \
    else               { c = 0x1; }                                    \
    crX = (c << 1) | GET_SO();                                         \
    update_crF(crD, crX)

    cmpl_code(ARG0, ARG1, REG2, REG3);
}
X(cmpli)
{
#define cmpli_code(crD, L, rA, UIMM)                                   \
    uint64_t a, b, c;                                                  \
    uint64_t crX;                                                      \
    if(L == 0){                                                        \
        a = B_32_63(rA);                                               \
    }else{                                                             \
        a = rA;                                                        \
    }                                                                  \
    b = UIMM;                                                          \
    if(a < b)          { c = 0x4; }                                    \
    else if(a > b)     { c = 0x2; }                                    \
    else               { c = 0x1; }                                    \
    crX = (c << 1) | GET_SO();                                         \
    update_crF(crD, crX)

    cmpli_code(ARG0, ARG1, REG2, ARG3);
}


// START
// ------------------------- CONDITION REGISTER LOGICAL -------------------------
// condition register instrs
// mnemonics :
//               crand
//               cror
//               crxor
//               crnand
//               crnor
//               creqv
//               crandc
//               crorc
//               mcrf
//

X(crand)
{
#define crand_code(crD, crA, crB)                       \
    update_crf(crD, get_crf(crA) & get_crf(crB));

    crand_code(ARG0, ARG1, ARG2);
}
X(crandc)
{
#define crandc_code(crD, crA, crB)                      \
    update_crf(crD, get_crf(crA) & ~(get_crf(crB)));

    crandc_code(ARG0, ARG1, ARG2);
}
X(creqv)
{
#define creqv_code(crD, crA, crB)                      \
    update_crf(crD, ~(get_crf(crA) ^ get_crf(crB)));

    creqv_code(ARG0, ARG1, ARG2);
}
X(crnand)
{
#define crnand_code(crD, crA, crB)                      \
    update_crf(crD, ~(get_crf(crA) & get_crf(crB)));

    crnand_code(ARG0, ARG1, ARG2);
}
X(crnor)
{
#define crnor_code(crD, crA, crB)                       \
    update_crf(crD, ~(get_crf(crA) | get_crf(crB)));

    crnor_code(ARG0, ARG1, ARG2);
}
X(cror)
{
#define cror_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) | get_crf(crB)));

    cror_code(ARG0, ARG1, ARG2);
}
X(crorc)
{
#define crorc_code(crD, crA, crB)                       \
    update_crf(crD, (get_crf(crA) | ~(get_crf(crB))));

    crorc_code(ARG0, ARG1, ARG2);
}
X(crxor)
{
#define crxor_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) ^ get_crf(crB)));

    crxor_code(ARG0, ARG1, ARG2);
}
X(mcrf)
{
#define mcrf_code(crfD, crfS)            \
    update_crF(crfD, get_crF(crfS));

    mcrf_code(ARG0, ARG1);
}


// START
// -------------------------- INTEGER SHIFT AND ROTATE -----------------------------
// mnemonics :
//               rlwinm  ( rlwinm. )
//               rlwnm   ( rlwnm. )
//               rlwimi  ( rlwimi. )
//               slw     ( slw. )
//               srw     ( srw. )
//               srawi   ( srawi. )
//               sraw    ( sraw. )
X(rlwimi)
{
#define rlwimi_code(rA, rS, SH, MB, ME)                                            \
    uint64_t n = (SH & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB + 32, ME + 32);                                           \
    rA = (r & m) | (rA & ~m)

    rlwimi_code(REG0, REG1, ARG2, ARG3, ARG4);
}
X(rlwimi.)
{
    rlwimi_code(REG0, REG1, ARG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
}
X(rlwinm)
{
#define rlwinm_code(rA, rS, SH, MB, ME)                                            \
    uint64_t n = (SH & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB + 32, ME + 32);                                           \
    rA = (r & m)

    rlwinm_code(REG0, REG1, ARG2, ARG3, ARG4);
}
X(rlwinm.)
{
    rlwinm_code(REG0, REG1, ARG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
}
X(rlwnm)
{
#define rlwnm_code(rA, rS, rB, MB, ME)                                             \
    uint64_t n = (rB & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB+32, ME+32);                                               \
    rA = (r & m)

    rlwnm_code(REG0, REG1, REG2, ARG3, ARG4);
}
X(rlwnm.)
{
    rlwnm_code(REG0, REG1, REG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
}

// Shift instrs
X(slw)
{
#define slw_code(rA, rS, rB)                                                       \
    uint64_t n = (rB & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m;                                                                    \
    uint8_t rB_58 = (rB >> 5) & 0x1;                                               \
    if(rB_58 == 0)  m = MASK(32, (63 - n));                                        \
    else            m = 0;                                                         \
    rA = (r & m)

    slw_code(REG0, REG1, REG2);
}
X(slw.)
{
    slw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
}
X(sraw)
{
#define sraw_code(rA, rS, rB)                                                            \
    uint64_t n = (rB & 0x1f);                                                            \
    uint64_t r = ROTL32(B_32_63(rS), (64 - n));                                          \
    uint64_t m;                                                                          \
    uint8_t rB_58 = (rB >> 5) & 0x1;                                                     \
    if(rB_58 == 0)  m = MASK(n+32, 63);                                                  \
    else            m = 0;                                                               \
    int s = ((rS >> 31) & 0x1);                                                          \
    rA = (r & m) | (((s) ? 0xffffffffffffffffL : 0L) & ~m);                              \
    update_xer_ca(s & (B_32_63(r & ~m) != 0))

    sraw_code(REG0, REG1, REG2);
}
X(sraw.)
{
    sraw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
}
X(srawi)
{
#define srawi_code(rA, rS, SH)                                                           \
    uint64_t n = (SH & 0x1f);                                                            \
    uint64_t r = ROTL32(B_32_63(rS), (64 - n));                                          \
    uint64_t m = MASK(n+32, 63);                                                         \
    int s = ((rS >> 31) & 0x1);                                                          \
    rA = (r& m) | (((s) ? 0xffffffffffffffffL : 0L) & ~m);                               \
    update_xer_ca(s & (B_32_63(r & ~m) != 0))

    srawi_code(REG0, REG1, ARG2);
}
X(srawi.)
{
    srawi_code(REG0, REG1, ARG2);
    UPDATE_CR0_V(UT(REG0));
}
X(srw)
{
#define srw_code(rA, rS, rB)                                                             \
    uint64_t n = (rB & 0x1f);                                                            \
    uint64_t r = ROTL32(B_32_63(rS), (64 - n));                                          \
    uint64_t m;                                                                          \
    uint8_t rB_58 = ((rB >> 5) & 0x1);                                                   \
    if ( rB_58 == 0)   m = MASK(n+32, 63);                                               \
    else               m = 0;                                                            \
    rA = (r & m)

    srw_code(REG0, REG1, REG2);
}
X(srw.)
{
    srw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
}

// ------------------------------ load/store ---------------------------------------
// mnemonics : 
//              lbz          stb
//              lbzx         stbx
//              lbzu         stbu
//              lbzux        stbux
//              lhz          sth
//              lhzx         sthx
//              lhzu         sthu
//              lhzux        sthux
//              lha          stw
//              lhax         stwx
//              lhau         stwu
//              lhaux        stwux
//              lwz
//              lwzx
//              lwzu
//              lwzux
//
//              lhbrx        sthbrx
//              lwbrx        stwbrx
//
//              lmw          stmw
//
//              

// byte loads
X(lbz)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD8(ea);    
}
X(lbzx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD8(ea);
}
X(lbzu)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD8(ea);
    REG2 = ea;
}
X(lbzux)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD8(ea);
    REG1 = ea;
}

// Halfword algebraic loads
X(lha)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = EXTS_H2N(LOAD16(ea));
}
X(lhax)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = EXTS_H2N(LOAD16(ea));
}
X(lhau)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = EXTS_H2N(LOAD16(ea));
    REG2 = ea;
}
X(lhaux)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = EXTS_H2N(LOAD16(ea));
    REG1 = ea;
}

// Half word loads
X(lhz)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD16(ea);
}
X(lhzx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD16(ea);
}
X(lhzu)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD16(ea);
    REG2 = ea;
}
X(lhzux)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD16(ea);
    REG1 = ea;
}

// word loads
// lwz rD,D(rA)
X(lwz)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD32(ea);
}
// lwzx rD,rA,rB
X(lwzx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD32(ea);
}
//  lwzu rD,D(rA)
X(lwzu)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD32(ea);
    REG2 = ea;
}
X(lwzux)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD32(ea);
    REG1 = ea;
}

// Byte reversed indexed loads
X(lhbrx)
{
    UMODE tmp = 0;
    UMODE ea;
    uint16_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = LOAD16(ea);
    REG0 = SWAPB16(data);
}
X(lwbrx)
{
    UMODE tmp = 0;
    UMODE ea;
    uint32_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = LOAD32(ea);
    REG0 = SWAPB32(data);
}

// load multiple words
X(lmw)
{
    UMODE tmp = 0;
    UMODE ea;
    int r;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    r = ARG0;
    while(r <= 31){
        GPR(r) = LOAD32(ea);
        r++;
        ea += 4;
    }
}

// byte stores
X(stb)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE8(ea, REG0);
}
X(stbx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE8(ea, REG0);
}
X(stbu)
{
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE8(ea, REG0);
    REG2 = ea;
}
X(stbux)
{
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE8(ea, REG0);
    REG1 = ea;
}

// half word stores
X(sth)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE16(ea, REG0);
}
X(sthx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE16(ea, REG0);
}
X(sthu)
{
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE16(ea, REG0);
    REG2 = ea;
}
X(sthux)
{
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE16(ea, REG0);
    REG1 = ea;
}

// word stores
X(stw)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE32(ea, REG0);
}
X(stwx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE32(ea, REG0);
}
X(stwu)
{
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE32(ea, REG0);
    REG2 = ea;
}
X(stwux)
{
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE32(ea, REG0);
    REG1 = ea;
}

// byte reversed stores
X(sthbrx)
{
    UMODE tmp = 0;
    UMODE ea;
    uint16_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = REG0;
    STORE16(ea, SWAPB16(data));
}
X(stwbrx)
{
    UMODE tmp = 0;
    UMODE ea;
    uint32_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = REG0;
    STORE32(ea, SWAPB32(data));
}

// multiple word store
X(stmw)
{
    UMODE tmp = 0;
    UMODE ea;
    int r;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    r = ARG0;
    while(r <= 31){
        STORE32(ea, GPR(r));
        r++;
        ea += 4;
    }
}



// START
// ------------------------------ TLB ----------------------------------------------
// mnemonics :
//              tlbivax
//              tlbre
//              tlbsx
//              tlbsync
//              tlbwe

X(tlbwe)
{
    TLBWE();
}

X(tlbre)
{
    TLBRE();
}

X(tlbsx)
{
    uint64_t ea = REG1;
    if(ARG0){ ea += REG0; }
    TLBSX(ea);
}

X(tlbivax)
{
    uint64_t ea = REG1;
    if(ARG0){ ea += REG0; }
    TLBIVAX(ea);
}

X(tlbsync)
{
    //dummy
}

// START
// ------------------------------ CACHE -------------------------------------------
// mnemonics :
//               dcba
//               dcbf
//               dcbi
//               dcblc
//               dcbz
//               dcbst
//               dcbt
//               dcbtst
//               dcbtstls
//               icbi
//               icblc
//               icbt
//               icbtls
//
// TODO  : Right now, no cache is implemented, hence cache instructions are all dummy.

X(dcba)
{
    // dummy
}

X(dcbf)
{
    // dummy
}

X(dcbi)
{
    // dummy
}

X(dcblc)
{
    // dummy
}

X(dcbz)
{
    // dummy
}

X(dcbst)
{
    // dummy
}

X(dcbt)
{
    // dummy
}

X(dcbtst)
{
    // dummy
}

X(dcbtstls)
{
    // dummy
}

X(icbi)
{
    // dummy
}

X(icblc)
{
    // dummy
}

X(icbt)
{
    // dummy
}

X(icbtls)
{
    // dummy
}

// START
// ------------------------------ SYSTEM LINKAGE -----------------------------------
// menmonics :
//             rfi
//             rfmci
//             rfci
//             sc
//             mfmsr
//             mtmsr
//             wrtee
//             wrteei

X(rfi)
{
    MSR = SRR1;
    NIP = ((UMODE)SRR0) & ~0x3ULL;       // Mask Lower 2 bits to zero
}

X(rfmci)
{
    MSR = MCSRR1;
    NIP = ((UMODE)MCSRR0) & ~0x3ULL;
}

X(rfci)
{
    MSR = CSRR1;
    NIP = ((UMODE)CSRR0) & ~0x3ULL;
}

X(sc)
{
    throw PPC_EXCEPT(PPC_EXCEPT_SC, "system call");     // raise a system call exception
}

X(mfmsr)
{
    uint64_t newmsr = B_32_63(REG0);
    uint8_t newmsr_cm = ((newmsr & MSR_CM) ? 1:0);
    if((MSR_CM == 0) && (newmsr_cm == 1)) { NIP &= 0xffffffff; }

    // Another check is required for MSR_GS == 1, but since we don't have guest mode
    // this is irrelevant.
    MSR = newmsr;
}

X(mtmsr)
{
    REG0 = B_32_63(MSR);
}

X(wrtee)
{
#define wrtee_code(rS)            \
    MSR &= ~(1L << 15);           \
    MSR |= rS & (1L << 15);

    wrtee_code(REG0);
}
X(wrteei)
{
#define wrteei_code(E)            \
    MSR &= ~(1 << 15);            \
    MSR |= ((E & 0x1) << 15);

    wrteei_code(ARG0);
}

// START
// ------------------------------ TRAP ----------------------------------------------
// mnemonics :
//             twi
//             tw

X(twi)
{
#define twi_code(TO, rA, SI)                                                           \
    int64_t a = EXTS_W2D(B_32_63(rA));                                                 \
    int64_t b = EXTS_H2D(SI);                                                          \
    uint64_t au = rA;                                                                  \
    uint64_t bu = SI;                                                                  \
    bool trap = false;                                                                 \
    if((a < b)   && ((TO >> 0) & 0x1)) { trap = true; }                                \
    if((a > b)   && ((TO >> 1) & 0x1)) { trap = true; }                                \
    if((a == b)  && ((TO >> 2) & 0x1)) { trap = true; }                                \
    if((au < bu) && ((TO >> 3) & 0x1)) { trap = true; }                                \
    if((au > bu) && ((TO >> 4) & 0x1)) { trap = true; }                                \
    if(trap == true){                                                                  \
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_TRAP, "System Trap");       \
    }

    twi_code(ARG0, REG1, ARG2);
}

X(tw)
{
#define tw_code(TO, rA, rB)                                                            \
    int64_t a = EXTS_W2D(B_32_63(rA));                                                 \
    int64_t b = EXTS_W2D(B_32_63(rB));                                                 \
    uint64_t au = rA;                                                                  \
    uint64_t bu = rB;                                                                  \
    bool trap = false;                                                                 \
    if((a < b)   && ((TO >> 0) & 0x1)) { trap = true; }                                \
    if((a > b)   && ((TO >> 1) & 0x1)) { trap = true; }                                \
    if((a == b)  && ((TO >> 2) & 0x1)) { trap = true; }                                \
    if((au < bu) && ((TO >> 3) & 0x1)) { trap = true; }                                \
    if((au > bu) && ((TO >> 4) & 0x1)) { trap = true; }                                \
    if(trap == true){                                                                  \
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_TRAP, "System Trap");       \
    }

    tw_code(ARG0, REG1, REG2);
}

// START
// ------------------------------ PROCESSOR CONTROL --------------------------------
// mnemonics :
//              mtcrf
//              mcrxr
//              mfcr
//              mtspr
//              mfspr

X(mtcrf)
{
#define mtcrf_code(CRM, rS)                         \
    uint64_t mask = 0, i;                           \
    uint8_t tmp = CRM;                              \
    for(i=0; i<8; i++){                             \
        mask |= (tmp & 0x80)?(0xf << (7 - i)*4):0;  \
        tmp <<= 1;                                  \
    }                                               \
    CR = ((B_32_63(rS) & mask) | (CR & ~mask))

    mtcrf_code(ARG0, REG1);
}

X(mcrxr)
{
#define mcrxr_code(crfD)                 \
    update_crF(crfD, get_xerF(0));       \
    /* clear XER[32:35] */               \
    XER &= 0xfffffff

    mcrxr_code(ARG0);
}

X(mfcr)
{
#define mfcr_code(rD)                               \
    rD = B_32_63(CR)

    mfcr_code(ARG0);
}

X(mfspr)
{
#define mfspr_code(rD, SPRN)                   \
    rD = SPR(SPRN);

    mfspr_code(REG0, ARG1);
}

X(mtspr)
{
#define mtspr_code(SPRN, rS)                  \
    SPR(SPRN) = rS;

    mtspr_code(ARG0, REG1);       // FIXME : No special checks for SPRN no. Will fix this later on.
}

// START
// ------------------------------ MEMORY SYNCHRONIZATION ---------------------------
// mnemonics :
//             isync
//             lwarx
//             mbar
//             msync
//             stwcx.

X(isync)
{
    // Do nothing
}

// Barrier
X(mbar)
{
    //Do nothing
}

X(msync)
{
    // Do nothing
}

// Reservation load
X(lwarx)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    SET_RESV(ea, 4);
    REG0 = LOAD32(ea);
}

// Reservation store
X(stwcx.)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    if(CHECK_RESV(ea, 4)){
        STORE32(ea, REG0);
        update_crF(0, 0x20 | get_xer_so());  // Set ZERO bit
    }else{
        update_crF(0, get_xer_so());
    }
    CLEAR_RESV(ea);
}

// START
// ------------------------------ PERFORMANCE MONITORING ---------------------------
// mnemonics :
//             mfpmr
//             mtpmr

X(mtpmr)
{
#define mtpmr_code(PMRN, rS)                  \
    PMR(PMRN) = rS;

    mtpmr_code(ARG0, REG1);
}

X(mfpmr)
{
#define mfpmr_code(rD, PMRN)                  \
    rD = PMR(PMRN);

    mfpmr_code(ARG0, REG1);
}

// START
// ------------------------------ INTEGER SELECT -----------------------------------
// mnemonics :
//              isel

X(isel)
{
    uint64_t a = 0;
    if(ARG1) { a = REG1; }
    if(B_N(CR, (32 + ARG3)) == 0){ REG0 = a;    }
    else                         { REG0 = REG2; }
}


/////////////////////////////////////////////////////////////////////////////////////

// ------------------------------ SPE -----------------------------------------------

typedef struct BITREV {
    inline uint64_t static BITREV_FN(uint64_t x){
        uint64_t result = 0;
        uint64_t mask = 1;
        int shift = 31;
        int cnt = 32;
        uint64_t t;

        while(cnt > 0){
           t = x & mask;
           if(shift >= 0){ result |= t << shift; }
           else{ result |= (t >> -shift); }
           cnt --;
           shift -= 2;
           mask <<= 1;
        }
        return result;
    }
} BITREV;

// Pack/unpack macros
// Pack 2 words into a double word (u & v should be 32 bit) : u = higher word, v = lower word
#define PACK_2W(u, v)                                            ((U64((u)) << 32) | U64((v)))

// SPE macros
#define SATURATE(ov, carry, sat_ovn, sat_ov, val)                ((ov) ? ((carry) ? (sat_ovn) : (sat_ov)) : val)
#define SL(value, cnt)                                           (((cnt) > 31) ? 0 : ((value) << (cnt)))
#define BITREVERSE(x)                                            BITREV::BITREV_FN(x)

// NOTE : ABS_32(0x8000_0000) = 0x8000_0000
#define ABS_32(x)                                                ((((int32_t)(x)) > 0) ? (x) : -((int32_t)(x)))
#define ABS_64(x)                                                ((((int64_t)(x)) > 0) ? (x) : -((int64_t)(x)))

// Sign extension macros
// 5 BI (bits) to word
#define EXTS_5BI2W(x)                                            ((((x) >> 4) & 0x1) ? (((x) & 0xfffff) | ~0xfffffUL) : ((x) & 0xfffff))

// FIXME : Verify the correctness of these macros/functions
typedef struct SFF {
    inline uint64_t static SF(uint16_t a, uint16_t b){
        uint64_t p = EXTS_H2D(a) * EXTS_H2D(b);
        p = EXTS_W2D(B_32_63(p));
        p = (p << 1) & 0xfffffffe;
        return p;
    }
    inline uint64_t static GSF(uint16_t a, uint16_t b){
        uint64_t p = EXTS_H2D(a) * EXTS_H2D(b);
        p = EXTS_W2D(B_32_63(p));
        p = (p << 1) & 0xfffffffffffffffeULL;
        return p;
    }
} SF_F;

// Guarded fraction multiplications
#define SF(a, b)                                                 SFF::SF(U16(a), U16(b))
#define GSF(a, b)                                                SFF::GSF(U16(a), U16(b))

#define UPDATE_SPEFSCR_OV(ovh, ovl)                                          \
    do {                                                                     \
        SPEFSCR &= ~(SPEFSCR_OVH | SPEFSCR_OV);                              \
        SPEFSCR |= ((ovh) ? SPEFSCR_OVH:0) | ((ovl) ? SPEFSCR_OV:0) |        \
                   ((ovh) ? SPEFSCR_SOVH:0) | ((ovl) ? SPEFSCR_SOV:0);       \
    }while(0)

// ----------------------------------------------------------------------------------

X(brinc)
{
    // FIXME : Verify value of n for e500v2
    int n         = 32;  // Implementation dependant
    uint64_t mask = (MASK(64-n, 63) & REG2);
    uint64_t a    = (MASK(64-n, 63) & REG1);

    uint64_t d    = BITREVERSE(1 + BITREVERSE(a | ~mask));
    REG0          = (MASK(0, 63-n) & REG1) | (d & mask);
}

X(evabs)
{
    uint32_t u, v;
    u    = ABS_32(B_0_31(REG1));
    v    = ABS_32(B_32_63(REG1));
    REG0 = PACK_2W(u, v);
}

X(evaddiw)
{
    uint32_t u, v;
    u    = B_32_63(B_0_31(REG1) + (ARG2 & 0xfffff));
    v    = B_32_63(B_32_63(REG1) +(ARG2 & 0xfffff));
    // ARG2 is zero extended and added to each of lower & upper halves of REG1
    REG0 = PACK_2W(u, v);
}

X(evaddsmiaaw)
{
    uint32_t u, v;
    u    = B_32_63(B_0_31(ACC) + B_0_31(REG1));
    v    = B_32_63(B_32_63(ACC) + B_32_63(REG1));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evaddssiaaw)
{
    int64_t tmp;
    int ovh, ovl, tmp31;
    uint64_t h=0, l=0;

    tmp   = EXTS_W2D(B_0_31(ACC)) + EXTS_W2D(B_0_31(REG1));
    ovh   = B_N(tmp, 31) ^ B_N(tmp, 32);
    tmp31 = B_N(tmp, 31);
    h     = SATURATE(ovh, tmp31, 0x80000000UL, 0x7fffffffUL, B_32_63(tmp));

    tmp   = EXTS_W2D(B_32_63(ACC)) + EXTS_W2D(B_32_63(REG1));
    ovl   = B_N(tmp, 31) ^ B_N(tmp, 32);
    tmp31 = B_N(tmp, 31);
    l     = SATURATE(ovl, tmp31, 0x80000000UL, 0x7fffffffUL, B_32_63(tmp));

    REG0  = (h << 32) |  l;
    ACC   = REG0;

    // Update SPEFSCR
    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evaddusiaaw)
{
    uint64_t tmp;
    int ovh, ovl, tmp31;
    uint64_t h=0, l=0;

    tmp   = B_0_31(ACC) + B_0_31(REG1);
    ovh   = B_N(tmp, 31);
    tmp31 = ovh;
    h     = SATURATE(ovh, tmp31, 0xffffffffUL, 0xffffffffUL, B_32_63(tmp));

    tmp   = B_32_63(ACC) + B_32_63(REG1);
    ovl   = B_N(tmp, 31);
    tmp31 = ovl;
    l     = SATURATE(ovl, tmp31, 0xffffffffUL, 0xffffffffUL, B_32_63(tmp));

    REG0  = (h << 32) | l;
    ACC   = REG0;

    // Update SPEFSCR
    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evaddumiaaw)
{
    uint32_t u, v;
    u     = B_32_63(B_0_31(ACC) + B_0_31(REG1));
    v     = B_32_63(B_32_63(ACC) + B_32_63(REG1));
    REG0  = PACK_2W(u, v);
    ACC   = REG0;
}

X(evaddw)
{
    uint32_t u, v;
    u     = B_32_63(B_0_31(REG1) + B_0_31(REG2));
    v     = B_32_63(B_32_63(REG1) + B_32_63(REG2));
    REG0  = PACK_2W(u, v);
}

X(evand)
{
    REG0  = REG1 & REG2;
}

X(evandc)
{
    REG0  = REG1 & ~REG2;
}

X(evcmpeq)
{
    uint32_t ah, al, bh, bl;
    int ch=0, cl=0;

    ah  = B_0_31(REG1);
    al  = B_32_63(REG1);
    bh  = B_0_31(REG2);
    bl  = B_32_63(REG2);

    if(ah == bh){ ch = 1; }
    if(al == bl){ cl = 1; }

    // Update CR[BF] where BF=ARG0
    update_crF(ARG0, ((ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl)));
}

X(evcmpgts)
{
    int32_t ah, al, bh, bl;
    int ch=0, cl=0;

    ah  = B_0_31(REG1);
    al  = B_32_63(REG1);
    bh  = B_0_31(REG2);
    bl  = B_32_63(REG2);

    if(ah > bh){ ch = 1; }
    if(al > bl){ cl = 1; }

    // Update CR[BF] where BF=ARG0
    update_crF(ARG0, ((ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl)));
}

X(evcmpgtu)
{
    uint32_t ah, al, bh, bl;
    int ch=0, cl=0;

    ah  = B_0_31(REG1);
    al  = B_32_63(REG1);
    bh  = B_0_31(REG2);
    bl  = B_32_63(REG2);

    if(ah > bh){ ch = 1; }
    if(al > bl){ cl = 1; }

    // Update CR[BF] where BF=ARG0
    update_crF(ARG0, ((ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl)));
}

X(evcmplts)
{
    int32_t ah, al, bh, bl;
    int ch=0, cl=0;

    ah  = B_0_31(REG1);
    al  = B_32_63(REG1);
    bh  = B_0_31(REG2);
    bl  = B_32_63(REG2);

    if(ah < bh){ ch = 1; }
    if(al < bl){ cl = 1; }

    // Update CR[BF] where BF=ARG0
    update_crF(ARG0, ((ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl)));
}

X(evcmpltu)
{
    uint32_t ah, al, bh, bl;
    int ch=0, cl=0;

    ah  = B_0_31(REG1);
    al  = B_32_63(REG1);
    bh  = B_0_31(REG2);
    bl  = B_32_63(REG2);

    if(ah < bh){ ch = 1; }
    if(al < bl){ cl = 1; }

    // Update CR[BF] where BF=ARG0
    update_crF(ARG0, ((ch << 3) | (cl << 2) | ((ch | cl) << 1) | (ch & cl)));
}

X(evcntlsw)
{
    int n = 0;
    uint64_t s = B_N(REG1, n);
    uint64_t h, l;

    while(n < 32){
        if(B_N(REG1, n) != s){ break; }
        n++;
    }
    h = n;

    n = 0;
    s = B_N(REG1, n+32);

    while(n < 32){
        if(B_N(REG1, n+32) != s){ break; }
        n++;
    }
    l =  n;

    REG0 = (h << 32) | l;
}

X(evcntlzw)
{
    int n = 0;
    uint64_t h, l;

    while(n < 32){
        if(B_N(REG1, n) == 1){ break; }
        n++;
    }
    h = n;

    n = 0;
    while(n < 32){
        if(B_N(REG1, n+32) == 1){ break; }
        n++;
    }
    l =  n;

    REG0 = (h << 32) | l;
}

X(evdivws)
{
    // TODO : Implement this
    throw sim_except_fatal("Not implemented !!");
}

X(evdivwu)
{
    // TODO : Implement this
    throw sim_except_fatal("Not implemented !!");
}

X(eveqv)
{
    REG0 = REG1 ^ REG2;  // xor to compare
}

X(evextsb)
{
    uint32_t u, v;
    u    = B_32_63(EXTS_B2D(B_24_31(REG1)));
    v    = B_32_63(EXTS_B2D(B_56_63(REG1)));
    REG0 = PACK_2W(u, v);
}

X(evextsh)
{
    uint32_t u, v;
    u    = B_32_63(EXTS_H2D(B_16_31(REG1)));
    v    = B_32_63(EXTS_H2D(B_48_63(REG1)));
    REG0 = PACK_2W(u, v);
}

X(evldd)
{
    UMODE b = 0;
    UMODE ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = LOAD64(ea);
}

X(evlddx)
{
    UMODE b = 0;
    UMODE ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = LOAD64(ea);
}

X(evldh)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea + 2)) << 32) | (U64(LOAD16(ea + 4)) << 16) | U64(LOAD16(ea + 6));
}

X(evldhx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea + 2)) << 32) | (U64(LOAD16(ea + 4)) << 16) | U64(LOAD16(ea + 6));
}

X(evldw)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD32(ea)) << 32) | U64(LOAD32(ea + 4));
}

X(evldwx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    REG0 = (U64(LOAD32(ea)) << 32) | U64(LOAD32(ea + 4));
}

X(evlhhesplat)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t v = LOAD16(ea);
    REG0 = ((U64(v) << 48) | (U64(v) << 16)) & 0xffff0000ffff0000ULL;
}

X(evlhhesplatx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    uint16_t v = LOAD16(ea);
    REG0 = ((U64(v) << 48) | (U64(v) << 16)) & 0xffff0000ffff0000ULL;
}

X(evlhhossplat)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    u    = B_32_63(EXTS_H2W(LOAD16(ea)));
    REG0 = PACK_2W(u, u);
}

X(evlhhossplatx)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    u    = B_32_63(EXTS_H2W(LOAD16(ea)));
    REG0 = PACK_2W(u, u);
}

X(evlhhousplat)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t v = LOAD16(ea);
    REG0 = (U64(v) << 32) | U64(v);
}

X(evlhhousplatx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    uint16_t v = LOAD16(ea);
    REG0 = (U64(v) << 32) | U64(v);
}

X(evlwhe)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea+2)) << 16);
}

X(evlwhex)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea+2)) << 16);
}

X(evlwhos)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;
    
    u    = B_32_63(EXTS_H2D(LOAD16(ea)));
    v    = B_32_63(EXTS_H2D(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X(evlwhosx)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    u    = B_32_63(EXTS_H2D(LOAD16(ea)));
    v    = B_32_63(EXTS_H2D(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X(evlwhou)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    u    = B_32_63(U64(LOAD16(ea)));
    v    = B_32_63(U64(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X(evlwhoux)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    u    = B_32_63(U64(LOAD16(ea)));
    v    = B_32_63(U64(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X(evlwhsplat)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t u = LOAD16(ea);
    uint16_t v = LOAD16(ea+2);

    REG0 = (U64(u) << 48) | (U64(u) << 32) | (U64(v) << 16) | U64(v);
}

X(evlwhsplatx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    uint16_t u = LOAD16(ea);
    uint16_t v = LOAD16(ea+2);

    REG0 = (U64(u) << 48) | (U64(u) << 32) | (U64(v) << 16) | U64(v);
}

X(evlwwsplat)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint32_t u = LOAD32(ea);

    REG0 = (U64(u) << 32) | U64(u);
}

X(evlwwsplatx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    uint32_t u = LOAD32(ea);

    REG0 = (U64(u) << 32) | U64(u);
}

X(evmergehi)
{
    uint32_t u, v;
    u    = B_0_31(REG1);
    v    = B_0_31(REG2);
    REG0 = PACK_2W(u, v);
}

X(evmergelo)
{
    uint32_t u, v;
    u    = B_32_63(REG1);
    v    = B_32_63(REG2);
    REG0 = PACK_2W(u, v);
}

X(evmergehilo)
{
    uint32_t u, v;
    u    = B_0_31(REG1);
    v    = B_32_63(REG2);
    REG0 = PACK_2W(u, v);
}

X(evmergelohi)
{
    uint32_t u, v;
    u    = B_32_63(REG1);
    v    = B_0_31(REG2);
    REG0 = PACK_2W(u, v);
}

X(evmhegsmfaa)
{
    uint64_t tmp = GSF(B_32_47(REG1), B_32_47(REG2));
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X(evmhegsmfan)
{
    uint64_t tmp = GSF(B_32_47(REG1), B_32_47(REG2));
    REG0 = ACC - tmp;
    ACC  = REG0;
}

X(evmhegsmiaa)
{
    uint64_t tmp = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));
    tmp = EXTS_W2D(tmp);
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X(evmhegsmian)
{
    uint64_t tmp = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));
    tmp = EXTS_W2D(tmp);
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X(evmhegumiaa)
{
    uint64_t tmp = B_32_63(B_32_47(REG1) * B_32_47(REG2));
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X(evmhegumian)
{
    uint64_t tmp = B_32_63(B_32_47(REG1) * B_32_47(REG2));
    REG0 = ACC - tmp;
    ACC  = REG0;
}

X(evmhesmf)
{
    uint32_t u, v;
    u    = B_32_63(SF(B_0_15(REG1), B_0_15(REG2)));
    v    = B_32_63(SF(B_32_47(REG1), B_32_47(REG2)));
    REG0 = PACK_2W(u, v);
}

X(evmhesmfa)
{
    uint32_t u, v;
    u    = B_32_63(SF(B_0_15(REG1), B_0_15(REG2)));
    v    = B_32_63(SF(B_32_47(REG1), B_32_47(REG2)));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evmhesmfaaw)
{
    uint32_t u = B_32_63(B_0_31(ACC)  + SF(B_0_15(REG1), B_0_15(REG2)));
    uint32_t v = B_32_63(B_32_63(ACC) + SF(B_32_47(REG1), B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhesmfanw)
{
    uint32_t u = B_32_63(B_0_31(ACC)  - SF(B_0_15(REG1), B_0_15(REG2)));
    uint32_t v = B_32_63(B_32_63(ACC) - SF(B_32_47(REG1), B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhesmi)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
}

X(evmhesmia)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhesmiaaw)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    u = B_0_31(ACC) + u;
    v = B_32_63(ACC) + v;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhesmianw)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    u = B_0_31(ACC) - u;
    v = B_32_63(ACC) - v;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhessf)
{
    uint32_t tmp;
    uint32_t u, v;
    bool movh, movl;

    tmp = SF(B_0_15(REG1), B_0_15(REG2));
    if((B_0_15(REG1) == 0x8000) && (B_0_15(REG2) == 0x8000))  { u = 0x7fffffff; movh = 1; }
    else                                                      { u = tmp; movh = 0;        }

    tmp = SF(B_32_47(REG1), B_32_47(REG2));
    if((B_32_47(REG1) == 0x8000) && (B_32_47(REG2) == 0x8000))  { v = 0x7fffffff; movl = 1; }
    else                                                        { v = tmp; movl = 0;        }

    REG0 = (U64(u) << 32) | U64(v);

    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmhessfa)
{
    uint32_t tmp;
    uint32_t u, v;
    bool movh, movl;

    tmp = SF(B_0_15(REG1), B_0_15(REG2));
    if((B_0_15(REG1) == 0x8000) && (B_0_15(REG2) == 0x8000))  { u = 0x7fffffff; movh = 1; }
    else                                                      { u = tmp; movh = 0;        }

    tmp = SF(B_32_47(REG1), B_32_47(REG2));
    if((B_32_47(REG1) == 0x8000) && (B_32_47(REG2) == 0x8000))  { v = 0x7fffffff; movl = 1; }
    else                                                        { v = tmp; movl = 0;        }

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmhessfaaw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool movh, movl, ovh, ovl;

    tmp = SF(B_0_15(REG1), B_0_15(REG2));
    if((B_0_15(REG1) == 0x8000) && (B_0_15(REG2) == 0x8000))  { tmp = 0x7fffffff; movh = 1; }
    else                                                      { movh = 0;                   }

    tmp64 = EXTS_W2D(B_0_31(ACC)) + EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);

    u = SATURATE(ovh, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    tmp = SF(B_32_47(REG1), B_32_47(REG2));
    if((B_32_47(REG1) == 0x8000) && (B_32_47(REG2) == 0x8000))  { tmp = 0x7fffffff; movl = 1; }
    else                                                        { movl = 0;                   }

    tmp64 = EXTS_W2D(B_32_63(ACC)) + EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);

    v = SATURATE(ovl, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmhessfanw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool movh, movl, ovh, ovl;

    tmp = SF(B_0_15(REG1), B_0_15(REG2));
    if((B_0_15(REG1) == 0x8000) && (B_0_15(REG2) == 0x8000))  { tmp = 0x7fffffff; movh = 1; }
    else                                                      { movh = 0;                   }

    tmp64 = EXTS_W2D(B_0_31(ACC)) - EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);

    u = SATURATE(ovh, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    tmp = SF(B_32_47(REG1), B_32_47(REG2));
    if((B_32_47(REG1) == 0x8000) && (B_32_47(REG2) == 0x8000))  { tmp = 0x7fffffff; movl = 1; }
    else                                                        { movl = 0;                   }

    tmp64 = EXTS_W2D(B_32_63(ACC)) - EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);

    v = SATURATE(ovl, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmhessiaaw)
{
    uint32_t tmp;
    uint64_t tmp64;
    uint32_t u, v;
    bool ovh, ovl;

    tmp   = X86_MULW_L(B_0_15(REG1), B_0_15(REG2));
    tmp64 = EXTS_W2D(B_0_31(ACC)) + EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    tmp   = X86_MULW_L(B_32_47(REG1), B_32_47(REG1));
    tmp64 = EXTS_W2D(B_32_63(ACC)) + EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmhessianw)
{
    uint32_t tmp;
    uint64_t tmp64;
    uint32_t u, v;
    bool ovh, ovl;

    tmp   = X86_MULW_L(B_0_15(REG1), B_0_15(REG2));
    tmp64 = EXTS_W2D(B_0_31(ACC)) - EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    tmp   = X86_MULW_L(B_32_47(REG1), B_32_47(REG1));
    tmp64 = EXTS_W2D(B_32_63(ACC)) - EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000UL, 0x7fffffffUL, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmheumi)
{
    uint32_t u, v;

    u  = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    v  = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));

    REG0 = (U64(u) << 32) | U64(v);
}

X(evmheumia)
{
    uint32_t u, v;

    u  = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    v  = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmheumiaaw)
{
    uint32_t tmp;
    uint32_t u, v;

    tmp = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmheumianw)
{
    uint32_t tmp;
    uint32_t u, v;

    tmp = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmheusiaaw)
{
    uint32_t tmp;
    uint64_t tmp64;
    bool     ovh, ovl;
    uint32_t u, v;

    tmp   = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    tmp64 = EXTZ_W2D(B_0_31(ACC)) + EXTZ_W2D(tmp);
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, 0, 0xffffffffUL, 0xffffffffUL, B_32_63(tmp64));

    tmp   = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));
    tmp64 = EXTZ_W2D(B_32_63(ACC)) + EXTZ_W2D(tmp);
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, 0, 0xffffffffUL, 0xffffffffUL, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmheusianw)
{
    uint32_t tmp;
    uint64_t tmp64;
    bool     ovh, ovl;
    uint32_t u, v;

    tmp   = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    tmp64 = EXTZ_W2D(B_0_31(ACC)) - EXTZ_W2D(tmp);
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, 0, 0x0UL, 0x0UL, B_32_63(tmp64));

    tmp   = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));
    tmp64 = EXTZ_W2D(B_32_63(ACC)) - EXTZ_W2D(tmp);
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, 0, 0x0UL, 0x0UL, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}


X(evmhogsmfaa)
{
    uint64_t tmp64;
    tmp64 = GSF(B_48_63(REG1), B_48_63(REG2));
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X(evmhogsmfan)
{
    uint64_t tmp64;
    tmp64 = GSF(B_48_63(REG1), B_48_63(REG2));
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X(evmhogsmiaa)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(tmp);
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X(evmhogsmian)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(tmp);
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X(evmhogumiaa)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(tmp);
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X(evmhogumian)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(tmp);
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X(evmhosmf)
{
    uint32_t u, v;
    u    = SF(B_16_31(REG1), B_16_31(REG2));
    v    = SF(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
}

X(evmhosmfa)
{
    uint32_t u, v;
    u    = SF(B_16_31(REG1), B_16_31(REG2));
    v    = SF(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhosmfaaw)
{
    uint32_t tmp, u, v;

    tmp = SF(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhosmfanw)
{
    uint32_t tmp, u, v;

    tmp = SF(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhosmi)
{
    uint32_t u, v;
    u    = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
}

X(evmhosmia)
{
    uint32_t u, v;
    u    = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhosmiaaw)
{
    uint32_t tmp, u, v;

    tmp = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhosmianw)
{
    uint32_t tmp, u, v;

    tmp = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhossf)
{
    uint32_t tmp, u, v;
    bool movh, movl;

    tmp = SF(B_16_31(REG1), B_16_31(REG1));
    if((B_16_31(REG1) == 0x8000) && (B_16_31(REG2) == 0x8000))  { u = 0x7fffffff; movh = 1; }
    else                                                        { u = tmp; movh = 0;        }

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    if((B_48_63(REG1) == 0x8000) && (B_48_63(REG2) == 0x8000))  { v = 0x7fffffff; movl = 1; }
    else                                                        { v = tmp; movl = 0;        }

    REG0 = (U64(u) << 32) | U64(v);
    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmhossfa)
{
    uint32_t tmp, u, v;
    bool movh, movl;

    tmp = SF(B_16_31(REG1), B_16_31(REG1));
    if((B_16_31(REG1) == 0x8000) && (B_16_31(REG2) == 0x8000))  { u = 0x7fffffff; movh = 1; }
    else                                                        { u = tmp; movh = 0;        }

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    if((B_48_63(REG1) == 0x8000) && (B_48_63(REG2) == 0x8000))  { v = 0x7fffffff; movl = 1; }
    else                                                        { v = tmp; movl = 0;        }

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmhossfaaw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool movh, movl, ovh, ovl;

    tmp   = SF(B_16_31(REG1), B_16_31(REG2));
    if((B_16_31(REG1) == 0x8000) && (B_16_31(REG2) == 0x8000))  { tmp = 0x7fffffff; movh = 1; }
    else                                                        { movh = 0;                   }
    tmp64 = EXTS_W2D(B_0_31(ACC)) + EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp   = SF(B_48_63(REG1), B_48_63(REG2));
    if((B_48_63(REG1) == 0x8000) && (B_48_63(REG2) == 0x8000))  { tmp = 0x7fffffff; movl = 1; }
    else                                                        { movl = 0;                   }
    tmp64 = EXTS_W2D(B_32_63(ACC)) + EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh | movh, ovl | movl);
}

X(evmhossfanw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool movh, movl, ovh, ovl;

    tmp   = SF(B_16_31(REG1), B_16_31(REG2));
    if((B_16_31(REG1) == 0x8000) && (B_16_31(REG2) == 0x8000))  { tmp = 0x7fffffff; movh = 1; }
    else                                                        { movh = 0;                   }
    tmp64 = EXTS_W2D(B_0_31(ACC)) - EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp   = SF(B_48_63(REG1), B_48_63(REG2));
    if((B_48_63(REG1) == 0x8000) && (B_48_63(REG2) == 0x8000))  { tmp = 0x7fffffff; movl = 1; }
    else                                                        { movl = 0;                   }
    tmp64 = EXTS_W2D(B_32_63(ACC)) - EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh | movh, ovl | movl);
}

X(evmhossiaaw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool ovh, ovl;

    tmp   = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    tmp64 = EXTS_W2D(B_0_31(ACC)) + EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(B_32_63(ACC)) + EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmhossianw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool ovh, ovl;

    tmp   = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    tmp64 = EXTS_W2D(B_0_31(ACC)) - EXTS_W2D(tmp);
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(B_32_63(ACC)) - EXTS_W2D(tmp);
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmhoumi)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
}

X(evmhoumia)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhoumiaaw)
{
    uint32_t tmp, u, v;

    tmp  = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    u    = B_0_31(ACC) + tmp;

    tmp  = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    v    = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhoumianw)
{
    uint32_t tmp, u, v;

    tmp  = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    u    = B_0_31(ACC) - tmp;

    tmp  = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    v    = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmhousiaaw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool ovh, ovl;

    tmp   = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    tmp64 = EXTZ_W2D(B_0_31(ACC)) + EXTZ_W2D(tmp);
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, 0, 0xfffffff, 0xffffffff, B_32_63(tmp64));

    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(B_32_63(ACC)) + EXTZ_W2D(tmp);
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, 0, 0xffffffff, 0xffffffff, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmhousianw)
{
    uint32_t tmp, u, v;
    uint64_t tmp64;
    bool ovh, ovl;

    tmp   = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    tmp64 = EXTZ_W2D(B_0_31(ACC)) - EXTZ_W2D(tmp);
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, 0, 0x0000000, 0x00000000, B_32_63(tmp64));

    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(B_32_63(ACC)) - EXTZ_W2D(tmp);
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, 0, 0x00000000, 0x00000000, B_32_63(tmp64));

    REG0  = (U64(u) << 32) | U64(v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmra)
{
    ACC  = REG1;
    REG0 = REG1;
}

X(evmwhsmf)
{
    uint32_t u, v;

    u = B_0_31(SF(B_0_31(REG1), B_0_31(REG2)));
    v = B_0_31(SF(B_32_63(REG1), B_32_63(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
}

X(evmwhsmfa)
{
    uint32_t u, v;

    u = B_0_31(SF(B_0_31(REG1), B_0_31(REG2)));
    v = B_0_31(SF(B_32_63(REG1), B_32_63(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmwhssf)
{
    uint64_t tmp64;
    uint32_t u, v;
    bool movh, movl;

    tmp64 = SF(B_0_31(REG1), B_0_31(REG2));
    if((B_0_31(REG1) == 0x80000000) && (B_0_31(REG2) == 0x80000000))  { u = 0x7fffffff; movh = 1;    }
    else                                                              { u = B_0_31(tmp64); movh = 0; }

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { v = 0x7fffffff; movl = 1;    }
    else                                                               { v = B_0_31(tmp64); movl = 0; }

    REG0 = (U64(u) << 32) | U64(v);
    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmwhssfa)
{
    uint64_t tmp64;
    uint32_t u, v;
    bool movh, movl;

    tmp64 = SF(B_0_31(REG1), B_0_31(REG2));
    if((B_0_31(REG1) == 0x80000000) && (B_0_31(REG2) == 0x80000000))  { u = 0x7fffffff; movh = 1;    }
    else                                                              { u = B_0_31(tmp64); movh = 0; }

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { v = 0x7fffffff; movl = 1;    }
    else                                                               { v = B_0_31(tmp64); movl = 0; }

    REG0 = (U64(u) << 32) | U64(v);
    ACC = REG0;
    UPDATE_SPEFSCR_OV(movh, movl);
}

X(evmwhumi)
{
    uint32_t u, v;
    u    = X86_MULUW_H(B_0_31(REG1), B_0_31(REG2));      // get high word
    v    = X86_MULUW_H(B_32_63(REG1), B_32_63(REG2));    // get high word
    REG0 = (U64(u) << 32) | U64(v);
}

X(evmwhumia)
{
    uint32_t u, v;
    u    = X86_MULUW_H(B_0_31(REG1), B_0_31(REG2));      // get high word
    v    = X86_MULUW_H(B_32_63(REG1), B_32_63(REG2));    // get high word
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmwlsmiaaw)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  + X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) + X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmwlsmianw)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) - X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X(evmwlssiaaw)
{
    uint64_t tmp64;
    uint32_t u, v;
    bool ovh, ovl;

    tmp64 = X86_MULWF(B_0_31(REG1), B_0_31(REG2));
    tmp64 = EXTS_W2D(B_0_31(ACC)) + EXTS_W2D(B_32_63(tmp64));
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp64 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    tmp64 = EXTS_W2D(B_32_63(ACC)) + EXTS_W2D(B_32_63(tmp64));
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x8000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = PACK_2W(u, v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmwlssianw)
{
    uint64_t tmp64;
    uint32_t u, v;
    bool ovh, ovl;

    tmp64 = X86_MULWF(B_0_31(REG1), B_0_31(REG2));
    tmp64 = EXTS_W2D(B_0_31(ACC)) - EXTS_W2D(B_32_63(tmp64));
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp64 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    tmp64 = EXTS_W2D(B_32_63(ACC)) - EXTS_W2D(B_32_63(tmp64));
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x8000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = PACK_2W(u, v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmwlumi)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_0_31(REG1), B_0_31(REG2));
    v    = X86_MULUW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
}

X(evmwlumia)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_0_31(REG1), B_0_31(REG2));
    v    = X86_MULUW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evmwlumiaaw)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  + X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) + X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evmwlumianw)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) - X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evmwlusiaaw)
{
    uint64_t tmp64;
    uint32_t u, v;
    bool ovh, ovl;

    tmp64 = X86_MULUWF(B_0_31(REG1), B_0_31(REG2));
    tmp64 = EXTZ_W2D(B_0_31(ACC)) + EXTZ_W2D(B_32_63(tmp64));
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, 0, 0xffffffff, 0xffffffff, B_32_63(tmp64));

    tmp64 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    tmp64 = EXTZ_W2D(B_32_63(ACC)) + EXTZ_W2D(B_32_63(tmp64));
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, 0, 0xffffffff, 0xffffffff, B_32_63(tmp64));

    REG0  = PACK_2W(u, v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmwlusianw)
{
    uint64_t tmp64;
    uint32_t u, v;
    bool ovh, ovl;

    tmp64 = X86_MULUWF(B_0_31(REG1), B_0_31(REG2));
    tmp64 = EXTZ_W2D(B_0_31(ACC)) - EXTZ_W2D(B_32_63(tmp64));
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, 0, 0x00000000, 0x00000000, B_32_63(tmp64));

    tmp64 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    tmp64 = EXTZ_W2D(B_32_63(ACC)) - EXTZ_W2D(B_32_63(tmp64));
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, 0, 0x00000000, 0x00000000, B_32_63(tmp64));

    REG0  = PACK_2W(u, v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evmwsmf)
{
    REG0 = SF(B_32_63(REG1), B_32_63(REG2));
}

X(evmwsmfa)
{
    REG0 = SF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evmwsmfaa)
{
    uint64_t tmp64;
    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X(evmwsmfan)
{
    uint64_t tmp64;
    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X(evmwsmi)
{
    REG0 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
}

X(evmwsmia)
{
    REG0 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evmwsmiaa)
{
    REG0 = ACC + X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evmwsmian)
{
    REG0 = ACC - X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evmwssf)
{
    uint64_t tmp64;
    bool mov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { REG0 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { REG0 = tmp64; mov = 0;                 }

    UPDATE_SPEFSCR_OV(0, mov);
}

X(evmwssfa)
{
    uint64_t tmp64;
    bool mov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { REG0 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { REG0 = tmp64; mov = 0;                 }
    ACC = REG0;

    UPDATE_SPEFSCR_OV(0, mov);
}

X(evmwssfaa)
{
    uint64_t tmp64;
    bool mov, ov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { tmp64 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { mov = 0;                                }
    REG0 = X86_ADD64(ACC, tmp64);
    ov   = GET_CF_H() ^ B_N(REG0, 0);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(0, ov | mov);
}

X(evmwssfan)
{
    uint64_t tmp64;
    bool mov, ov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { tmp64 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { mov = 0;                                }
    REG0 = X86_SUB64(ACC, tmp64);
    ov   = GET_CF_H() ^ B_N(REG0, 0);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(0, ov | mov);
}

X(evmwumi)
{
    REG0 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
}

X(evmwumia)
{
    REG0 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evmwumiaa)
{
    REG0 = ACC + X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evmwumian)
{
    REG0 = ACC - X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X(evnand)
{
    REG0 = ~(REG1 & REG2);
}

X(evneg)
{
    REG0 = PACK_2W(X86_NEGW(B_0_31(REG1)), X86_NEGW(B_32_63(REG1)));
}

X(evnor)
{
    REG0 = ~(REG1 | REG2);
}

X(evor)
{
    REG0 = REG1 | REG2;
}

X(evorc)
{
    REG0 = REG1 | ~REG2;
}

X(evrlw)
{
    int nh, nl;
    uint32_t u, v;

    nh   = B_27_31(REG2);
    nl   = B_59_63(REG2);
    u    = ROTL32(B_0_31(REG1), nh);
    v    = ROTL32(B_32_63(REG1), nl);
    REG0 = PACK_2W(u, v);
}

X(evrlwi)
{
    uint32_t u, v;
    int n = ARG2;
    u     = ROTL32(B_0_31(REG1), n);
    v     = ROTL32(B_32_63(REG1), n);
    REG0  = PACK_2W(u, v);
}

X(evrndw)
{
    uint32_t u, v;
    u    = (B_0_31(REG1)  + 0x8000) & 0xffff0000;
    v    = (B_32_63(REG1) + 0x8000) & 0xffff0000;
    REG0 = PACK_2W(u, v);
}

X(evsel)
{
    bool ch = 0;
    bool cl = 0;
    uint32_t u, v;

    ch = get_crf(4*ARG3);
    cl = get_crf(4*ARG3 + 1);

    if(ch) { u = B_0_31(REG1);  }
    else   { u = B_0_31(REG2);  }
    if(cl) { v = B_32_63(REG1); }
    else   { v = B_32_63(REG2); }

    REG0 = PACK_2W(u, v);
}


X(evslw)
{
    int nh, nl;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    REG0 = PACK_2W(SL(B_0_31(REG1), nh), SL(B_32_63(REG1), nl));
}

X(evslwi)
{
    int n = ARG2;
    REG0  = PACK_2W(SL(B_0_31(REG1), n), SL(B_32_63(REG2), n));
}

X(evsplatfi)
{
    uint32_t u;
    u    = ((ARG1 & 0x1f) << 27);
    REG0 = PACK_2W(u, u);
}

X(evsplati)
{
    uint32_t u = EXTS_5B_2_32B(ARG1);
    REG0       = PACK_2W(u, u);
}

X(evsrwis)
{
    int n = ARG2;
    uint32_t u, v;

    u     = U32(EXTS_BF2D(REG0, 0, 31-n));
    v     = U32(EXTS_BF2D(REG0, 32, 63-n));
    REG0  = PACK_2W(u, v);
}

X(evsrwiu)
{
    int n = ARG2;
    uint32_t u, v;

    u    = U32(EXTZ_BF2D(REG0, 0, 31-n));
    v    = U32(EXTZ_BF2D(REG0, 32, 63-n));
    REG0 = PACK_2W(u, v);
}

X(evsrws)
{
    int nh, nl;
    uint32_t u, v;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    u    = U32(EXTS_BF2D((REG1), 0, 31-nh));
    v    = U32(EXTS_BF2D((REG1), 32, 63-nl));
    REG0 = PACK_2W(u, v);
}

X(evsrwu)
{
    int nh, nl;
    uint32_t u, v;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    u    = U32(EXTZ_BF2D((REG1), 0, 31-nh));
    v    = U32(EXTZ_BF2D((REG1), 32, 63-nl));
    REG0 = PACK_2W(u, v);
}

X(evstdd)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    REG0 = LOAD64(ea);
}

X(evstddx)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    REG0 = LOAD64(ea);
}

X(evstdh)
{
    UMODE b = 0, ea;
    uint16_t u, v, w, x;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    w    = LOAD16(ea+4);
    x    = LOAD16(ea+6);
    REG0 = (U64(u) << 48) | (U64(v) << 32) | (U64(w) << 16) | U64(x);
}

X(evstdhx)
{
    UMODE b = 0, ea;
    uint16_t u, v, w, x;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    w    = LOAD16(ea+4);
    x    = LOAD16(ea+6);
    REG0 = (U64(u) << 48) | (U64(v) << 32) | (U64(w) << 16) | U64(x);
}

X(evstdw)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    v    = LOAD32(ea+4);
    REG0 = PACK_2W(u, v);
}

X(evstdwx)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    v    = LOAD32(ea+4);
    REG0 = PACK_2W(u, v);
}

X(evstwhe)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 48) | (U64(v) << 16);
}

X(evstwhex)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 48) | (U64(v) << 16);
}

X(evstwho)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 32) | U64(v);
}

X(evstwhox)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 32) | U64(v);
}

X(evstwwe)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    REG0 = (U64(u) << 32);
}

X(evstwwex)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    REG0 = (U64(u) << 32);
}

X(evstwwo)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    REG0 = U64(u);
}

X(evstwwox)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    REG0 = U64(u);
}

X(evsubfsmiaaw)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - B_0_31(REG1);
    v    = B_32_63(ACC) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evsubfssiaaw)
{
    uint32_t u, v;
    uint64_t tmp64;
    bool ovh, ovl;

    tmp64 = EXTS_W2D(B_0_31(ACC)) - EXTS_W2D(B_0_31(REG1));
    ovh   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    u     = SATURATE(ovh, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    tmp64 = EXTS_W2D(B_32_63(ACC)) - EXTS_W2D(B_32_63(REG1));
    ovl   = B_N(tmp64, 31) ^ B_N(tmp64, 32);
    v     = SATURATE(ovl, B_N(tmp64, 31), 0x80000000, 0x7fffffff, B_32_63(tmp64));

    REG0  = PACK_2W(u, v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

// NOTE: don't know what's the actual difference between evsubfsmiaaw & evsubfumiaaw
//       Techically signed subtraction & unsigned subtraction return the same final
//       value according to rules of 2's complement arithmetic. The diff lies only in
//       interpretation of final result.
X(evsubfumiaaw)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - B_0_31(REG1);
    v    = B_32_63(ACC) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X(evsubfw)
{
    uint32_t u, v;
    u    = B_0_31(REG2)  - B_0_31(REG1);
    v    = B_32_63(REG2) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
}

X(evsubfusiaaw)
{
    uint32_t u, v;
    uint64_t tmp64;
    bool ovh, ovl;

    tmp64 = EXTZ_W2D(B_0_31(ACC)) - EXTZ_W2D(B_0_31(REG1));
    ovh   = B_N(tmp64, 31);
    u     = SATURATE(ovh, ovh, 0x00000000, 0x00000000, B_32_63(tmp64));

    tmp64 = EXTZ_W2D(B_32_63(ACC)) - EXTZ_W2D(B_32_63(REG1));
    ovl   = B_N(tmp64, 31);
    v     = SATURATE(ovl, ovl, 0x00000000, 0x00000000, B_32_63(tmp64));

    REG0  = PACK_2W(u, v);
    ACC   = REG0;

    UPDATE_SPEFSCR_OV(ovh, ovl);
}

X(evsubifw)
{
    uint32_t u, v;
    u    = B_0_31(REG2)  - (ARG1 & 0x1f);
    v    = B_32_63(REG2) - (ARG1 & 0x1f);
    REG0 = PACK_2W(u, v);
}

X(evxor)
{
    REG0 = REG1 ^ REG2;
}

// ----------------- SPE FP ---------------------------------------------------------------------------

X(efdabs)
{
    REG0 = REG1 & 0x7fffffffffffffffULL;       // Change sign bit to zero
}
X(efdnabs)
{
    REG0 = (REG1 & 0x7fffffffffffffffULL) | 0x8000000000000000ULL;
}
X(efdneg)
{
    REG0 = (REG1 & 0x7fffffffffffffffULL) | ((REG1 ^ 0x8000000000000000ULL) & 0x8000000000000000ULL);
}

