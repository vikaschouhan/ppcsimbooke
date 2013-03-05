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

X("add", ___add___)
{
#define add_code(rD, rA, rB)           \
    rD = X86_ADDW(rA, rB);

    add_code(REG0, REG1, REG2);
}
X("add.", ___add_dot___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X("addo", ___addo___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X("addo.", ___addo_dot___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

// These instrs always update XER[CA] flag
X("addc", ___addc___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
}
X("addc.", ___addc_dot___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_CR0();
}
X("addco", ___addco___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X("addco.", ___addco_dot___)
{
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

/* Add extended : rA + rB + CA */
X("adde", ___adde___)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X("adde.", ___adde_dot___)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}
X("addeo", ___addeo___)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X("addeo.", ___addeo_dot___)
{
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X("addi", ___addi___)
{
    SMODE tmp = (int16_t)ARG2;
    if(ARG1){ add_code(REG0, REG1, tmp);        }
    else    { REG0 = (int16_t)ARG2;             }
}

X("addic", ___addic___)
{
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X("addic.", ___addic_dot___)
{
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}

X("addis", ___addis___)
{
    SMODE tmp = (((int16_t)ARG2) << 16);
    if(ARG1){ add_code(REG0, REG1, tmp);        }
    else    { REG0 = ((int16_t)ARG2) << 16;     }
}

// XER[CA] is always altered
X("addme", ___addme___)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X("addme.", ___addme_dot___)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}
X("addmeo", ___addmeo___)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X("addmeo.", ___addmeo_dot___)
{
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

// XER[CA] is always altered
X("addze", ___addze___)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
}
X("addze.", ___addze_dot___)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
}
X("addzeo", ___addzeo___)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X("addzeo.", ___addzeo_dot___)
{
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X("subf", ___subf___)
{
#define subf_code(rD, rA, rB)            \
    uint64_t subf_tmp = rB + 1;          \
    uint64_t inva = ~rA;                 \
    add_code(rD, inva, subf_tmp)

    subf_code(REG0, REG1, REG2);
}
X("subf.", ___subf_dot___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X("subfo", ___subfo___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X("subfo.", ___subfo_dot___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

// XER[CA] is altered
X("subfic", ___subfic___)
{
    uint64_t tmp = EXTS_H2N(ARG2);
    subf_code(REG0, REG1, tmp);
    UPDATE_CA();
}

// XER[CA] is always altered
X("subfc", ___subfc___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
}
X("subfc.", ___subfc_dot___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_CR0();
}
X("subfco", ___subfco___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
}
X("subfco.", ___subfco_dot___)
{
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X("subfe", ___subfe___)
{
#define subfe_code(rD, rA, rB)          \
    UMODE tmp = GET_CA() + rB;          \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfe_code(REG0, REG1, REG2);
}
X("subfe.", ___subfe_dot___)
{
    subfe_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X("subfeo", ___subfeo___)
{
    subfe_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X("subfeo.", ___subfeo_dot___)
{
    subfe_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X("subfme", ___subfme___)
{
#define subfme_code(rD, rA)             \
    SMODE tmp = GET_CA() - 1;           \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfme_code(REG0, REG1);
}
X("subfme.", ___subfme_dot___)
{
    subfme_code(REG0, REG1);
    UPDATE_CR0();
}
X("subfmeo", ___subfmeo___)
{
    subfme_code(REG0, REG1);
    UPDATE_SO_OV();
}
X("subfmeo.", ___subfmeo_dot___)
{
    subfme_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X("subfze", ___subfze___)
{
#define subfze_code(rD, rA)             \
    UMODE tmp = GET_CA();               \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfze_code(REG0, REG1);
}
X("subfze.", ___subfze_dot___)
{
    subfze_code(REG0, REG1);
    UPDATE_CR0();
}
X("subfzeo", ___subfzeo___)
{
    subfze_code(REG0, REG1);
    UPDATE_SO_OV();
}
X("subfzeo.", ___subfzeo_dot___)
{
    subfze_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

X("divw", ___divw___)
{
#define divw_code(rD, rA, rB)          \
    if(rB){ rD = X86_DIVW(rA, rB); }

    divw_code(REG0, REG1, REG2);
}
X("divw.", ___divw_dot___)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X("divwo", ___divwo___)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
}
X("divwo.", ___divwo_dot___)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    UPDATE_CR0_V(REG0);
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
}

X("divwu", ___divwu___)
{
#define divwu_code(rD, rA, rB)         \
    if(rB){ rD = X86_DIVUW(rA, rB); }

    divwu_code(REG0, REG1, REG2);
}
X("divwu.", ___divwu_dot___)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X("divwuo", ___divwuo___)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }   // Set OV=1 if division by zero
}
X("divwuo.", ___divwuo_dot___)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    UPDATE_CR0_V(REG0);
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }
}

X("mulhw", ___mulhw___)
{
#define mulhw_code(rD, rA, rB)            \
    rD = X86_MULW_H(rA, rB)

    mulhw_code(REG0, REG1, REG2);
}
X("mulhw.", ___mulhw_dot___)
{
    mulhw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}

X("mulli", ___mulli___)
{
    uint32_t imm_val = EXTS_H2W(ARG2);          // Sign extend 16 bit IMM to 32 bit
    REG0 = X86_MULW_L(REG1, imm_val);
}

X("mulhwu", ___mulhwu___)
{
#define mulhwu_code(rD, rA, rB)           \
    rD = X86_MULUW_H(rA, rB)

    mulhwu_code(REG0, REG1, REG2);
}
X("mulhwu.", ___mulhwu_dot___)
{
    mulhwu_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);                 // muluw doesn't set SF or ZF, hence we have to set CR0 by value
}

X("mullw", ___mullw___)
{
#define mullw_code(rD, rA, rB)            \
    rD = X86_MULW_L(rA, rB)

    mullw_code(REG0, REG1, REG2);
}
X("mullw.", ___mullw_dot___)
{
    mullw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X("mullwo", ___mullwo___)
{
    mullw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
}
X("mullwo.", ___mullwo_dot___)
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
X("and", ___and___)
{
#define and_code(rA, rS, rB)             \
    rA = X86_ANDW(rS, rB)

    and_code(REG0, REG1, REG2);
}
X("and.", ___and_dot___)
{
    and_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X("andc", ___andc___)
{
    UMODE tmp = ~REG2;
    and_code(REG0, REG1, tmp);
}
X("andc.", ___andc_dot___)
{
    UMODE tmp = ~REG2;
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
}

X("andi.", ___andi_dot___)
{
    UMODE tmp = (uint16_t)ARG2;
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
}

X("andis.", ___andis_dot___)
{
    UMODE tmp = (((uint16_t)ARG2) << 16);
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
}

// Byte extend
X("extsb", ___extsb___)
{
    REG0 = EXTS_B2N(REG1);
}
X("extsb.", ___extsb_dot___)
{
    REG0 = EXTS_B2N(REG1);
    UPDATE_CR0_V(REG0);
}

// Halfword extend
X("extsh", ___extsh___)
{
    REG0 = EXTS_H2N(REG1);
}
X("extsh.", ___extsh_dot___)
{
    REG0 = EXTS_H2N(REG1);
    UPDATE_CR0_V(REG0);
}

// cntlzw:  Count leading zeroes (32-bit word).
X("cntlzw", ___cntlzw___)
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
X("cntlzw.", ___cntlzw_dot___)
{
    cntlzw_code(REG0, REG1);
    UPDATE_CR0_V(REG0);
}

X("eqv", ___eqv___)
{
#define eqv_code(rD, rA, rB)                   \
    rD = ~(rA ^ rB)

    eqv_code(REG0, REG1, REG2);
}
X("eqv.", ___eqv_dot___)
{
    eqv_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}

X("nand", ___nand___)
{
#define nand_code(rA, rS, rB)                  \
    rA = ~(rS & rB)

    nand_code(REG0, REG1, REG2);
}
X("nand.", ___nand_dot___)
{
    nand_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}
X("neg", ___neg___)
{
#define neg_code(rD, rA)                        \
    rD = X86_NEGW(rA);

    neg_code(REG0, REG1);
}
X("neg.", ___neg_dot___)
{
    neg_code(REG0, REG1);
    UPDATE_CR0();
}
X("nego", ___nego___)
{
    neg_code(REG0, REG1);
    UPDATE_SO_OV();
}
X("nego.", ___nego_dot___)
{
    neg_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
}

//
X("nor", ___nor___)
{
#define nor_code(rA, rS, rB)                    \
    rA = ~(rS | rB)

    nor_code(REG0, REG1, REG2);
}
X("nor.", ___nor_dot___)
{
    nor_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
}

X("or", ___or___)
{
#define or_code(rA, rS, rB)                     \
    rA = X86_ORW(rS, rB)

    or_code(REG0, REG1, REG2);
}
X("or.", ___or_dot___)
{
    or_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X("orc", ___orc___)
{
#define orc_code(rA, rS, rB)                     \
    UMODE tmp = ~rB;                             \
    rA = X86_ORW(rS, tmp)

    orc_code(REG0, REG1, REG2);
}
X("orc.", ___orc_dot___)
{
    orc_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X("ori", ___ori___)
{
    UMODE tmp = (uint16_t)(ARG2);
    or_code(REG0, REG1, tmp);
}

X("oris", ___oris___)
{
    UMODE tmp = (((uint16_t)ARG2) << 16);
    or_code(REG0, REG1, tmp);
}

// xor variants
X("xor", ___xor___)
{
#define xor_code(rA, rS, rB)      \
    rA = X86_XORW(rS, rB)

    xor_code(REG0, REG1, REG2);
}
X("xor.", ___xor_dot___)
{
    xor_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X("xori", ___xori___)
{
    UMODE tmp = ((uint16_t)ARG2);
    xor_code(REG0, REG1, tmp);
}

X("xoris", ___xoris___)
{
    UMODE tmp = (((uint16_t)ARG2) << 16);
    xor_code(REG0, REG1, tmp);
}

// START
// ------------------------------- BPU ---------------------------------------------
// BTB instrs
X("bbelr", ___bbelr___)
{
    // Not implemented
}
X("bblels", ___bblels___)
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
X("b", ___b___)
{
#define b_code(tgtaddr)      \
    NIP = tgtaddr

    b_code(ARG0);
}
X("ba", ___ba___)
{
    b_code(ARG0);
}
X("bl", ___bl___)
{
    b_code(ARG0);
    LR = (PC + 4);
}
X("bla", ___bla___)
{
    b_code(ARG0);
    LR = (PC + 4);
}

// branch conditional
X("bc", ___bc___)
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
X("bca", ___bca___)
{
    bc_code(ARG0, ARG1, ARG2)
}
X("bcl", ___bcl___)
{
#define bcl_code(BO, BI, tgtaddr)                   \
    bc_code(BO, BI, tgtaddr);                       \
    LR = (PC + 4);

    bcl_code(ARG0, ARG1, ARG2);
}
X("bcla", ___bcla___)
{
    bcl_code(ARG0, ARG1, ARG2);
}

// branch conditional to LR
X("bclr", ___bclr___)
{
#define bclr_code(BO, BI, BH)                                                       \
    if(!BO2(BO)) CTR = CTR - 1;                                                     \
    int ctr_ok = BO2(BO) | ((((MSR_CM) ? CTR: (CTR & 0xffffffff)) != 0) ^ BO3(BO)); \
    int cond_ok = BO0(BO) | (get_crf(BI) == BO1(BO));                               \
    if(ctr_ok & cond_ok) NIP = LR & ~0x3;

    bclr_code(ARG0, ARG1, ARG2);
}
X("bclrl", ___bclrl___)
{
#define bclrl_code(BO, BI, BH)                       \
    bclr_code(BO, BI, BH);                           \
    LR = (PC + 4);

    bclrl_code(ARG0, ARG1, ARG2);
}

// branch conditional to CTR
X("bcctr", ___bcctr___)
{
#define bcctr_code(BO, BI, BH)                                                       \
    if(BO0(BO) | (get_crf(BI) == BO1(BO))) NIP = CTR & ~0x3;
    
    bcctr_code(ARG0, ARG1, ARG2);
}
X("bcctrl", ___bcctrl___)
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
X("cmp", ___cmp___)
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
X("cmpi", ___cmpi___)
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
X("cmpl", ___cmpl___)
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
X("cmpli", ___cmpli___)
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

X("crand", ___crand___)
{
#define crand_code(crD, crA, crB)                       \
    update_crf(crD, get_crf(crA) & get_crf(crB));

    crand_code(ARG0, ARG1, ARG2);
}
X("crandc", ___crandc___)
{
#define crandc_code(crD, crA, crB)                      \
    update_crf(crD, get_crf(crA) & ~(get_crf(crB)));

    crandc_code(ARG0, ARG1, ARG2);
}
X("creqv", ___creqv___)
{
#define creqv_code(crD, crA, crB)                      \
    update_crf(crD, ~(get_crf(crA) ^ get_crf(crB)));

    creqv_code(ARG0, ARG1, ARG2);
}
X("crnand", ___crnand___)
{
#define crnand_code(crD, crA, crB)                      \
    update_crf(crD, ~(get_crf(crA) & get_crf(crB)));

    crnand_code(ARG0, ARG1, ARG2);
}
X("crnor", ___crnor___)
{
#define crnor_code(crD, crA, crB)                       \
    update_crf(crD, ~(get_crf(crA) | get_crf(crB)));

    crnor_code(ARG0, ARG1, ARG2);
}
X("cror", ___cror___)
{
#define cror_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) | get_crf(crB)));

    cror_code(ARG0, ARG1, ARG2);
}
X("crorc", ___crorc___)
{
#define crorc_code(crD, crA, crB)                       \
    update_crf(crD, (get_crf(crA) | ~(get_crf(crB))));

    crorc_code(ARG0, ARG1, ARG2);
}
X("crxor", ___crxor___)
{
#define crxor_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) ^ get_crf(crB)));

    crxor_code(ARG0, ARG1, ARG2);
}
X("mcrf", ___mcrf___)
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
X("rlwimi", ___rlwimi___)
{
#define rlwimi_code(rA, rS, SH, MB, ME)                                            \
    uint64_t n = (SH & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB + 32, ME + 32);                                           \
    rA = (r & m) | (rA & ~m)

    rlwimi_code(REG0, REG1, ARG2, ARG3, ARG4);
}
X("rlwimi.", ___rlwimi_dot___)
{
    rlwimi_code(REG0, REG1, ARG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
}
X("rlwinm", ___rlwinm___)
{
#define rlwinm_code(rA, rS, SH, MB, ME)                                            \
    uint64_t n = (SH & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB + 32, ME + 32);                                           \
    rA = (r & m)

    rlwinm_code(REG0, REG1, ARG2, ARG3, ARG4);
}
X("rlwinm.", ___rlwinm_dot___)
{
    rlwinm_code(REG0, REG1, ARG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
}
X("rlwnm", ___rlwnm___)
{
#define rlwnm_code(rA, rS, rB, MB, ME)                                             \
    uint64_t n = (rB & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB+32, ME+32);                                               \
    rA = (r & m)

    rlwnm_code(REG0, REG1, REG2, ARG3, ARG4);
}
X("rlwnm.", ___rlwnm_dot___)
{
    rlwnm_code(REG0, REG1, REG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
}

// Shift instrs
X("slw", ___slw___)
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
X("slw.", ___slw_dot___)
{
    slw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
}
X("sraw", ___sraw___)
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
X("sraw.", ___sraw_dot___)
{
    sraw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
}
X("srawi", ___srawi___)
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
X("srawi.", ___srawi_dot___)
{
    srawi_code(REG0, REG1, ARG2);
    UPDATE_CR0_V(UT(REG0));
}
X("srw", ___srw___)
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
X("srw.", ___srw_dot___)
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
X("lbz", ___lbz___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD8(ea);    
}
X("lbzx", ___lbzx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD8(ea);
}
X("lbzu", ___lbzu___)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD8(ea);
    REG2 = ea;
}
X("lbzux", ___lbzux___)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD8(ea);
    REG1 = ea;
}

// Halfword algebraic loads
X("lha", ___lha___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = EXTS_H2N(LOAD16(ea));
}
X("lhax", ___lhax___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = EXTS_H2N(LOAD16(ea));
}
X("lhau", ___lhau___)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = EXTS_H2N(LOAD16(ea));
    REG2 = ea;
}
X("lhaux", ___lhaux___)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = EXTS_H2N(LOAD16(ea));
    REG1 = ea;
}

// Half word loads
X("lhz", ___lhz___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD16(ea);
}
X("lhzx", ___lhzx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD16(ea);
}
X("lhzu", ___lhzu___)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD16(ea);
    REG2 = ea;
}
X("lhzux", ___lhzux___)
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
X("lwz", ___lwz___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD32(ea);
}
// lwzx rD,rA,rB
X("lwzx", ___lwzx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD32(ea);
}
//  lwzu rD,D(rA)
X("lwzu", ___lwzu___)
{
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD32(ea);
    REG2 = ea;
}
X("lwzux", ___lwzux___)
{
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD32(ea);
    REG1 = ea;
}

// Byte reversed indexed loads
X("lhbrx", ___lhbrx___)
{
    UMODE tmp = 0;
    UMODE ea;
    uint16_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = LOAD16(ea);
    REG0 = SWAPB16(data);
}
X("lwbrx", ___lwbrx___)
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
X("lmw", ___lmw___)
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
X("stb", ___stb___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE8(ea, REG0);
}
X("stbx", ___stbx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE8(ea, REG0);
}
X("stbu", ___stbu___)
{
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE8(ea, REG0);
    REG2 = ea;
}
X("stbux", ___stbux___)
{
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE8(ea, REG0);
    REG1 = ea;
}

// half word stores
X("sth", ___sth___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE16(ea, REG0);
}
X("sthx", ___sthx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE16(ea, REG0);
}
X("sthu", ___sthu___)
{
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE16(ea, REG0);
    REG2 = ea;
}
X("sthux", ___sthux___)
{
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE16(ea, REG0);
    REG1 = ea;
}

// word stores
X("stw", ___stw___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE32(ea, REG0);
}
X("stwx", ___stwx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE32(ea, REG0);
}
X("stwu", ___stwu___)
{
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE32(ea, REG0);
    REG2 = ea;
}
X("stwux", ___stwux___)
{
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE32(ea, REG0);
    REG1 = ea;
}

// byte reversed stores
X("sthbrx", ___sthbrx___)
{
    UMODE tmp = 0;
    UMODE ea;
    uint16_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = REG0;
    STORE16(ea, SWAPB16(data));
}
X("stwbrx", ___stwbrx___)
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
X("stmw", ___stmw___)
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

X("tlbwe", ___tlbwe___)
{
    TLBWE();
}

X("tlbre", ___tlbre___)
{
    TLBRE();
}

X("tlbsx", ___tlbsx___)
{
    uint64_t ea = REG1;
    if(ARG0){ ea += REG0; }
    TLBSX(ea);
}

X("tlbivax", ___tlbivax___)
{
    uint64_t ea = REG1;
    if(ARG0){ ea += REG0; }
    TLBIVAX(ea);
}

X("tlbsync", ___tlbsync___)
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

X("dcba", ___dcba___)
{
    // dummy
}

X("dcbf", ___dcbf___)
{
    // dummy
}

X("dcbi", ___dcbi___)
{
    // dummy
}

X("dcblc", ___dcblc___)
{
    // dummy
}

X("dcbz", ___dcbz___)
{
    // dummy
}

X("dcbst", ___dcbst___)
{
    // dummy
}

X("dcbt", ___dcbt___)
{
    // dummy
}

X("dcbtst", ___dcbtst___)
{
    // dummy
}

X("dcbtstls", ___dcbtstls___)
{
    // dummy
}

X("icbi", ___icbi___)
{
    // dummy
}

X("icblc", ___icblc___)
{
    // dummy
}

X("icbt", ___icbt___)
{
    // dummy
}

X("icbtls", ___icbtls___)
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

X("rfi", ___rfi___)
{
    MSR = SRR1;
    NIP = ((UMODE)SRR0) & ~0x3ULL;       // Mask Lower 2 bits to zero
}

X("rfmci", ___rfmci___)
{
    MSR = MCSRR1;
    NIP = ((UMODE)MCSRR0) & ~0x3ULL;
}

X("rfci", ___rfci___)
{
    MSR = CSRR1;
    NIP = ((UMODE)CSRR0) & ~0x3ULL;
}

X("sc", ___sc___)
{
    throw PPC_EXCEPT(PPC_EXCEPT_SC, "system call");     // raise a system call exception
}

X("mfmsr", ___mfmsr___)
{
    uint64_t newmsr = B_32_63(REG0);
    uint8_t newmsr_cm = ((newmsr & MSR_CM) ? 1:0);
    if((MSR_CM == 0) && (newmsr_cm == 1)) { NIP &= 0xffffffff; }

    // Another check is required for MSR_GS == 1, but since we don't have guest mode
    // this is irrelevant.
    MSR = newmsr;
}

X("mtmsr", ___mtmsr___)
{
    REG0 = B_32_63(MSR);
}

X("wrtee", ___wrtee___)
{
#define wrtee_code(rS)            \
    MSR &= ~(1L << 15);           \
    MSR |= rS & (1L << 15);

    wrtee_code(REG0);
}
X("wrteei", ___wrteei___)
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

X("twi", ___twi___)
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

X("tw", ___tw___)
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

X("mtcrf", ___mtcrf___)
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

X("mcrxr", ___mcrxr___)
{
#define mcrxr_code(crfD)                 \
    update_crF(crfD, get_xerF(0));       \
    /* clear XER[32:35] */               \
    XER &= 0xfffffff

    mcrxr_code(ARG0);
}

X("mfcr", ___mfcr___)
{
#define mfcr_code(rD)                               \
    rD = B_32_63(CR)

    mfcr_code(ARG0);
}

X("mfspr", ___mfspr___)
{
#define mfspr_code(rD, SPRN)                   \
    rD = SPR(SPRN);

    mfspr_code(REG0, ARG1);
}

X("mtspr", ___mtspr___)
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

X("isync", ___isync___)
{
    // Do nothing
}

// Barrier
X("mbar", ___mbar___)
{
    //Do nothing
}

X("msync", ___msync___)
{
    // Do nothing
}

// Reservation load
X("lwarx", ___lwarx___)
{
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    SET_RESV(ea, 4);
    REG0 = LOAD32(ea);
}

// Reservation store
X("stwcx.", ___stwcx_dot___)
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

X("mtpmr", ___mtpmr___)
{
#define mtpmr_code(PMRN, rS)                  \
    PMR(PMRN) = rS;

    mtpmr_code(ARG0, REG1);
}

X("mfpmr", ___mfpmr___)
{
#define mfpmr_code(rD, PMRN)                  \
    rD = PMR(PMRN);

    mfpmr_code(ARG0, REG1);
}

// START
// ------------------------------ INTEGER SELECT -----------------------------------
// mnemonics :
//              isel

X("isel", ___isel___)
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

X("brinc", ___brinc___)
{
    // FIXME : Verify value of n for e500v2
    int n         = 32;  // Implementation dependant
    uint64_t mask = (MASK(64-n, 63) & REG2);
    uint64_t a    = (MASK(64-n, 63) & REG1);

    uint64_t d    = BITREVERSE(1 + BITREVERSE(a | ~mask));
    REG0          = (MASK(0, 63-n) & REG1) | (d & mask);
}

X("evabs", ___evabs___)
{
    uint32_t u, v;
    u    = ABS_32(B_0_31(REG1));
    v    = ABS_32(B_32_63(REG1));
    REG0 = PACK_2W(u, v);
}

X("evaddiw", ___evaddiw___)
{
    uint32_t u, v;
    u    = B_32_63(B_0_31(REG1) + (ARG2 & 0xfffff));
    v    = B_32_63(B_32_63(REG1) +(ARG2 & 0xfffff));
    // ARG2 is zero extended and added to each of lower & upper halves of REG1
    REG0 = PACK_2W(u, v);
}

X("evaddsmiaaw", ___evaddsmiaaw___)
{
    uint32_t u, v;
    u    = B_32_63(B_0_31(ACC) + B_0_31(REG1));
    v    = B_32_63(B_32_63(ACC) + B_32_63(REG1));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evaddssiaaw", ___evaddssiaaw___)
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

X("evaddusiaaw", ___evaddusiaaw___)
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

X("evaddumiaaw", ___evaddumiaaw___)
{
    uint32_t u, v;
    u     = B_32_63(B_0_31(ACC) + B_0_31(REG1));
    v     = B_32_63(B_32_63(ACC) + B_32_63(REG1));
    REG0  = PACK_2W(u, v);
    ACC   = REG0;
}

X("evaddw", ___evaddw___)
{
    uint32_t u, v;
    u     = B_32_63(B_0_31(REG1) + B_0_31(REG2));
    v     = B_32_63(B_32_63(REG1) + B_32_63(REG2));
    REG0  = PACK_2W(u, v);
}

X("evand", ___evand___)
{
    REG0  = REG1 & REG2;
}

X("evandc", ___evandc___)
{
    REG0  = REG1 & ~REG2;
}

X("evcmpeq", ___evcmpeq___)
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

X("evcmpgts", ___evcmpgts___)
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

X("evcmpgtu", ___evcmpgtu___)
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

X("evcmplts", ___evcmplts___)
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

X("evcmpltu", ___evcmpltu___)
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

X("evcntlsw", ___evcntlsw___)
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

X("evcntlzw", ___evcntlzw___)
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

X("evdivws", ___evdivws___)
{
    // TODO : Implement this
    throw sim_except_fatal("Not implemented !!");
}

X("evdivwu", ___evdivwu___)
{
    // TODO : Implement this
    throw sim_except_fatal("Not implemented !!");
}

X("eveqv", ___eveqv___)
{
    REG0 = REG1 ^ REG2;  // xor to compare
}

X("evextsb", ___evextsb___)
{
    uint32_t u, v;
    u    = B_32_63(EXTS_B2D(B_24_31(REG1)));
    v    = B_32_63(EXTS_B2D(B_56_63(REG1)));
    REG0 = PACK_2W(u, v);
}

X("evextsh", ___evextsh___)
{
    uint32_t u, v;
    u    = B_32_63(EXTS_H2D(B_16_31(REG1)));
    v    = B_32_63(EXTS_H2D(B_48_63(REG1)));
    REG0 = PACK_2W(u, v);
}

X("evldd", ___evldd___)
{
    UMODE b = 0;
    UMODE ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = LOAD64(ea);
}

X("evlddx", ___evlddx___)
{
    UMODE b = 0;
    UMODE ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = LOAD64(ea);
}

X("evldh", ___evldh___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea + 2)) << 32) | (U64(LOAD16(ea + 4)) << 16) | U64(LOAD16(ea + 6));
}

X("evldhx", ___evldhx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea + 2)) << 32) | (U64(LOAD16(ea + 4)) << 16) | U64(LOAD16(ea + 6));
}

X("evldw", ___evldw___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD32(ea)) << 32) | U64(LOAD32(ea + 4));
}

X("evldwx", ___evldwx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    REG0 = (U64(LOAD32(ea)) << 32) | U64(LOAD32(ea + 4));
}

X("evlhhesplat", ___evlhhesplat___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t v = LOAD16(ea);
    REG0 = ((U64(v) << 48) | (U64(v) << 16)) & 0xffff0000ffff0000ULL;
}

X("evlhhesplatx", ___evlhhesplatx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    uint16_t v = LOAD16(ea);
    REG0 = ((U64(v) << 48) | (U64(v) << 16)) & 0xffff0000ffff0000ULL;
}

X("evlhhossplat", ___evlhhossplat___)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    u    = B_32_63(EXTS_H2W(LOAD16(ea)));
    REG0 = PACK_2W(u, u);
}

X("evlhhossplatx", ___evlhhossplatx___)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    u    = B_32_63(EXTS_H2W(LOAD16(ea)));
    REG0 = PACK_2W(u, u);
}

X("evlhhousplat", ___evlhhousplat___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t v = LOAD16(ea);
    REG0 = (U64(v) << 32) | U64(v);
}

X("evlhhousplatx", ___evlhhousplatx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    uint16_t v = LOAD16(ea);
    REG0 = (U64(v) << 32) | U64(v);
}

X("evlwhe", ___evlwhe___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea+2)) << 16);
}

X("evlwhex", ___evlwhex___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea+2)) << 16);
}

X("evlwhos", ___evlwhos___)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;
    
    u    = B_32_63(EXTS_H2D(LOAD16(ea)));
    v    = B_32_63(EXTS_H2D(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X("evlwhosx", ___evlwhosx___)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    u    = B_32_63(EXTS_H2D(LOAD16(ea)));
    v    = B_32_63(EXTS_H2D(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X("evlwhou", ___evlwhou___)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    u    = B_32_63(U64(LOAD16(ea)));
    v    = B_32_63(U64(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X("evlwhoux", ___evlwhoux___)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    u    = B_32_63(U64(LOAD16(ea)));
    v    = B_32_63(U64(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
}

X("evlwhsplat", ___evlwhsplat___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t u = LOAD16(ea);
    uint16_t v = LOAD16(ea+2);

    REG0 = (U64(u) << 48) | (U64(u) << 32) | (U64(v) << 16) | U64(v);
}

X("evlwhsplatx", ___evlwhsplatx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    uint16_t u = LOAD16(ea);
    uint16_t v = LOAD16(ea+2);

    REG0 = (U64(u) << 48) | (U64(u) << 32) | (U64(v) << 16) | U64(v);
}

X("evlwwsplat", ___evlwwsplat___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint32_t u = LOAD32(ea);

    REG0 = (U64(u) << 32) | U64(u);
}

X("evlwwsplatx", ___evlwwsplatx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    uint32_t u = LOAD32(ea);

    REG0 = (U64(u) << 32) | U64(u);
}

X("evmergehi", ___evmergehi___)
{
    uint32_t u, v;
    u    = B_0_31(REG1);
    v    = B_0_31(REG2);
    REG0 = PACK_2W(u, v);
}

X("evmergelo", ___evmergelo___)
{
    uint32_t u, v;
    u    = B_32_63(REG1);
    v    = B_32_63(REG2);
    REG0 = PACK_2W(u, v);
}

X("evmergehilo", ___evmergehilo___)
{
    uint32_t u, v;
    u    = B_0_31(REG1);
    v    = B_32_63(REG2);
    REG0 = PACK_2W(u, v);
}

X("evmergelohi", ___evmergelohi___)
{
    uint32_t u, v;
    u    = B_32_63(REG1);
    v    = B_0_31(REG2);
    REG0 = PACK_2W(u, v);
}

X("evmhegsmfaa", ___evmhegsmfaa___)
{
    uint64_t tmp = GSF(B_32_47(REG1), B_32_47(REG2));
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X("evmhegsmfan", ___evmhegsmfan___)
{
    uint64_t tmp = GSF(B_32_47(REG1), B_32_47(REG2));
    REG0 = ACC - tmp;
    ACC  = REG0;
}

X("evmhegsmiaa", ___evmhegsmiaa___)
{
    uint64_t tmp = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));
    tmp = EXTS_W2D(tmp);
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X("evmhegsmian", ___evmhegsmian___)
{
    uint64_t tmp = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));
    tmp = EXTS_W2D(tmp);
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X("evmhegumiaa", ___evmhegumiaa___)
{
    uint64_t tmp = B_32_63(B_32_47(REG1) * B_32_47(REG2));
    REG0 = ACC + tmp;
    ACC  = REG0;
}

X("evmhegumian", ___evmhegumian___)
{
    uint64_t tmp = B_32_63(B_32_47(REG1) * B_32_47(REG2));
    REG0 = ACC - tmp;
    ACC  = REG0;
}

X("evmhesmf", ___evmhesmf___)
{
    uint32_t u, v;
    u    = B_32_63(SF(B_0_15(REG1), B_0_15(REG2)));
    v    = B_32_63(SF(B_32_47(REG1), B_32_47(REG2)));
    REG0 = PACK_2W(u, v);
}

X("evmhesmfa", ___evmhesmfa___)
{
    uint32_t u, v;
    u    = B_32_63(SF(B_0_15(REG1), B_0_15(REG2)));
    v    = B_32_63(SF(B_32_47(REG1), B_32_47(REG2)));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evmhesmfaaw", ___evmhesmfaaw___)
{
    uint32_t u = B_32_63(B_0_31(ACC)  + SF(B_0_15(REG1), B_0_15(REG2)));
    uint32_t v = B_32_63(B_32_63(ACC) + SF(B_32_47(REG1), B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhesmfanw", ___evmhesmfanw___)
{
    uint32_t u = B_32_63(B_0_31(ACC)  - SF(B_0_15(REG1), B_0_15(REG2)));
    uint32_t v = B_32_63(B_32_63(ACC) - SF(B_32_47(REG1), B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhesmi", ___evmhesmi___)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
}

X("evmhesmia", ___evmhesmia___)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhesmiaaw", ___evmhesmiaaw___)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    u = B_0_31(ACC) + u;
    v = B_32_63(ACC) + v;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhesmianw", ___evmhesmianw___)
{
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    u = B_0_31(ACC) - u;
    v = B_32_63(ACC) - v;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhessf", ___evmhessf___)
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

X("evmhessfa", ___evmhessfa___)
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

X("evmhessfaaw", ___evmhessfaaw___)
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

X("evmhessfanw", ___evmhessfanw___)
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

X("evmhessiaaw", ___evmhessiaaw___)
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

X("evmhessianw", ___evmhessianw___)
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

X("evmheumi", ___evmheumi___)
{
    uint32_t u, v;

    u  = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    v  = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));

    REG0 = (U64(u) << 32) | U64(v);
}

X("evmheumia", ___evmheumia___)
{
    uint32_t u, v;

    u  = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    v  = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmheumiaaw", ___evmheumiaaw___)
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

X("evmheumianw", ___evmheumianw___)
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

X("evmheusiaaw", ___evmheusiaaw___)
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

X("evmheusianw", ___evmheusianw___)
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


X("evmhogsmfaa", ___evmhogsmfaa___)
{
    uint64_t tmp64;
    tmp64 = GSF(B_48_63(REG1), B_48_63(REG2));
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X("evmhogsmfan", ___evmhogsmfan___)
{
    uint64_t tmp64;
    tmp64 = GSF(B_48_63(REG1), B_48_63(REG2));
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X("evmhogsmiaa", ___evmhogsmiaa___)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(tmp);
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X("evmhogsmian", ___evmhogsmian___)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(tmp);
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X("evmhogumiaa", ___evmhogumiaa___)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(tmp);
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X("evmhogumian", ___evmhogumian___)
{
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(tmp);
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X("evmhosmf", ___evmhosmf___)
{
    uint32_t u, v;
    u    = SF(B_16_31(REG1), B_16_31(REG2));
    v    = SF(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
}

X("evmhosmfa", ___evmhosmfa___)
{
    uint32_t u, v;
    u    = SF(B_16_31(REG1), B_16_31(REG2));
    v    = SF(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhosmfaaw", ___evmhosmfaaw___)
{
    uint32_t tmp, u, v;

    tmp = SF(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhosmfanw", ___evmhosmfanw___)
{
    uint32_t tmp, u, v;

    tmp = SF(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhosmi", ___evmhosmi___)
{
    uint32_t u, v;
    u    = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
}

X("evmhosmia", ___evmhosmia___)
{
    uint32_t u, v;
    u    = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhosmiaaw", ___evmhosmiaaw___)
{
    uint32_t tmp, u, v;

    tmp = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhosmianw", ___evmhosmianw___)
{
    uint32_t tmp, u, v;

    tmp = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhossf", ___evmhossf___)
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

X("evmhossfa", ___evmhossfa___)
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

X("evmhossfaaw", ___evmhossfaaw___)
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

X("evmhossfanw", ___evmhossfanw___)
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

X("evmhossiaaw", ___evmhossiaaw___)
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

X("evmhossianw", ___evmhossianw___)
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

X("evmhoumi", ___evmhoumi___)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
}

X("evmhoumia", ___evmhoumia___)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhoumiaaw", ___evmhoumiaaw___)
{
    uint32_t tmp, u, v;

    tmp  = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    u    = B_0_31(ACC) + tmp;

    tmp  = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    v    = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhoumianw", ___evmhoumianw___)
{
    uint32_t tmp, u, v;

    tmp  = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    u    = B_0_31(ACC) - tmp;

    tmp  = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    v    = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmhousiaaw", ___evmhousiaaw___)
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

X("evmhousianw", ___evmhousianw___)
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

X("evmra", ___evmra___)
{
    ACC  = REG1;
    REG0 = REG1;
}

X("evmwhsmf", ___evmwhsmf___)
{
    uint32_t u, v;

    u = B_0_31(SF(B_0_31(REG1), B_0_31(REG2)));
    v = B_0_31(SF(B_32_63(REG1), B_32_63(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
}

X("evmwhsmfa", ___evmwhsmfa___)
{
    uint32_t u, v;

    u = B_0_31(SF(B_0_31(REG1), B_0_31(REG2)));
    v = B_0_31(SF(B_32_63(REG1), B_32_63(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmwhssf", ___evmwhssf___)
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

X("evmwhssfa", ___evmwhssfa___)
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

X("evmwhumi", ___evmwhumi___)
{
    uint32_t u, v;
    u    = X86_MULUW_H(B_0_31(REG1), B_0_31(REG2));      // get high word
    v    = X86_MULUW_H(B_32_63(REG1), B_32_63(REG2));    // get high word
    REG0 = (U64(u) << 32) | U64(v);
}

X("evmwhumia", ___evmwhumia___)
{
    uint32_t u, v;
    u    = X86_MULUW_H(B_0_31(REG1), B_0_31(REG2));      // get high word
    v    = X86_MULUW_H(B_32_63(REG1), B_32_63(REG2));    // get high word
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmwlsmiaaw", ___evmwlsmiaaw___)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  + X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) + X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmwlsmianw", ___evmwlsmianw___)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) - X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
}

X("evmwlssiaaw", ___evmwlssiaaw___)
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

X("evmwlssianw", ___evmwlssianw___)
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

X("evmwlumi", ___evmwlumi___)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_0_31(REG1), B_0_31(REG2));
    v    = X86_MULUW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
}

X("evmwlumia", ___evmwlumia___)
{
    uint32_t u, v;
    u    = X86_MULUW_L(B_0_31(REG1), B_0_31(REG2));
    v    = X86_MULUW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evmwlumiaaw", ___evmwlumiaaw___)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  + X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) + X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evmwlumianw", ___evmwlumianw___)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) - X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evmwlusiaaw", ___evmwlusiaaw___)
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

X("evmwlusianw", ___evmwlusianw___)
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

X("evmwsmf", ___evmwsmf___)
{
    REG0 = SF(B_32_63(REG1), B_32_63(REG2));
}

X("evmwsmfa", ___evmwsmfa___)
{
    REG0 = SF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evmwsmfaa", ___evmwsmfaa___)
{
    uint64_t tmp64;
    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    REG0  = ACC + tmp64;
    ACC   = REG0;
}

X("evmwsmfan", ___evmwsmfan___)
{
    uint64_t tmp64;
    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    REG0  = ACC - tmp64;
    ACC   = REG0;
}

X("evmwsmi", ___evmwsmi___)
{
    REG0 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
}

X("evmwsmia", ___evmwsmia___)
{
    REG0 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evmwsmiaa", ___evmwsmiaa___)
{
    REG0 = ACC + X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evmwsmian", ___evmwsmian___)
{
    REG0 = ACC - X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evmwssf", ___evmwssf___)
{
    uint64_t tmp64;
    bool mov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { REG0 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { REG0 = tmp64; mov = 0;                 }

    UPDATE_SPEFSCR_OV(0, mov);
}

X("evmwssfa", ___evmwssfa___)
{
    uint64_t tmp64;
    bool mov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { REG0 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { REG0 = tmp64; mov = 0;                 }
    ACC = REG0;

    UPDATE_SPEFSCR_OV(0, mov);
}

X("evmwssfaa", ___evmwssfaa___)
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

X("evmwssfan", ___evmwssfan___)
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

X("evmwumi", ___evmwumi___)
{
    REG0 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
}

X("evmwumia", ___evmwumia___)
{
    REG0 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evmwumiaa", ___evmwumiaa___)
{
    REG0 = ACC + X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evmwumian", ___evmwumian___)
{
    REG0 = ACC - X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
}

X("evnand", ___evnand___)
{
    REG0 = ~(REG1 & REG2);
}

X("evneg", ___evneg___)
{
    REG0 = PACK_2W(X86_NEGW(B_0_31(REG1)), X86_NEGW(B_32_63(REG1)));
}

X("evnor", ___evnor___)
{
    REG0 = ~(REG1 | REG2);
}

X("evor", ___evor___)
{
    REG0 = REG1 | REG2;
}

X("evorc", ___evorc___)
{
    REG0 = REG1 | ~REG2;
}

X("evrlw", ___evrlw___)
{
    int nh, nl;
    uint32_t u, v;

    nh   = B_27_31(REG2);
    nl   = B_59_63(REG2);
    u    = ROTL32(B_0_31(REG1), nh);
    v    = ROTL32(B_32_63(REG1), nl);
    REG0 = PACK_2W(u, v);
}

X("evrlwi", ___evrlwi___)
{
    uint32_t u, v;
    int n = ARG2;
    u     = ROTL32(B_0_31(REG1), n);
    v     = ROTL32(B_32_63(REG1), n);
    REG0  = PACK_2W(u, v);
}

X("evrndw", ___evrndw___)
{
    uint32_t u, v;
    u    = (B_0_31(REG1)  + 0x8000) & 0xffff0000;
    v    = (B_32_63(REG1) + 0x8000) & 0xffff0000;
    REG0 = PACK_2W(u, v);
}

X("evsel", ___evsel___)
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


X("evslw", ___evslw___)
{
    int nh, nl;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    REG0 = PACK_2W(SL(B_0_31(REG1), nh), SL(B_32_63(REG1), nl));
}

X("evslwi", ___evslwi___)
{
    int n = ARG2;
    REG0  = PACK_2W(SL(B_0_31(REG1), n), SL(B_32_63(REG2), n));
}

X("evsplatfi", ___evsplatfi___)
{
    uint32_t u;
    u    = ((ARG1 & 0x1f) << 27);
    REG0 = PACK_2W(u, u);
}

X("evsplati", ___evsplati___)
{
    uint32_t u = EXTS_5B_2_32B(ARG1);
    REG0       = PACK_2W(u, u);
}

X("evsrwis", ___evsrwis___)
{
    int n = ARG2;
    uint32_t u, v;

    u     = U32(EXTS_BF2D(REG0, 0, 31-n));
    v     = U32(EXTS_BF2D(REG0, 32, 63-n));
    REG0  = PACK_2W(u, v);
}

X("evsrwiu", ___evsrwiu___)
{
    int n = ARG2;
    uint32_t u, v;

    u    = U32(EXTZ_BF2D(REG0, 0, 31-n));
    v    = U32(EXTZ_BF2D(REG0, 32, 63-n));
    REG0 = PACK_2W(u, v);
}

X("evsrws", ___evsrws___)
{
    int nh, nl;
    uint32_t u, v;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    u    = U32(EXTS_BF2D((REG1), 0, 31-nh));
    v    = U32(EXTS_BF2D((REG1), 32, 63-nl));
    REG0 = PACK_2W(u, v);
}

X("evsrwu", ___evsrwu___)
{
    int nh, nl;
    uint32_t u, v;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    u    = U32(EXTZ_BF2D((REG1), 0, 31-nh));
    v    = U32(EXTZ_BF2D((REG1), 32, 63-nl));
    REG0 = PACK_2W(u, v);
}

X("evstdd", ___evstdd___)
{
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    REG0 = LOAD64(ea);
}

X("evstddx", ___evstddx___)
{
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    REG0 = LOAD64(ea);
}

X("evstdh", ___evstdh___)
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

X("evstdhx", ___evstdhx___)
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

X("evstdw", ___evstdw___)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    v    = LOAD32(ea+4);
    REG0 = PACK_2W(u, v);
}

X("evstdwx", ___evstdwx___)
{
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    v    = LOAD32(ea+4);
    REG0 = PACK_2W(u, v);
}

X("evstwhe", ___evstwhe___)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 48) | (U64(v) << 16);
}

X("evstwhex", ___evstwhex___)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 48) | (U64(v) << 16);
}

X("evstwho", ___evstwho___)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 32) | U64(v);
}

X("evstwhox", ___evstwhox___)
{
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 32) | U64(v);
}

X("evstwwe", ___evstwwe___)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    REG0 = (U64(u) << 32);
}

X("evstwwex", ___evstwwex___)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    REG0 = (U64(u) << 32);
}

X("evstwwo", ___evstwwo___)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    REG0 = U64(u);
}

X("evstwwox", ___evstwwox___)
{
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    REG0 = U64(u);
}

X("evsubfsmiaaw", ___evsubfsmiaaw___)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - B_0_31(REG1);
    v    = B_32_63(ACC) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evsubfssiaaw", ___evsubfssiaaw___)
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
X("evsubfumiaaw", ___evsubfumiaaw___)
{
    uint32_t u, v;
    u    = B_0_31(ACC)  - B_0_31(REG1);
    v    = B_32_63(ACC) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
}

X("evsubfw", ___evsubfw___)
{
    uint32_t u, v;
    u    = B_0_31(REG2)  - B_0_31(REG1);
    v    = B_32_63(REG2) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
}

X("evsubfusiaaw", ___evsubfusiaaw___)
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

X("evsubifw", ___evsubifw___)
{
    uint32_t u, v;
    u    = B_0_31(REG2)  - (ARG1 & 0x1f);
    v    = B_32_63(REG2) - (ARG1 & 0x1f);
    REG0 = PACK_2W(u, v);
}

X("evxor", ___evxor___)
{
    REG0 = REG1 ^ REG2;
}

// ----------------- SPE FP ---------------------------------------------------------------------------

X("efdabs", ___efdabs___)
{
    REG0 = REG1 & 0x7fffffffffffffffULL;       // Change sign bit to zero
}
X("efdnabs", ___efdnabs___)
{
    REG0 = (REG1 & 0x7fffffffffffffffULL) | 0x8000000000000000ULL;
}
X("efdneg", ___efdneg___)
{
    REG0 = (REG1 & 0x7fffffffffffffffULL) | ((REG1 ^ 0x8000000000000000ULL) & 0x8000000000000000ULL);
}

