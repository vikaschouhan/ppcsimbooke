/*
 * ------------------------------------------------------------------------------------
 *  PowerPC BookE RTL description.
 * ------------------------------------------------------------------------------------
 *
 *  Author : Vikas Chouhan (presentisgood@gmail.com)
 *  Copyright (C) 2012.
 *  
 *  e500v2 is taken as a reference, but the code is generic and can be used for higher
 *  booke 64 bit cores.
 *
 *  NOTE:
 *  This file is a regular C/C++ file (although it doesn't look like it :)).
 *  Technically it's a form of template for defining instructions'
 *  RTL(register transfer logic) implementation, trying to be very very generic
 *  by making heavy use of preprocesor macros.
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
//
// TERMINOLOGIES :
//       1. byte means 8 bits.
//       2. half-word means 16 bits.
//       3. word means 32 bits.
//       4. double/double-word means 64 bits.
//

#ifndef __CPU_PPC_INSTR_RTL
#define __CPU_PPC_INSTR_RTL

// Alias to lambada function parameters ------------------------------------------------------
// Global defines for this template file
#define CPU                      pcpu
#define IC                       ic
// specific to e500v2 (e500v2 is a 32 bit core hence, native type is 32bits only)
#define UMODE                    uint32_t
#define SMODE                    int32_t

// Alias to CPU functions ----------------------------------------------------------------------
#pragma push_macro("PPCREG")
#undef  PPCREG
#define PPCREG(regid)            (CPU->reg(regid)->value.u64v)

#pragma push_macro("DREG")
#undef  DREG
#define DREG(reg_alias)          CPU->m_cpu_regs.reg_alias.value.u64v

#pragma push_macro("DREG_FN")
#undef  DREG_FN
#define DREG_FN(reg_fn)          CPU->m_cpu_regs.reg_fn

// Alias to Registers --------------------------------------------------------------------------
// MSR bits
#pragma push_macro("MSR_CM")
#undef  MSR_CM
#define MSR_CM                   ((DREG(msr) & 0x80000000) ? 1 : 0)

#pragma push_macro("MSR_UCLE")
#undef  MSR_UCLE
#define MSR_UCLE                 ((DREG(msr) & 0x4000000) ? 1:0)

#pragma push_macro("MSR_PR")
#undef  MSR_PR
#define MSR_PR                   ((DREG(msr) & 0x4000) ? 1:0)

#pragma push_macro("GPR")
#undef  GPR
#define GPR(gprno)               DREG(gpr[gprno])

#pragma push_macro("SPR")
#undef  SPR
#define SPR(sprno)               PPCREG(REG_SPR0 + sprno)

#pragma push_macro("XER")
#undef  XER
#define XER                      DREG(xer)

#pragma push_macro("MSR")
#undef  MSR
#define MSR                      DREG(msr)

#pragma push_macro("ACC")
#undef  ACC
#define ACC                      DREG(acc)

#pragma push_macro("SPEFSCR")
#undef  SPEFSCR
#define SPEFSCR                  DREG(spefscr)

#pragma push_macro("PMR")
#undef  PMR
#define PMR(pmrno)               PPCREG(REG_PMR0 + pmrno)

#pragma push_macro("CR")
#undef  CR
#define CR                       DREG(cr)

#pragma push_macro("LR")
#undef  LR
#define LR                       DREG(lr)

#pragma push_macro("CTR")
#undef  CTR
#define CTR                      DREG(ctr)

#pragma push_macro("SRR0")
#undef  SRR0
#define SRR0                     DREG(srr0)

#pragma push_macro("SRR1")
#undef  SRR1
#define SRR1                     DREG(srr1)

#pragma push_macro("CSRR0")
#undef  CSRR0
#define CSRR0                    DREG(csrr0)

#pragma push_macro("CSRR1")
#undef  CSRR1
#define CSRR1                    DREG(csrr1)

#pragma push_macro("MCSRR0")
#undef  MCSRR0
#define MCSRR0                   DREG(mcsrr0)

#pragma push_macro("MCSRR1")
#undef  MCSRR1
#define MCSRR1                   DREG(mcsrr1)

#pragma push_macro("PC")
#undef  PC
#define PC                       CPU->m_cpu_regs.pc

#pragma push_macro("NIP")
#undef  NIP
#define NIP                      CPU->m_cpu_regs.nip

#pragma push_macro("HOST_FLAGS")
#undef  HOST_FLAGS
#define HOST_FLAGS               CPU->host_flags

#pragma push_macro("HOST_FPU_FLAGS")
#undef  HOST_FPU_FLAGS
#define HOST_FPU_FLAGS           CPU->m_x86_mxcsr

#pragma push_macro("CACHE_LINE_SIZE")
#undef  CACHE_LINE_SIZE
#define CACHE_LINE_SIZE          CPU->m_cache_line_size

// Alias to some cpu functions ----------------------------------------------------------------

#define get_crf                  DREG_FN(get_crf)
#define get_crF                  DREG_FN(get_crF)
#define update_crf               DREG_FN(update_crf)
#define update_crF               DREG_FN(update_crF)
#define get_xerf                 DREG_FN(get_xerf)
#define get_xerF                 DREG_FN(get_xerF)
#define update_xerf              DREG_FN(update_xerf)
#define update_xerF              DREG_FN(update_xerF)

// Host flags
#define GET_CF_H()               CPU->host_flags.cf       // x86 carry flag
#define GET_OF_H()               CPU->host_flags.of       // x86 overflow flag

// Generic macros
#define UPDATE_CA()              DREG_FN(update_xer_ca_host(HOST_FLAGS))
#define UPDATE_CA_V              DREG_FN(update_xer_ca)
#define UPDATE_SO_OV()           DREG_FN(update_xer_so_ov_host(HOST_FLAGS))
#define UPDATE_SO_OV_V           DREG_FN(update_xer_so_ov)
#define UPDATE_CR0()             DREG_FN(update_cr0_host(HOST_FLAGS))
#define GET_CA()                 DREG_FN(get_xer_ca())
#define GET_SO()                 DREG_FN(get_xer_so())
#define UPDATE_CR0_V             DREG_FN(update_cr0)

#define UPDATE_SPEFSCR_H(high)   DREG_FN(update_spefscr(HOST_FPU_FLAGS, high))

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
#define TLBWE()                  CPU->m_l2tlb.tlbwe(DREG(mas0), DREG(mas1), DREG(mas2), DREG(mas3), DREG(mas7), DREG(hid0))
#define TLBRE()                  CPU->m_l2tlb.tlbre(DREG(mas0), DREG(mas1), DREG(mas2), DREG(mas3), DREG(mas7), DREG(hid0))
#define TLBIVAX(ea)              CPU->m_l2tlb.tlbive(ea)
#define TLBSX(ea)                CPU->m_l2tlb.tlbse(ea, DREG(mas0), DREG(mas1), DREG(mas2), DREG(mas3), DREG(mas6), DREG(mas7), DREG(hid0))

// In case of register operands, their pointers are instead passed in corresponding ARG parameter.
#define ARG_BASE                 IC->arg

#define ARG0                     (ARG_BASE[0].v)
#define ARG1                     (ARG_BASE[1].v)
#define ARG2                     (ARG_BASE[2].v)
#define ARG3                     (ARG_BASE[3].v)
#define ARG4                     (ARG_BASE[4].v)

#define REG0                     CPU->m_cpu_regs.gpr[ARG0].value.u64v
#define REG1                     CPU->m_cpu_regs.gpr[ARG1].value.u64v
#define REG2                     CPU->m_cpu_regs.gpr[ARG2].value.u64v
#define REG3                     CPU->m_cpu_regs.gpr[ARG3].value.u64v
#define REG4                     CPU->m_cpu_regs.gpr[ARG4].value.u64v


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
#define X86_ADDW(arg1,    arg2)                       x86_add<int32_t>(arg1, arg2, HOST_FLAGS)
#define X86_ANDW(arg1,    arg2)                       x86_and<uint32_t>(arg1,arg2, HOST_FLAGS)
#define X86_ORW(arg1,     arg2)                       x86_or<uint32_t>(arg1, arg2, HOST_FLAGS)
#define X86_XORW(arg1,    arg2)                       x86_xor<uint32_t>(arg1, arg2, HOST_FLAGS)
#define X86_NEGW(arg1         )                       x86_neg<uint32_t>(arg1, HOST_FLAGS)
#define X86_DIVW(arg1,    arg2)                       x86_idiv<int32_t>(arg1, arg2)
#define X86_DIVUW(arg1,   arg2)                       x86_div<uint32_t>(arg1, arg2)
#define X86_MULW_L(arg1,  arg2)                       x86_imul<int32_t>(arg1, arg2, HOST_FLAGS, 0)
#define X86_MULW_H(arg1,  arg2)                       x86_imul<int32_t>(arg1, arg2, HOST_FLAGS, 1)
#define X86_MULUW_L(arg1, arg2)                       x86_mul<uint32_t>(arg1, arg2, HOST_FLAGS, 0)
#define X86_MULUW_H(arg1, arg2)                       x86_mul<uint32_t>(arg1, arg2, HOST_FLAGS, 1)
#define X86_MULWF(arg1,   arg2)                       x86_imulf<int64_t, int32_t>(arg1, arg2, HOST_FLAGS)
#define X86_MULUWF(arg1,  arg2)                       x86_mulf<uint64_t, uint32_t>(arg1, arg2, HOST_FLAGS)

#define X86_ADD64(arg1,   arg2)                      x86_add<int64_t>(arg1,  arg2, HOST_FLAGS)           // 64bit ADD
#define X86_SUB64(arg1,   arg2)                      x86_sub<int64_t>(arg1,  arg2, HOST_FLAGS)           // 64bit SUB

#define X86_ADDS_F32(arg1, arg2)                     x86_adds_f32(arg1, arg2, HOST_FPU_FLAGS)
#define X86_SUBS_F32(arg1, arg2)                     x86_subs_f32(arg1, arg2, HOST_FPU_FLAGS)
#define X86_MULS_F32(arg1, arg2)                     x86_muls_f32(arg1, arg2, HOST_FPU_FLAGS)
#define X86_DIVS_F32(arg1, arg2)                     x86_divs_f32(arg1, arg2, HOST_FPU_FLAGS)

// define building blocks for lambda syntax (GCC compiler must support -std=c++11)
// Incrementing NIP actually takes care of program flow.
// NOTE : All branch instructions calculate NIP (using the current PC for relative jumps).
//        NIP is assigned to PC at the end.So effectively NIP is always 4 byte ahead of PC,
//        except when a branch is taken.
#define RTL_BEGIN(opc_name, func_name)     CPU->m_ppc_func_hash[CPU->m_dis.get_opc_hash(opc_name)] =  \
                                               [](ppcsimbooke::ppcsimbooke_cpu::cpu *CPU, ppcsimbooke::instr_call *IC) \
                                               -> void { NIP += 4;
#define RTL_END                            PC = NIP; };


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

RTL_BEGIN("add", ___add___)
#define add_code(rD, rA, rB)           \
    rD = X86_ADDW(rA, rB);

    add_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("add.", ___add_dot___)
    add_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END
RTL_BEGIN("addo", ___addo___)
    add_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("addo.", ___addo_dot___)
    add_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

// These instrs always update XER[CA] flag
RTL_BEGIN("addc", ___addc___)
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
RTL_END
RTL_BEGIN("addc.", ___addc_dot___)
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_CR0();
RTL_END
RTL_BEGIN("addco", ___addco___)
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("addco.", ___addco_dot___)
    add_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

/* Add extended : rA + rB + CA */
RTL_BEGIN("adde", ___adde___)
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
RTL_END
RTL_BEGIN("adde.", ___adde_dot___)
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
RTL_END
RTL_BEGIN("addeo", ___addeo___)
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("addeo.", ___addeo_dot___)
    UMODE tmp = REG2 + GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("addi", ___addi___)
    SMODE tmp = (int16_t)ARG2;
    if(ARG1){ add_code(REG0, REG1, tmp);        }
    else    { REG0 = (int16_t)ARG2;             }
RTL_END

RTL_BEGIN("addic", ___addic___)
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
RTL_END
RTL_BEGIN("addic.", ___addic_dot___)
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("addis", ___addis___)
    SMODE tmp = (((int16_t)ARG2) << 16);
    if(ARG1){ add_code(REG0, REG1, tmp);        }
    else    { REG0 = ((int16_t)ARG2) << 16;     }
RTL_END

// XER[CA] is always altered
RTL_BEGIN("addme", ___addme___)
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
RTL_END
RTL_BEGIN("addme.", ___addme_dot___)
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
RTL_END
RTL_BEGIN("addmeo", ___addmeo___)
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("addmeo.", ___addmeo_dot___)
    SMODE tmp = GET_CA() - 1;
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

// XER[CA] is always altered
RTL_BEGIN("addze", ___addze___)
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
RTL_END
RTL_BEGIN("addze.", ___addze_dot___)
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_CR0();
RTL_END
RTL_BEGIN("addzeo", ___addzeo___)
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("addzeo.", ___addzeo_dot___)
    UMODE tmp = GET_CA();
    add_code(REG0, REG1, tmp);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("subf", ___subf___)
#define subf_code(rD, rA, rB)            \
    uint64_t subf_tmp = rB + 1;          \
    uint64_t inva = ~rA;                 \
    add_code(rD, inva, subf_tmp)

    subf_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("subf.", ___subf_dot___)
    subf_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END
RTL_BEGIN("subfo", ___subfo___)
    subf_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("subfo.", ___subfo_dot___)
    subf_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

// XER[CA] is altered
RTL_BEGIN("subfic", ___subfic___)
    uint64_t tmp = EXTS_H2N(ARG2);
    subf_code(REG0, REG1, tmp);
    UPDATE_CA();
RTL_END

// XER[CA] is always altered
RTL_BEGIN("subfc", ___subfc___)
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
RTL_END
RTL_BEGIN("subfc.", ___subfc_dot___)
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_CR0();
RTL_END
RTL_BEGIN("subfco", ___subfco___)
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("subfco.", ___subfco_dot___)
    subf_code(REG0, REG1, REG2);
    UPDATE_CA();
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("subfe", ___subfe___)
#define subfe_code(rD, rA, rB)          \
    UMODE tmp = GET_CA() + rB;          \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfe_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("subfe.", ___subfe_dot___)
    subfe_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END
RTL_BEGIN("subfeo", ___subfeo___)
    subfe_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("subfeo.", ___subfeo_dot___)
    subfe_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("subfme", ___subfme___)
#define subfme_code(rD, rA)             \
    SMODE tmp = GET_CA() - 1;           \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfme_code(REG0, REG1);
RTL_END
RTL_BEGIN("subfme.", ___subfme_dot___)
    subfme_code(REG0, REG1);
    UPDATE_CR0();
RTL_END
RTL_BEGIN("subfmeo", ___subfmeo___)
    subfme_code(REG0, REG1);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("subfmeo.", ___subfmeo_dot___)
    subfme_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("subfze", ___subfze___)
#define subfze_code(rD, rA)             \
    UMODE tmp = GET_CA();               \
    UMODE inva = ~rA;                   \
    add_code(rD, inva, tmp);            \
    UPDATE_CA()

    subfze_code(REG0, REG1);
RTL_END
RTL_BEGIN("subfze.", ___subfze_dot___)
    subfze_code(REG0, REG1);
    UPDATE_CR0();
RTL_END
RTL_BEGIN("subfzeo", ___subfzeo___)
    subfze_code(REG0, REG1);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("subfzeo.", ___subfzeo_dot___)
    subfze_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

RTL_BEGIN("divw", ___divw___)
#define divw_code(rD, rA, rB)          \
    if(rB){ rD = X86_DIVW(rA, rB); }

    divw_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("divw.", ___divw_dot___)
    divw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END
RTL_BEGIN("divwo", ___divwo___)
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
RTL_END
RTL_BEGIN("divwo.", ___divwo_dot___)
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    UPDATE_CR0_V(REG0);
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
RTL_END

RTL_BEGIN("divwu", ___divwu___)
#define divwu_code(rD, rA, rB)         \
    if(rB){ rD = X86_DIVUW(rA, rB); }

    divwu_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("divwu.", ___divwu_dot___)
    divwu_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END
RTL_BEGIN("divwuo", ___divwuo___)
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }   // Set OV=1 if division by zero
RTL_END
RTL_BEGIN("divwuo.", ___divwuo_dot___)
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV_V(0);
    UPDATE_CR0_V(REG0);
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }
RTL_END

RTL_BEGIN("mulhw", ___mulhw___)
#define mulhw_code(rD, rA, rB)            \
    rD = X86_MULW_H(rA, rB)

    mulhw_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("mulhw.", ___mulhw_dot___)
    mulhw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END

RTL_BEGIN("mulli", ___mulli___)
    uint32_t imm_val = EXTS_H2W(ARG2);          // Sign extend 16 bit IMM to 32 bit
    REG0 = X86_MULW_L(REG1, imm_val);
RTL_END

RTL_BEGIN("mulhwu", ___mulhwu___)
#define mulhwu_code(rD, rA, rB)           \
    rD = X86_MULUW_H(rA, rB)

    mulhwu_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("mulhwu.", ___mulhwu_dot___)
    mulhwu_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);                 // muluw doesn't set SF or ZF, hence we have to set CR0 by value
RTL_END

RTL_BEGIN("mullw", ___mullw___)
#define mullw_code(rD, rA, rB)            \
    rD = X86_MULW_L(rA, rB)

    mullw_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("mullw.", ___mullw_dot___)
    mullw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END
RTL_BEGIN("mullwo", ___mullwo___)
    mullw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("mullwo.", ___mullwo_dot___)
    mullw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0_V(REG0);
RTL_END


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
RTL_BEGIN("and", ___and___)
#define and_code(rA, rS, rB)             \
    rA = X86_ANDW(rS, rB)

    and_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("and.", ___and_dot___)
    and_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END

RTL_BEGIN("andc", ___andc___)
    UMODE tmp = ~REG2;
    and_code(REG0, REG1, tmp);
RTL_END
RTL_BEGIN("andc.", ___andc_dot___)
    UMODE tmp = ~REG2;
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
RTL_END

RTL_BEGIN("andi.", ___andi_dot___)
    UMODE tmp = (uint16_t)ARG2;
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
RTL_END

RTL_BEGIN("andis.", ___andis_dot___)
    UMODE tmp = (((uint16_t)ARG2) << 16);
    and_code(REG0, REG1, tmp);
    UPDATE_CR0();
RTL_END

// Byte extend
RTL_BEGIN("extsb", ___extsb___)
    REG0 = EXTS_B2N(REG1);
RTL_END
RTL_BEGIN("extsb.", ___extsb_dot___)
    REG0 = EXTS_B2N(REG1);
    UPDATE_CR0_V(REG0);
RTL_END

// Halfword extend
RTL_BEGIN("extsh", ___extsh___)
    REG0 = EXTS_H2N(REG1);
RTL_END
RTL_BEGIN("extsh.", ___extsh_dot___)
    REG0 = EXTS_H2N(REG1);
    UPDATE_CR0_V(REG0);
RTL_END

// cntlzw:  Count leading zeroes (32-bit word).
RTL_BEGIN("cntlzw", ___cntlzw___)
#define cntlzw_code(rA, rS)                     \
    uint64_t n = 32;                            \
    while(n < 64){                              \
        if(B_N(rS, n))  break;                  \
        n++;                                    \
    }                                           \
    rA = (n - 32)

    cntlzw_code(REG0, REG1);
RTL_END
RTL_BEGIN("cntlzw.", ___cntlzw_dot___)
    cntlzw_code(REG0, REG1);
    UPDATE_CR0_V(REG0);
RTL_END

RTL_BEGIN("eqv", ___eqv___)
#define eqv_code(rD, rA, rB)                   \
    rD = ~(rA ^ rB)

    eqv_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("eqv.", ___eqv_dot___)
    eqv_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END

RTL_BEGIN("nand", ___nand___)
#define nand_code(rA, rS, rB)                  \
    rA = ~(rS & rB)

    nand_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("nand.", ___nand_dot___)
    nand_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END
RTL_BEGIN("neg", ___neg___)
#define neg_code(rD, rA)                        \
    rD = X86_NEGW(rA);

    neg_code(REG0, REG1);
RTL_END
RTL_BEGIN("neg.", ___neg_dot___)
    neg_code(REG0, REG1);
    UPDATE_CR0();
RTL_END
RTL_BEGIN("nego", ___nego___)
    neg_code(REG0, REG1);
    UPDATE_SO_OV();
RTL_END
RTL_BEGIN("nego.", ___nego_dot___)
    neg_code(REG0, REG1);
    UPDATE_SO_OV();
    UPDATE_CR0();
RTL_END

//
RTL_BEGIN("nor", ___nor___)
#define nor_code(rA, rS, rB)                    \
    rA = ~(rS | rB)

    nor_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("nor.", ___nor_dot___)
    nor_code(REG0, REG1, REG2);
    UPDATE_CR0_V(REG0);
RTL_END

RTL_BEGIN("or", ___or___)
#define or_code(rA, rS, rB)                     \
    rA = X86_ORW(rS, rB)

    or_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("or.", ___or_dot___)
    or_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END

RTL_BEGIN("orc", ___orc___)
#define orc_code(rA, rS, rB)                     \
    UMODE tmp = ~rB;                             \
    rA = X86_ORW(rS, tmp)

    orc_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("orc.", ___orc_dot___)
    orc_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END

RTL_BEGIN("ori", ___ori___)
    UMODE tmp = (uint16_t)(ARG2);
    or_code(REG0, REG1, tmp);
RTL_END

RTL_BEGIN("oris", ___oris___)
    UMODE tmp = (((uint16_t)ARG2) << 16);
    or_code(REG0, REG1, tmp);
RTL_END

// xor variants
RTL_BEGIN("xor", ___xor___)
#define xor_code(rA, rS, rB)      \
    rA = X86_XORW(rS, rB)

    xor_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("xor.", ___xor_dot___)
    xor_code(REG0, REG1, REG2);
    UPDATE_CR0();
RTL_END

RTL_BEGIN("xori", ___xori___)
    UMODE tmp = ((uint16_t)ARG2);
    xor_code(REG0, REG1, tmp);
RTL_END

RTL_BEGIN("xoris", ___xoris___)
    UMODE tmp = (((uint16_t)ARG2) << 16);
    xor_code(REG0, REG1, tmp);
RTL_END

// START
// ------------------------------- BPU ---------------------------------------------
// BTB instrs
RTL_BEGIN("bbelr", ___bbelr___)
    // Not implemented
RTL_END
RTL_BEGIN("bblels", ___bblels___)
    // Not implemented
RTL_END


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
RTL_BEGIN("b", ___b___)
#define b_code(tgtaddr)      \
    NIP = tgtaddr

    b_code(ARG0);
RTL_END
RTL_BEGIN("ba", ___ba___)
    b_code(ARG0);
RTL_END
RTL_BEGIN("bl", ___bl___)
    b_code(ARG0);
    LR = (PC + 4);
RTL_END
RTL_BEGIN("bla", ___bla___)
    b_code(ARG0);
    LR = (PC + 4);
RTL_END

// branch conditional
RTL_BEGIN("bc", ___bc___)
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
RTL_END
RTL_BEGIN("bca", ___bca___)
    bc_code(ARG0, ARG1, ARG2)
RTL_END
RTL_BEGIN("bcl", ___bcl___)
#define bcl_code(BO, BI, tgtaddr)                   \
    bc_code(BO, BI, tgtaddr);                       \
    LR = (PC + 4);

    bcl_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("bcla", ___bcla___)
    bcl_code(ARG0, ARG1, ARG2);
RTL_END

// branch conditional to LR
RTL_BEGIN("bclr", ___bclr___)
#define bclr_code(BO, BI, BH)                                                       \
    if(!BO2(BO)) CTR = CTR - 1;                                                     \
    int ctr_ok = BO2(BO) | ((((MSR_CM) ? CTR: (CTR & 0xffffffff)) != 0) ^ BO3(BO)); \
    int cond_ok = BO0(BO) | (get_crf(BI) == BO1(BO));                               \
    if(ctr_ok & cond_ok) NIP = LR & ~0x3;

    bclr_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("bclrl", ___bclrl___)
#define bclrl_code(BO, BI, BH)                       \
    bclr_code(BO, BI, BH);                           \
    LR = (PC + 4);

    bclrl_code(ARG0, ARG1, ARG2);
RTL_END

// branch conditional to CTR
RTL_BEGIN("bcctr", ___bcctr___)
#define bcctr_code(BO, BI, BH)                                                       \
    if(BO0(BO) | (get_crf(BI) == BO1(BO))) NIP = CTR & ~0x3;
    
    bcctr_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("bcctrl", ___bcctrl___)
#define bcctrl_code(BO, BI, BH)                \
    bcctr_code(BO, BI, BH);                    \
    LR = (PC + 4);

    bcctrl_code(ARG0, ARG1, ARG2);
RTL_END


// START
// ------------------------------------- INTEGER COMPARE ------------------------------------
// mnemonics :
//               cmp
//               cmpi
//               cmpl
//               cmpli
//

// cmp crD, L, rA, rB 
RTL_BEGIN("cmp", ___cmp___)
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
RTL_END
RTL_BEGIN("cmpi", ___cmpi___)
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
RTL_END
RTL_BEGIN("cmpl", ___cmpl___)
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
RTL_END
RTL_BEGIN("cmpli", ___cmpli___)
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
RTL_END


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

RTL_BEGIN("crand", ___crand___)
#define crand_code(crD, crA, crB)                       \
    update_crf(crD, get_crf(crA) & get_crf(crB));

    crand_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("crandc", ___crandc___)
#define crandc_code(crD, crA, crB)                      \
    update_crf(crD, get_crf(crA) & ~(get_crf(crB)));

    crandc_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("creqv", ___creqv___)
#define creqv_code(crD, crA, crB)                      \
    update_crf(crD, ~(get_crf(crA) ^ get_crf(crB)));

    creqv_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("crnand", ___crnand___)
#define crnand_code(crD, crA, crB)                      \
    update_crf(crD, ~(get_crf(crA) & get_crf(crB)));

    crnand_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("crnor", ___crnor___)
#define crnor_code(crD, crA, crB)                       \
    update_crf(crD, ~(get_crf(crA) | get_crf(crB)));

    crnor_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("cror", ___cror___)
#define cror_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) | get_crf(crB)));

    cror_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("crorc", ___crorc___)
#define crorc_code(crD, crA, crB)                       \
    update_crf(crD, (get_crf(crA) | ~(get_crf(crB))));

    crorc_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("crxor", ___crxor___)
#define crxor_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) ^ get_crf(crB)));

    crxor_code(ARG0, ARG1, ARG2);
RTL_END
RTL_BEGIN("mcrf", ___mcrf___)
#define mcrf_code(crfD, crfS)            \
    update_crF(crfD, get_crF(crfS));

    mcrf_code(ARG0, ARG1);
RTL_END


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
RTL_BEGIN("rlwimi", ___rlwimi___)
#define rlwimi_code(rA, rS, SH, MB, ME)                                            \
    uint64_t n = (SH & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB + 32, ME + 32);                                           \
    rA = (r & m) | (rA & ~m)

    rlwimi_code(REG0, REG1, ARG2, ARG3, ARG4);
RTL_END
RTL_BEGIN("rlwimi.", ___rlwimi_dot___)
    rlwimi_code(REG0, REG1, ARG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
RTL_END
RTL_BEGIN("rlwinm", ___rlwinm___)
#define rlwinm_code(rA, rS, SH, MB, ME)                                            \
    uint64_t n = (SH & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB + 32, ME + 32);                                           \
    rA = (r & m)

    rlwinm_code(REG0, REG1, ARG2, ARG3, ARG4);
RTL_END
RTL_BEGIN("rlwinm.", ___rlwinm_dot___)
    rlwinm_code(REG0, REG1, ARG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
RTL_END
RTL_BEGIN("rlwnm", ___rlwnm___)
#define rlwnm_code(rA, rS, rB, MB, ME)                                             \
    uint64_t n = (rB & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m = MASK(MB+32, ME+32);                                               \
    rA = (r & m)

    rlwnm_code(REG0, REG1, REG2, ARG3, ARG4);
RTL_END
RTL_BEGIN("rlwnm.", ___rlwnm_dot___)
    rlwnm_code(REG0, REG1, REG2, ARG3, ARG4);
    UPDATE_CR0_V(UT(REG0));
RTL_END

// Shift instrs
RTL_BEGIN("slw", ___slw___)
#define slw_code(rA, rS, rB)                                                       \
    uint64_t n = (rB & 0x1f);                                                      \
    uint64_t r = ROTL32(B_32_63(rS), n);                                           \
    uint64_t m;                                                                    \
    uint8_t rB_58 = (rB >> 5) & 0x1;                                               \
    if(rB_58 == 0)  m = MASK(32, (63 - n));                                        \
    else            m = 0;                                                         \
    rA = (r & m)

    slw_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("slw.", ___slw_dot___)
    slw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
RTL_END
RTL_BEGIN("sraw", ___sraw___)
#define sraw_code(rA, rS, rB)                                                            \
    uint64_t n = (rB & 0x1f);                                                            \
    uint64_t r = ROTL32(B_32_63(rS), (64 - n));                                          \
    uint64_t m;                                                                          \
    uint8_t rB_58 = (rB >> 5) & 0x1;                                                     \
    if(rB_58 == 0)  m = MASK(n+32, 63);                                                  \
    else            m = 0;                                                               \
    int s = ((rS >> 31) & 0x1);                                                          \
    rA = (r & m) | (((s) ? 0xffffffffffffffffL : 0L) & ~m);                              \
    UPDATE_CA_V(s & (B_32_63(r & ~m) != 0))

    sraw_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("sraw.", ___sraw_dot___)
    sraw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
RTL_END
RTL_BEGIN("srawi", ___srawi___)
#define srawi_code(rA, rS, SH)                                                           \
    uint64_t n = (SH & 0x1f);                                                            \
    uint64_t r = ROTL32(B_32_63(rS), (64 - n));                                          \
    uint64_t m = MASK(n+32, 63);                                                         \
    int s = ((rS >> 31) & 0x1);                                                          \
    rA = (r& m) | (((s) ? 0xffffffffffffffffL : 0L) & ~m);                               \
    UPDATE_CA_V(s & (B_32_63(r & ~m) != 0))

    srawi_code(REG0, REG1, ARG2);
RTL_END
RTL_BEGIN("srawi.", ___srawi_dot___)
    srawi_code(REG0, REG1, ARG2);
    UPDATE_CR0_V(UT(REG0));
RTL_END
RTL_BEGIN("srw", ___srw___)
#define srw_code(rA, rS, rB)                                                             \
    uint64_t n = (rB & 0x1f);                                                            \
    uint64_t r = ROTL32(B_32_63(rS), (64 - n));                                          \
    uint64_t m;                                                                          \
    uint8_t rB_58 = ((rB >> 5) & 0x1);                                                   \
    if ( rB_58 == 0)   m = MASK(n+32, 63);                                               \
    else               m = 0;                                                            \
    rA = (r & m)

    srw_code(REG0, REG1, REG2);
RTL_END
RTL_BEGIN("srw.", ___srw_dot___)
    srw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
RTL_END

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
RTL_BEGIN("lbz", ___lbz___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD8(ea);    
RTL_END
RTL_BEGIN("lbzx", ___lbzx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD8(ea);
RTL_END
RTL_BEGIN("lbzu", ___lbzu___)
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD8(ea);
    REG2 = ea;
RTL_END
RTL_BEGIN("lbzux", ___lbzux___)
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD8(ea);
    REG1 = ea;
RTL_END

// Halfword algebraic loads
RTL_BEGIN("lha", ___lha___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = EXTS_H2N(LOAD16(ea));
RTL_END
RTL_BEGIN("lhax", ___lhax___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = EXTS_H2N(LOAD16(ea));
RTL_END
RTL_BEGIN("lhau", ___lhau___)
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = EXTS_H2N(LOAD16(ea));
    REG2 = ea;
RTL_END
RTL_BEGIN("lhaux", ___lhaux___)
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = EXTS_H2N(LOAD16(ea));
    REG1 = ea;
RTL_END

// Half word loads
RTL_BEGIN("lhz", ___lhz___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD16(ea);
RTL_END
RTL_BEGIN("lhzx", ___lhzx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD16(ea);
RTL_END
RTL_BEGIN("lhzu", ___lhzu___)
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD16(ea);
    REG2 = ea;
RTL_END
RTL_BEGIN("lhzux", ___lhzux___)
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD16(ea);
    REG1 = ea;
RTL_END

// word loads
// lwz rD,D(rA)
RTL_BEGIN("lwz", ___lwz___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    REG0 = LOAD32(ea);
RTL_END
// lwzx rD,rA,rB
RTL_BEGIN("lwzx", ___lwzx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    REG0 = LOAD32(ea);
RTL_END
//  lwzu rD,D(rA)
RTL_BEGIN("lwzu", ___lwzu___)
    if(ARG2 == 0 || ARG0 == ARG2)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    REG0 = LOAD32(ea);
    REG2 = ea;
RTL_END
RTL_BEGIN("lwzux", ___lwzux___)
    if(ARG1 == 0 || ARG0 == ARG1)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    REG0 = LOAD32(ea);
    REG1 = ea;
RTL_END

// Byte reversed indexed loads
RTL_BEGIN("lhbrx", ___lhbrx___)
    UMODE tmp = 0;
    UMODE ea;
    uint16_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = LOAD16(ea);
    REG0 = SWAPB16(data);
RTL_END
RTL_BEGIN("lwbrx", ___lwbrx___)
    UMODE tmp = 0;
    UMODE ea;
    uint32_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = LOAD32(ea);
    REG0 = SWAPB32(data);
RTL_END

// load multiple words
RTL_BEGIN("lmw", ___lmw___)
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
RTL_END

// byte stores
RTL_BEGIN("stb", ___stb___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE8(ea, REG0);
RTL_END
RTL_BEGIN("stbx", ___stbx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE8(ea, REG0);
RTL_END
RTL_BEGIN("stbu", ___stbu___)
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE8(ea, REG0);
    REG2 = ea;
RTL_END
RTL_BEGIN("stbux", ___stbux___)
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE8(ea, REG0);
    REG1 = ea;
RTL_END

// half word stores
RTL_BEGIN("sth", ___sth___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE16(ea, REG0);
RTL_END
RTL_BEGIN("sthx", ___sthx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE16(ea, REG0);
RTL_END
RTL_BEGIN("sthu", ___sthu___)
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE16(ea, REG0);
    REG2 = ea;
RTL_END
RTL_BEGIN("sthux", ___sthux___)
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE16(ea, REG0);
    REG1 = ea;
RTL_END

// word stores
RTL_BEGIN("stw", ___stw___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG2){ tmp = REG2; }
    ea = tmp + EXTS_H2N(ARG1);
    STORE32(ea, REG0);
RTL_END
RTL_BEGIN("stwx", ___stwx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    STORE32(ea, REG0);
RTL_END
RTL_BEGIN("stwu", ___stwu___)
    if(ARG2 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG2 + EXTS_H2N(ARG1);
    STORE32(ea, REG0);
    REG2 = ea;
RTL_END
RTL_BEGIN("stwux", ___stwux___)
    if(ARG1 == 0)
        throw PPC_EXCEPT(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal opcode");
    UMODE ea;
    ea = REG1 + REG2;
    STORE32(ea, REG0);
    REG1 = ea;
RTL_END

// byte reversed stores
RTL_BEGIN("sthbrx", ___sthbrx___)
    UMODE tmp = 0;
    UMODE ea;
    uint16_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = REG0;
    STORE16(ea, SWAPB16(data));
RTL_END
RTL_BEGIN("stwbrx", ___stwbrx___)
    UMODE tmp = 0;
    UMODE ea;
    uint32_t data;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    data = REG0;
    STORE32(ea, SWAPB32(data));
RTL_END

// multiple word store
RTL_BEGIN("stmw", ___stmw___)
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
RTL_END



// START
// ------------------------------ TLB ----------------------------------------------
// mnemonics :
//              tlbivax
//              tlbre
//              tlbsx
//              tlbsync
//              tlbwe

RTL_BEGIN("tlbwe", ___tlbwe___)
    TLBWE();
RTL_END

RTL_BEGIN("tlbre", ___tlbre___)
    TLBRE();
RTL_END

RTL_BEGIN("tlbsx", ___tlbsx___)
    uint64_t ea = REG1;
    if(ARG0){ ea += REG0; }
    TLBSX(ea);
RTL_END

RTL_BEGIN("tlbivax", ___tlbivax___)
    uint64_t ea = REG1;
    if(ARG0){ ea += REG0; }
    TLBIVAX(ea);
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
RTL_END

RTL_BEGIN("tlbsync", ___tlbsync___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    //dummy
RTL_END

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
//               dcbtls
//               dcbtstls
//               icbi
//               icblc
//               icbt
//               icbtls
//
// TODO  : Right now, no cache is implemented, hence cache instructions are all dummy.
//         
// Update 18th June, 2013 : Added checks for exceptions etc.
//                          Added dummy loads/stores for catching TLB exceptions.

RTL_BEGIN("dcba", ___dcba___)
    // if WIMGE[M]=1, this instruction is broadcast
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a store
    STORE32(ea, LOAD32(ea));    // do a dummy store to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbf", ___dcbf___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    // if WIMGE[M]=1, this instruction is boardcast
    //
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy store to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbi", ___dcbi___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a store
    STORE32(ea, LOAD32(ea));    // do a dummy store to capture tlb errors
    
    // dummy
RTL_END

RTL_BEGIN("dcblc", ___dcblc___)
    if(MSR_PR && !MSR_UCLE){ throw PPC_EXCEPT(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_CL, "dcblc: MSR[UCLE]=0, MSR[PR]=1."); }
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbz", ___dcbz___)
    // if WIMGE[M]=1, this instruction is broadcast

    // Exception priorities :-
    // 1. Cache is locked           -> Alignment Exception
    // 2. WIMGE[W]=1 or WIMGE[I]=1  -> Alignment Exception
    // 3. TLB protection violation  -> DSI Exception
    //
    // TODO : Implement the interrupts first. Without these, this is not guaranteed to work

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = (tmp + REG2) & ~(CACHE_LINE_SIZE - 1);   // round the ea to cache line boundary

    // zero out the cache line
    for(size_t i=0; i<CACHE_LINE_SIZE/8; i++){
        STORE64(ea + i*8, 0);   // write 8 bytes at a time
    }

RTL_END

RTL_BEGIN("dcbst", ___dcbst___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    // if WIMGE[M]=1, this instruction is broadcast

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbt", ___dcbt___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbtst", ___dcbtst___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a store
    STORE32(ea, LOAD32(ea));    // do a dummy store to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbtls", ___dcbtls___)
    if(MSR_PR && !MSR_UCLE){ throw PPC_EXCEPT(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_CL, "dcbtls: MSR[UCLE]=0, MSR[PR]=1."); }

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("dcbtstls", ___dcbtstls___)
    if(MSR_PR && !MSR_UCLE){ throw PPC_EXCEPT(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_CL, "dcbtstls: MSR[UCLE]=0, MSR[PR]=1."); }

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a store
    STORE32(ea, LOAD32(ea));    // do a dummy store to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("icbi", ___icbi___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("icblc", ___icblc___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    if(MSR_PR && !MSR_UCLE){ throw PPC_EXCEPT(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_CL, "icblc: MSR[UCLE]=0, MSR[PR]=1."); }

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("icbt", ___icbt___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

RTL_BEGIN("icbtls", ___icbtls___)
    if(MSR_PR && !MSR_UCLE){ throw PPC_EXCEPT(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_CL, "icbtls: MSR[UCLE]=0, MSR[PR]=1."); }

    UMODE tmp = 0;
    UMODE ea;
    if(ARG1)  { tmp = REG1; }
    ea = tmp + REG2;

    // According to Power ISA, this instruction is treated as a load
    LOAD32(ea);    // do a dummy load to capture tlb errors
    // dummy
RTL_END

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

RTL_BEGIN("rfi", ___rfi___)
    MSR = SRR1;
    NIP = ((UMODE)SRR0) & ~0x3ULL;       // Mask Lower 2 bits to zero
RTL_END

RTL_BEGIN("rfmci", ___rfmci___)
    MSR = MCSRR1;
    NIP = ((UMODE)MCSRR0) & ~0x3ULL;
RTL_END

RTL_BEGIN("rfci", ___rfci___)
    MSR = CSRR1;
    NIP = ((UMODE)CSRR0) & ~0x3ULL;
RTL_END

RTL_BEGIN("sc", ___sc___)
    throw PPC_EXCEPT(PPC_EXCEPT_SC, "system call");     // raise a system call exception
RTL_END

RTL_BEGIN("mtmsr", ___mtmsr___)
    uint64_t newmsr = B_32_63(REG0);
    uint8_t newmsr_cm = ((newmsr & MSR_CM) ? 1:0);
    if((MSR_CM == 0) && (newmsr_cm == 1)) { NIP &= 0xffffffff; }

    // Another check is required for MSR_GS == 1, but since we don't have guest mode
    // this is irrelevant.
    MSR = newmsr;
RTL_END

RTL_BEGIN("mfmsr", ___mfmsr___)
    REG0 = B_32_63(MSR);
RTL_END

RTL_BEGIN("wrtee", ___wrtee___)
#define wrtee_code(rS)            \
    MSR &= ~(1L << 15);           \
    MSR |= rS & (1L << 15);

    wrtee_code(REG0);
RTL_END
RTL_BEGIN("wrteei", ___wrteei___)
#define wrteei_code(E)            \
    MSR &= ~(1 << 15);            \
    MSR |= ((E & 0x1) << 15);

    wrteei_code(ARG0);
RTL_END

// START
// ------------------------------ TRAP ----------------------------------------------
// mnemonics :
//             twi
//             tw

RTL_BEGIN("twi", ___twi___)
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
RTL_END

RTL_BEGIN("tw", ___tw___)
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
RTL_END

// START
// ------------------------------ PROCESSOR CONTROL --------------------------------
// mnemonics :
//              mtcrf
//              mcrxr
//              mfcr
//              mtspr
//              mfspr

RTL_BEGIN("mtcrf", ___mtcrf___)
#define mtcrf_code(CRM, rS)                         \
    uint64_t mask = 0, i;                           \
    uint8_t tmp = CRM;                              \
    for(i=0; i<8; i++){                             \
        mask |= (tmp & 0x80)?(0xf << (7 - i)*4):0;  \
        tmp <<= 1;                                  \
    }                                               \
    CR = ((B_32_63(rS) & mask) | (CR & ~mask))

    mtcrf_code(ARG0, REG1);
RTL_END

RTL_BEGIN("mcrxr", ___mcrxr___)
#define mcrxr_code(crfD)                 \
    update_crF(crfD, get_xerF(0));       \
    /* clear XER[32:35] */               \
    XER &= 0xfffffff

    mcrxr_code(ARG0);
RTL_END

RTL_BEGIN("mfcr", ___mfcr___)
#define mfcr_code(rD)                               \
    rD = B_32_63(CR)

    mfcr_code(ARG0);
RTL_END

RTL_BEGIN("mfspr", ___mfspr___)
#define mfspr_code(rD, SPRN)                   \
    rD = SPR(SPRN);

    mfspr_code(REG0, ARG1);
RTL_END

RTL_BEGIN("mtspr", ___mtspr___)
#define mtspr_code(SPRN, rS)                  \
    SPR(SPRN) = rS;

    mtspr_code(ARG0, REG1);       // FIXME : No special checks for SPRN no. Will fix this later on.
RTL_END

// START
// ------------------------------ MEMORY SYNCHRONIZATION ---------------------------
// mnemonics :
//             isync
//             lwarx
//             mbar
//             msync
//             stwcx.

RTL_BEGIN("isync", ___isync___)
    // Do nothing
RTL_END

// Barrier
RTL_BEGIN("mbar", ___mbar___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    //Do nothing
RTL_END

RTL_BEGIN("msync", ___msync___)
    // if HID1[ABE]=1 && CT=1, this instruction is broadcast
    // Do nothing
RTL_END

// Reservation load
RTL_BEGIN("lwarx", ___lwarx___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    SET_RESV(ea, 4);
    REG0 = LOAD32(ea);
RTL_END

// Reservation store
RTL_BEGIN("stwcx.", ___stwcx_dot___)
    UMODE tmp = 0;
    UMODE ea;
    if(ARG1){ tmp = REG1; }
    ea = tmp + REG2;
    if(CHECK_RESV(ea, 4)){
        STORE32(ea, REG0);
        update_crF(0, 0x20 | GET_SO());  // Set ZERO bit
    }else{
        update_crF(0, GET_SO());
    }
    CLEAR_RESV(ea);
RTL_END

// START
// ------------------------------ PERFORMANCE MONITORING ---------------------------
// mnemonics :
//             mfpmr
//             mtpmr

RTL_BEGIN("mtpmr", ___mtpmr___)
#define mtpmr_code(PMRN, rS)                  \
    PMR(PMRN) = rS;

    mtpmr_code(ARG0, REG1);
RTL_END

RTL_BEGIN("mfpmr", ___mfpmr___)
#define mfpmr_code(rD, PMRN)                  \
    rD = PMR(PMRN);

    mfpmr_code(ARG0, REG1);
RTL_END

// START
// ------------------------------ INTEGER SELECT -----------------------------------
// mnemonics :
//              isel

RTL_BEGIN("isel", ___isel___)
    uint64_t a = 0;
    if(ARG1) { a = REG1; }
    if(B_N(CR, (32 + ARG3)) == 0){ REG0 = a;    }
    else                         { REG0 = REG2; }
RTL_END


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

RTL_BEGIN("brinc", ___brinc___)
    // FIXME : Verify value of n for e500v2
    int n         = 32;  // Implementation dependant
    uint64_t mask = (MASK(64-n, 63) & REG2);
    uint64_t a    = (MASK(64-n, 63) & REG1);

    uint64_t d    = BITREVERSE(1 + BITREVERSE(a | ~mask));
    REG0          = (MASK(0, 63-n) & REG1) | (d & mask);
RTL_END

RTL_BEGIN("evabs", ___evabs___)
    uint32_t u, v;
    u    = ABS_32(B_0_31(REG1));
    v    = ABS_32(B_32_63(REG1));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evaddiw", ___evaddiw___)
    uint32_t u, v;
    u    = B_32_63(B_0_31(REG1) + (ARG2 & 0xfffff));
    v    = B_32_63(B_32_63(REG1) +(ARG2 & 0xfffff));
    // ARG2 is zero extended and added to each of lower & upper halves of REG1
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evaddsmiaaw", ___evaddsmiaaw___)
    uint32_t u, v;
    u    = B_32_63(B_0_31(ACC) + B_0_31(REG1));
    v    = B_32_63(B_32_63(ACC) + B_32_63(REG1));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evaddssiaaw", ___evaddssiaaw___)
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
RTL_END

RTL_BEGIN("evaddusiaaw", ___evaddusiaaw___)
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
RTL_END

RTL_BEGIN("evaddumiaaw", ___evaddumiaaw___)
    uint32_t u, v;
    u     = B_32_63(B_0_31(ACC) + B_0_31(REG1));
    v     = B_32_63(B_32_63(ACC) + B_32_63(REG1));
    REG0  = PACK_2W(u, v);
    ACC   = REG0;
RTL_END

RTL_BEGIN("evaddw", ___evaddw___)
    uint32_t u, v;
    u     = B_32_63(B_0_31(REG1) + B_0_31(REG2));
    v     = B_32_63(B_32_63(REG1) + B_32_63(REG2));
    REG0  = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evand", ___evand___)
    REG0  = REG1 & REG2;
RTL_END

RTL_BEGIN("evandc", ___evandc___)
    REG0  = REG1 & ~REG2;
RTL_END

RTL_BEGIN("evcmpeq", ___evcmpeq___)
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
RTL_END

RTL_BEGIN("evcmpgts", ___evcmpgts___)
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
RTL_END

RTL_BEGIN("evcmpgtu", ___evcmpgtu___)
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
RTL_END

RTL_BEGIN("evcmplts", ___evcmplts___)
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
RTL_END

RTL_BEGIN("evcmpltu", ___evcmpltu___)
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
RTL_END

RTL_BEGIN("evcntlsw", ___evcntlsw___)
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
RTL_END

RTL_BEGIN("evcntlzw", ___evcntlzw___)
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
RTL_END

RTL_BEGIN("evdivws", ___evdivws___)
    // TODO : Implement this
    throw sim_except_fatal("Not implemented !!");
RTL_END

RTL_BEGIN("evdivwu", ___evdivwu___)
    // TODO : Implement this
    throw sim_except_fatal("Not implemented !!");
RTL_END

RTL_BEGIN("eveqv", ___eveqv___)
    REG0 = REG1 ^ REG2;  // xor to compare
RTL_END

RTL_BEGIN("evextsb", ___evextsb___)
    uint32_t u, v;
    u    = B_32_63(EXTS_B2D(B_24_31(REG1)));
    v    = B_32_63(EXTS_B2D(B_56_63(REG1)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evextsh", ___evextsh___)
    uint32_t u, v;
    u    = B_32_63(EXTS_H2D(B_16_31(REG1)));
    v    = B_32_63(EXTS_H2D(B_48_63(REG1)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evldd", ___evldd___)
    UMODE b = 0;
    UMODE ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = LOAD64(ea);
RTL_END

RTL_BEGIN("evlddx", ___evlddx___)
    UMODE b = 0;
    UMODE ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = LOAD64(ea);
RTL_END

RTL_BEGIN("evldh", ___evldh___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea + 2)) << 32) | (U64(LOAD16(ea + 4)) << 16) | U64(LOAD16(ea + 6));
RTL_END

RTL_BEGIN("evldhx", ___evldhx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea + 2)) << 32) | (U64(LOAD16(ea + 4)) << 16) | U64(LOAD16(ea + 6));
RTL_END

RTL_BEGIN("evldw", ___evldw___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD32(ea)) << 32) | U64(LOAD32(ea + 4));
RTL_END

RTL_BEGIN("evldwx", ___evldwx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    REG0 = (U64(LOAD32(ea)) << 32) | U64(LOAD32(ea + 4));
RTL_END

RTL_BEGIN("evlhhesplat", ___evlhhesplat___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t v = LOAD16(ea);
    REG0 = ((U64(v) << 48) | (U64(v) << 16)) & 0xffff0000ffff0000ULL;
RTL_END

RTL_BEGIN("evlhhesplatx", ___evlhhesplatx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    uint16_t v = LOAD16(ea);
    REG0 = ((U64(v) << 48) | (U64(v) << 16)) & 0xffff0000ffff0000ULL;
RTL_END

RTL_BEGIN("evlhhossplat", ___evlhhossplat___)
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    u    = B_32_63(EXTS_H2W(LOAD16(ea)));
    REG0 = PACK_2W(u, u);
RTL_END

RTL_BEGIN("evlhhossplatx", ___evlhhossplatx___)
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    u    = B_32_63(EXTS_H2W(LOAD16(ea)));
    REG0 = PACK_2W(u, u);
RTL_END

RTL_BEGIN("evlhhousplat", ___evlhhousplat___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t v = LOAD16(ea);
    REG0 = (U64(v) << 32) | U64(v);
RTL_END

RTL_BEGIN("evlhhousplatx", ___evlhhousplatx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1;  }
    ea = b + REG2;

    uint16_t v = LOAD16(ea);
    REG0 = (U64(v) << 32) | U64(v);
RTL_END

RTL_BEGIN("evlwhe", ___evlwhe___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea+2)) << 16);
RTL_END

RTL_BEGIN("evlwhex", ___evlwhex___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    REG0 = (U64(LOAD16(ea)) << 48) | (U64(LOAD16(ea+2)) << 16);
RTL_END

RTL_BEGIN("evlwhos", ___evlwhos___)
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;
    
    u    = B_32_63(EXTS_H2D(LOAD16(ea)));
    v    = B_32_63(EXTS_H2D(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evlwhosx", ___evlwhosx___)
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    u    = B_32_63(EXTS_H2D(LOAD16(ea)));
    v    = B_32_63(EXTS_H2D(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evlwhou", ___evlwhou___)
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    u    = B_32_63(U64(LOAD16(ea)));
    v    = B_32_63(U64(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evlwhoux", ___evlwhoux___)
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    u    = B_32_63(U64(LOAD16(ea)));
    v    = B_32_63(U64(LOAD16(ea+2)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evlwhsplat", ___evlwhsplat___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint16_t u = LOAD16(ea);
    uint16_t v = LOAD16(ea+2);

    REG0 = (U64(u) << 48) | (U64(u) << 32) | (U64(v) << 16) | U64(v);
RTL_END

RTL_BEGIN("evlwhsplatx", ___evlwhsplatx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    uint16_t u = LOAD16(ea);
    uint16_t v = LOAD16(ea+2);

    REG0 = (U64(u) << 48) | (U64(u) << 32) | (U64(v) << 16) | U64(v);
RTL_END

RTL_BEGIN("evlwwsplat", ___evlwwsplat___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea = b + ARG1;

    uint32_t u = LOAD32(ea);

    REG0 = (U64(u) << 32) | U64(u);
RTL_END

RTL_BEGIN("evlwwsplatx", ___evlwwsplatx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea = b + REG2;

    uint32_t u = LOAD32(ea);

    REG0 = (U64(u) << 32) | U64(u);
RTL_END

RTL_BEGIN("evmergehi", ___evmergehi___)
    uint32_t u, v;
    u    = B_0_31(REG1);
    v    = B_0_31(REG2);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evmergelo", ___evmergelo___)
    uint32_t u, v;
    u    = B_32_63(REG1);
    v    = B_32_63(REG2);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evmergehilo", ___evmergehilo___)
    uint32_t u, v;
    u    = B_0_31(REG1);
    v    = B_32_63(REG2);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evmergelohi", ___evmergelohi___)
    uint32_t u, v;
    u    = B_32_63(REG1);
    v    = B_0_31(REG2);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evmhegsmfaa", ___evmhegsmfaa___)
    uint64_t tmp = GSF(B_32_47(REG1), B_32_47(REG2));
    REG0 = ACC + tmp;
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhegsmfan", ___evmhegsmfan___)
    uint64_t tmp = GSF(B_32_47(REG1), B_32_47(REG2));
    REG0 = ACC - tmp;
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhegsmiaa", ___evmhegsmiaa___)
    uint64_t tmp = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));
    tmp = EXTS_W2D(tmp);
    REG0 = ACC + tmp;
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhegsmian", ___evmhegsmian___)
    uint64_t tmp = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));
    tmp = EXTS_W2D(tmp);
    REG0 = ACC + tmp;
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhegumiaa", ___evmhegumiaa___)
    uint64_t tmp = B_32_63(B_32_47(REG1) * B_32_47(REG2));
    REG0 = ACC + tmp;
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhegumian", ___evmhegumian___)
    uint64_t tmp = B_32_63(B_32_47(REG1) * B_32_47(REG2));
    REG0 = ACC - tmp;
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhesmf", ___evmhesmf___)
    uint32_t u, v;
    u    = B_32_63(SF(B_0_15(REG1), B_0_15(REG2)));
    v    = B_32_63(SF(B_32_47(REG1), B_32_47(REG2)));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evmhesmfa", ___evmhesmfa___)
    uint32_t u, v;
    u    = B_32_63(SF(B_0_15(REG1), B_0_15(REG2)));
    v    = B_32_63(SF(B_32_47(REG1), B_32_47(REG2)));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhesmfaaw", ___evmhesmfaaw___)
    uint32_t u = B_32_63(B_0_31(ACC)  + SF(B_0_15(REG1), B_0_15(REG2)));
    uint32_t v = B_32_63(B_32_63(ACC) + SF(B_32_47(REG1), B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhesmfanw", ___evmhesmfanw___)
    uint32_t u = B_32_63(B_0_31(ACC)  - SF(B_0_15(REG1), B_0_15(REG2)));
    uint32_t v = B_32_63(B_32_63(ACC) - SF(B_32_47(REG1), B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhesmi", ___evmhesmi___)
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmhesmia", ___evmhesmia___)
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhesmiaaw", ___evmhesmiaaw___)
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    u = B_0_31(ACC) + u;
    v = B_32_63(ACC) + v;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhesmianw", ___evmhesmianw___)
    uint32_t u = B_32_63(EXTS_H2W(B_0_15(REG1)) * EXTS_H2W(B_0_15(REG2)));
    uint32_t v = B_32_63(EXTS_H2W(B_32_47(REG1)) * EXTS_H2W(B_32_47(REG2)));

    u = B_0_31(ACC) - u;
    v = B_32_63(ACC) - v;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhessf", ___evmhessf___)
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
RTL_END

RTL_BEGIN("evmhessfa", ___evmhessfa___)
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
RTL_END

RTL_BEGIN("evmhessfaaw", ___evmhessfaaw___)
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
RTL_END

RTL_BEGIN("evmhessfanw", ___evmhessfanw___)
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
RTL_END

RTL_BEGIN("evmhessiaaw", ___evmhessiaaw___)
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
RTL_END

RTL_BEGIN("evmhessianw", ___evmhessianw___)
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
RTL_END

RTL_BEGIN("evmheumi", ___evmheumi___)
    uint32_t u, v;

    u  = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    v  = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));

    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmheumia", ___evmheumia___)
    uint32_t u, v;

    u  = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    v  = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmheumiaaw", ___evmheumiaaw___)
    uint32_t tmp;
    uint32_t u, v;

    tmp = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmheumianw", ___evmheumianw___)
    uint32_t tmp;
    uint32_t u, v;

    tmp = X86_MULUW_L(B_0_15(REG1), B_0_15(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = X86_MULUW_L(B_32_47(REG1), B_32_47(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmheusiaaw", ___evmheusiaaw___)
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
RTL_END

RTL_BEGIN("evmheusianw", ___evmheusianw___)
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
RTL_END


RTL_BEGIN("evmhogsmfaa", ___evmhogsmfaa___)
    uint64_t tmp64;
    tmp64 = GSF(B_48_63(REG1), B_48_63(REG2));
    REG0  = ACC + tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmhogsmfan", ___evmhogsmfan___)
    uint64_t tmp64;
    tmp64 = GSF(B_48_63(REG1), B_48_63(REG2));
    REG0  = ACC - tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmhogsmiaa", ___evmhogsmiaa___)
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(tmp);
    REG0  = ACC + tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmhogsmian", ___evmhogsmian___)
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTS_W2D(tmp);
    REG0  = ACC - tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmhogumiaa", ___evmhogumiaa___)
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(tmp);
    REG0  = ACC + tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmhogumian", ___evmhogumian___)
    uint32_t tmp;
    uint64_t tmp64;
    tmp   = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    tmp64 = EXTZ_W2D(tmp);
    REG0  = ACC - tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmhosmf", ___evmhosmf___)
    uint32_t u, v;
    u    = SF(B_16_31(REG1), B_16_31(REG2));
    v    = SF(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmhosmfa", ___evmhosmfa___)
    uint32_t u, v;
    u    = SF(B_16_31(REG1), B_16_31(REG2));
    v    = SF(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhosmfaaw", ___evmhosmfaaw___)
    uint32_t tmp, u, v;

    tmp = SF(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhosmfanw", ___evmhosmfanw___)
    uint32_t tmp, u, v;

    tmp = SF(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = SF(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhosmi", ___evmhosmi___)
    uint32_t u, v;
    u    = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmhosmia", ___evmhosmia___)
    uint32_t u, v;
    u    = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhosmiaaw", ___evmhosmiaaw___)
    uint32_t tmp, u, v;

    tmp = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) + tmp;

    tmp = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhosmianw", ___evmhosmianw___)
    uint32_t tmp, u, v;

    tmp = X86_MULW_L(B_16_31(REG1), B_16_31(REG2));
    u   = B_0_31(ACC) - tmp;

    tmp = X86_MULW_L(B_48_63(REG1), B_48_63(REG2));
    v   = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhossf", ___evmhossf___)
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
RTL_END

RTL_BEGIN("evmhossfa", ___evmhossfa___)
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
RTL_END

RTL_BEGIN("evmhossfaaw", ___evmhossfaaw___)
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
RTL_END

RTL_BEGIN("evmhossfanw", ___evmhossfanw___)
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
RTL_END

RTL_BEGIN("evmhossiaaw", ___evmhossiaaw___)
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
RTL_END

RTL_BEGIN("evmhossianw", ___evmhossianw___)
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
RTL_END

RTL_BEGIN("evmhoumi", ___evmhoumi___)
    uint32_t u, v;
    u    = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmhoumia", ___evmhoumia___)
    uint32_t u, v;
    u    = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    v    = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhoumiaaw", ___evmhoumiaaw___)
    uint32_t tmp, u, v;

    tmp  = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    u    = B_0_31(ACC) + tmp;

    tmp  = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    v    = B_32_63(ACC) + tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhoumianw", ___evmhoumianw___)
    uint32_t tmp, u, v;

    tmp  = X86_MULUW_L(B_16_31(REG1), B_16_31(REG2));
    u    = B_0_31(ACC) - tmp;

    tmp  = X86_MULUW_L(B_48_63(REG1), B_48_63(REG2));
    v    = B_32_63(ACC) - tmp;

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmhousiaaw", ___evmhousiaaw___)
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
RTL_END

RTL_BEGIN("evmhousianw", ___evmhousianw___)
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
RTL_END

RTL_BEGIN("evmra", ___evmra___)
    ACC  = REG1;
    REG0 = REG1;
RTL_END

RTL_BEGIN("evmwhsmf", ___evmwhsmf___)
    uint32_t u, v;

    u = B_0_31(SF(B_0_31(REG1), B_0_31(REG2)));
    v = B_0_31(SF(B_32_63(REG1), B_32_63(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmwhsmfa", ___evmwhsmfa___)
    uint32_t u, v;

    u = B_0_31(SF(B_0_31(REG1), B_0_31(REG2)));
    v = B_0_31(SF(B_32_63(REG1), B_32_63(REG2)));

    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwhssf", ___evmwhssf___)
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
RTL_END

RTL_BEGIN("evmwhssfa", ___evmwhssfa___)
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
RTL_END

RTL_BEGIN("evmwhumi", ___evmwhumi___)
    uint32_t u, v;
    u    = X86_MULUW_H(B_0_31(REG1), B_0_31(REG2));      // get high word
    v    = X86_MULUW_H(B_32_63(REG1), B_32_63(REG2));    // get high word
    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evmwhumia", ___evmwhumia___)
    uint32_t u, v;
    u    = X86_MULUW_H(B_0_31(REG1), B_0_31(REG2));      // get high word
    v    = X86_MULUW_H(B_32_63(REG1), B_32_63(REG2));    // get high word
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwlsmiaaw", ___evmwlsmiaaw___)
    uint32_t u, v;
    u    = B_0_31(ACC)  + X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) + X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwlsmianw", ___evmwlsmianw___)
    uint32_t u, v;
    u    = B_0_31(ACC)  - X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) - X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = (U64(u) << 32) | U64(v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwlssiaaw", ___evmwlssiaaw___)
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
RTL_END

RTL_BEGIN("evmwlssianw", ___evmwlssianw___)
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
RTL_END

RTL_BEGIN("evmwlumi", ___evmwlumi___)
    uint32_t u, v;
    u    = X86_MULUW_L(B_0_31(REG1), B_0_31(REG2));
    v    = X86_MULUW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evmwlumia", ___evmwlumia___)
    uint32_t u, v;
    u    = X86_MULUW_L(B_0_31(REG1), B_0_31(REG2));
    v    = X86_MULUW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwlumiaaw", ___evmwlumiaaw___)
    uint32_t u, v;
    u    = B_0_31(ACC)  + X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) + X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwlumianw", ___evmwlumianw___)
    uint32_t u, v;
    u    = B_0_31(ACC)  - X86_MULW_L(B_0_31(REG1), B_0_31(REG2));
    v    = B_32_63(ACC) - X86_MULW_L(B_32_63(REG1), B_32_63(REG2));
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwlusiaaw", ___evmwlusiaaw___)
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
RTL_END

RTL_BEGIN("evmwlusianw", ___evmwlusianw___)
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
RTL_END

RTL_BEGIN("evmwsmf", ___evmwsmf___)
    REG0 = SF(B_32_63(REG1), B_32_63(REG2));
RTL_END

RTL_BEGIN("evmwsmfa", ___evmwsmfa___)
    REG0 = SF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwsmfaa", ___evmwsmfaa___)
    uint64_t tmp64;
    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    REG0  = ACC + tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmwsmfan", ___evmwsmfan___)
    uint64_t tmp64;
    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    REG0  = ACC - tmp64;
    ACC   = REG0;
RTL_END

RTL_BEGIN("evmwsmi", ___evmwsmi___)
    REG0 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
RTL_END

RTL_BEGIN("evmwsmia", ___evmwsmia___)
    REG0 = X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwsmiaa", ___evmwsmiaa___)
    REG0 = ACC + X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwsmian", ___evmwsmian___)
    REG0 = ACC - X86_MULWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwssf", ___evmwssf___)
    uint64_t tmp64;
    bool mov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { REG0 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { REG0 = tmp64; mov = 0;                 }

    UPDATE_SPEFSCR_OV(0, mov);
RTL_END

RTL_BEGIN("evmwssfa", ___evmwssfa___)
    uint64_t tmp64;
    bool mov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { REG0 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { REG0 = tmp64; mov = 0;                 }
    ACC = REG0;

    UPDATE_SPEFSCR_OV(0, mov);
RTL_END

RTL_BEGIN("evmwssfaa", ___evmwssfaa___)
    uint64_t tmp64;
    bool mov, ov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { tmp64 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { mov = 0;                                }
    REG0 = X86_ADD64(ACC, tmp64);
    ov   = GET_CF_H() ^ B_N(REG0, 0);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(0, ov | mov);
RTL_END

RTL_BEGIN("evmwssfan", ___evmwssfan___)
    uint64_t tmp64;
    bool mov, ov;

    tmp64 = SF(B_32_63(REG1), B_32_63(REG2));
    if((B_32_63(REG1) == 0x80000000) && (B_32_63(REG2) == 0x80000000)) { tmp64 = 0x7fffffffffffffffULL; mov = 1; }
    else                                                               { mov = 0;                                }
    REG0 = X86_SUB64(ACC, tmp64);
    ov   = GET_CF_H() ^ B_N(REG0, 0);
    ACC  = REG0;

    UPDATE_SPEFSCR_OV(0, ov | mov);
RTL_END

RTL_BEGIN("evmwumi", ___evmwumi___)
    REG0 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
RTL_END

RTL_BEGIN("evmwumia", ___evmwumia___)
    REG0 = X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwumiaa", ___evmwumiaa___)
    REG0 = ACC + X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evmwumian", ___evmwumian___)
    REG0 = ACC - X86_MULUWF(B_32_63(REG1), B_32_63(REG2));
    ACC  = REG0;
RTL_END

RTL_BEGIN("evnand", ___evnand___)
    REG0 = ~(REG1 & REG2);
RTL_END

RTL_BEGIN("evneg", ___evneg___)
    REG0 = PACK_2W(X86_NEGW(B_0_31(REG1)), X86_NEGW(B_32_63(REG1)));
RTL_END

RTL_BEGIN("evnor", ___evnor___)
    REG0 = ~(REG1 | REG2);
RTL_END

RTL_BEGIN("evor", ___evor___)
    REG0 = REG1 | REG2;
RTL_END

RTL_BEGIN("evorc", ___evorc___)
    REG0 = REG1 | ~REG2;
RTL_END

RTL_BEGIN("evrlw", ___evrlw___)
    int nh, nl;
    uint32_t u, v;

    nh   = B_27_31(REG2);
    nl   = B_59_63(REG2);
    u    = ROTL32(B_0_31(REG1), nh);
    v    = ROTL32(B_32_63(REG1), nl);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evrlwi", ___evrlwi___)
    uint32_t u, v;
    int n = ARG2;
    u     = ROTL32(B_0_31(REG1), n);
    v     = ROTL32(B_32_63(REG1), n);
    REG0  = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evrndw", ___evrndw___)
    uint32_t u, v;
    u    = (B_0_31(REG1)  + 0x8000) & 0xffff0000;
    v    = (B_32_63(REG1) + 0x8000) & 0xffff0000;
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evsel", ___evsel___)
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
RTL_END


RTL_BEGIN("evslw", ___evslw___)
    int nh, nl;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    REG0 = PACK_2W(SL(B_0_31(REG1), nh), SL(B_32_63(REG1), nl));
RTL_END

RTL_BEGIN("evslwi", ___evslwi___)
    int n = ARG2;
    REG0  = PACK_2W(SL(B_0_31(REG1), n), SL(B_32_63(REG2), n));
RTL_END

RTL_BEGIN("evsplatfi", ___evsplatfi___)
    uint32_t u;
    u    = ((ARG1 & 0x1f) << 27);
    REG0 = PACK_2W(u, u);
RTL_END

RTL_BEGIN("evsplati", ___evsplati___)
    uint32_t u = EXTS_5B_2_32B(ARG1);
    REG0       = PACK_2W(u, u);
RTL_END

RTL_BEGIN("evsrwis", ___evsrwis___)
    int n = ARG2;
    uint32_t u, v;

    u     = U32(EXTS_BF2D(REG0, 0, 31-n));
    v     = U32(EXTS_BF2D(REG0, 32, 63-n));
    REG0  = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evsrwiu", ___evsrwiu___)
    int n = ARG2;
    uint32_t u, v;

    u    = U32(EXTZ_BF2D(REG0, 0, 31-n));
    v    = U32(EXTZ_BF2D(REG0, 32, 63-n));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evsrws", ___evsrws___)
    int nh, nl;
    uint32_t u, v;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    u    = U32(EXTS_BF2D((REG1), 0, 31-nh));
    v    = U32(EXTS_BF2D((REG1), 32, 63-nl));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evsrwu", ___evsrwu___)
    int nh, nl;
    uint32_t u, v;

    nh   = B_26_31(REG2);
    nl   = B_58_63(REG2);
    u    = U32(EXTZ_BF2D((REG1), 0, 31-nh));
    v    = U32(EXTZ_BF2D((REG1), 32, 63-nl));
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evstdd", ___evstdd___)
    UMODE b = 0, ea;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    REG0 = LOAD64(ea);
RTL_END

RTL_BEGIN("evstddx", ___evstddx___)
    UMODE b = 0, ea;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    REG0 = LOAD64(ea);
RTL_END

RTL_BEGIN("evstdh", ___evstdh___)
    UMODE b = 0, ea;
    uint16_t u, v, w, x;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    w    = LOAD16(ea+4);
    x    = LOAD16(ea+6);
    REG0 = (U64(u) << 48) | (U64(v) << 32) | (U64(w) << 16) | U64(x);
RTL_END

RTL_BEGIN("evstdhx", ___evstdhx___)
    UMODE b = 0, ea;
    uint16_t u, v, w, x;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    w    = LOAD16(ea+4);
    x    = LOAD16(ea+6);
    REG0 = (U64(u) << 48) | (U64(v) << 32) | (U64(w) << 16) | U64(x);
RTL_END

RTL_BEGIN("evstdw", ___evstdw___)
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    v    = LOAD32(ea+4);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evstdwx", ___evstdwx___)
    UMODE b = 0, ea;
    uint32_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    v    = LOAD32(ea+4);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evstwhe", ___evstwhe___)
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 48) | (U64(v) << 16);
RTL_END

RTL_BEGIN("evstwhex", ___evstwhex___)
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 48) | (U64(v) << 16);
RTL_END

RTL_BEGIN("evstwho", ___evstwho___)
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evstwhox", ___evstwhox___)
    UMODE b = 0, ea;
    uint16_t u, v;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD16(ea);
    v    = LOAD16(ea+2);
    REG0 = (U64(u) << 32) | U64(v);
RTL_END

RTL_BEGIN("evstwwe", ___evstwwe___)
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    REG0 = (U64(u) << 32);
RTL_END

RTL_BEGIN("evstwwex", ___evstwwex___)
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    REG0 = (U64(u) << 32);
RTL_END

RTL_BEGIN("evstwwo", ___evstwwo___)
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG2){ b = REG2; }
    ea   = b + ARG1;
    u    = LOAD32(ea);
    REG0 = U64(u);
RTL_END

RTL_BEGIN("evstwwox", ___evstwwox___)
    UMODE b = 0, ea;
    uint32_t u;

    if(ARG1){ b = REG1; }
    ea   = b + REG2;
    u    = LOAD32(ea);
    REG0 = U64(u);
RTL_END

RTL_BEGIN("evsubfsmiaaw", ___evsubfsmiaaw___)
    uint32_t u, v;
    u    = B_0_31(ACC)  - B_0_31(REG1);
    v    = B_32_63(ACC) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evsubfssiaaw", ___evsubfssiaaw___)
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
RTL_END

// NOTE: don't know what's the actual difference between evsubfsmiaaw & evsubfumiaaw
//       Techically signed subtraction & unsigned subtraction return the same final
//       value according to rules of 2's complement arithmetic. The diff lies only in
//       interpretation of final result.
RTL_BEGIN("evsubfumiaaw", ___evsubfumiaaw___)
    uint32_t u, v;
    u    = B_0_31(ACC)  - B_0_31(REG1);
    v    = B_32_63(ACC) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
    ACC  = REG0;
RTL_END

RTL_BEGIN("evsubfw", ___evsubfw___)
    uint32_t u, v;
    u    = B_0_31(REG2)  - B_0_31(REG1);
    v    = B_32_63(REG2) - B_32_63(REG1);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evsubfusiaaw", ___evsubfusiaaw___)
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
RTL_END

RTL_BEGIN("evsubifw", ___evsubifw___)
    uint32_t u, v;
    u    = B_0_31(REG2)  - (ARG1 & 0x1f);
    v    = B_32_63(REG2) - (ARG1 & 0x1f);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evxor", ___evxor___)
    REG0 = REG1 ^ REG2;
RTL_END

// ----------------- SPE FP ---------------------------------------------------------------------------
//
//
#define SPE_FP_2_OP_DEFRES_32(res, opcode, arg0, arg1)   if unlikely(HOST_FPU_FLAGS.is_error())  \
                                                         ppcsimbooke::fp_emul::get_default_results<uint32_t>(res, opcode, arg0, arg1, HOST_FPU_FLAGS)
#define SPE_FP_1_OP_DEFRES_32(res, opcode, arg0)         if unlikely(HOST_FPU_FLAGS.is_error())  \
                                                         ppcsimbooke::fp_emul::get_default_results<uint32_t>(res, opcode, arg0, HOST_FPU_FLAGS)

// Vector FP SP Absolute
RTL_BEGIN("evfsabs", __evfsabs__)
    REG0 = REG1 & 0x7fffffff7fffffffULL;
RTL_END

// Vector FP SP Negative Absolute
RTL_BEGIN("evfsnabs", __evfsnabs__)
    REG0 = REG1 | 0x8000000080000000ULL;
RTL_END

// Vector FP SP Negate
RTL_BEGIN("evfsneg", __evfsneg__)
    REG0 = REG1 | (~REG1 & 0x8000000080000000ULL);
RTL_END

RTL_BEGIN("evfsadd", __evfsadd__)
    uint32_t u, v;
    u = X86_ADDS_F32(B_0_31(REG1), B_0_31(REG2));
    SPE_FP_2_OP_DEFRES_32(u, fp_emul::fp_op_add, REG1, REG2);
    UPDATE_SPEFSCR_H(1);
    v = X86_ADDS_F32(B_32_63(REG1), B_32_63(REG2));
    SPE_FP_2_OP_DEFRES_32(v, fp_emul::fp_op_add, REG1, REG2);
    UPDATE_SPEFSCR_H(0);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evfssub", __evfssub__)
    uint32_t u, v;
    u = X86_SUBS_F32(B_0_31(REG1), B_0_31(REG2));
    SPE_FP_2_OP_DEFRES_32(u, fp_emul::fp_op_sub, REG1, REG2);
    UPDATE_SPEFSCR_H(1);
    v = X86_SUBS_F32(B_32_63(REG1), B_32_63(REG2));
    SPE_FP_2_OP_DEFRES_32(v, fp_emul::fp_op_sub, REG1, REG2);
    UPDATE_SPEFSCR_H(0);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evfsmul", __evfsmul__)
    uint32_t u, v;
    u = X86_MULS_F32(B_0_31(REG1), B_0_31(REG2));
    SPE_FP_2_OP_DEFRES_32(u, fp_emul::fp_op_mul, REG1, REG2);
    UPDATE_SPEFSCR_H(1);
    v = X86_MULS_F32(B_32_63(REG1), B_32_63(REG2));
    SPE_FP_2_OP_DEFRES_32(v, fp_emul::fp_op_mul, REG1, REG2);
    UPDATE_SPEFSCR_H(0);
    REG0 = PACK_2W(u, v);
RTL_END

RTL_BEGIN("evfsdiv", __evfsdiv__)
    uint32_t u, v;
    u = X86_DIVS_F32(B_0_31(REG1), B_0_31(REG2));
    SPE_FP_2_OP_DEFRES_32(u, fp_emul::fp_op_div, REG1, REG2);
    UPDATE_SPEFSCR_H(1);
    v = X86_DIVS_F32(B_32_63(REG1), B_32_63(REG2));
    SPE_FP_2_OP_DEFRES_32(v, fp_emul::fp_op_div, REG1, REG2);
    UPDATE_SPEFSCR_H(0);
    REG0 = PACK_2W(u, v);
RTL_END


RTL_BEGIN("efdabs", ___efdabs___)
    REG0 = REG1 & 0x7fffffffffffffffULL;       // Change sign bit to zero
RTL_END

RTL_BEGIN("efdnabs", ___efdnabs___)
    REG0 = (REG1 & 0x7fffffffffffffffULL) | 0x8000000000000000ULL;
RTL_END

RTL_BEGIN("efdneg", ___efdneg___)
    REG0 = (REG1 & 0x7fffffffffffffffULL) | ((REG1 ^ 0x8000000000000000ULL) & 0x8000000000000000ULL);
RTL_END


// Restore all saved macros
#pragma pop_macro("PPCREG")
#pragma pop_macro("DREG")
#pragma pop_macro("DREG_FN")
#pragma pop_macro("MSR_CM")
#pragma pop_macro("MSR_UCLE")
#pragma pop_macro("MSR_PR")
#pragma pop_macro("GPR")
#pragma pop_macro("SPR")
#pragma pop_macro("XER")
#pragma pop_macro("MSR")
#pragma pop_macro("ACC")
#pragma pop_macro("SPEFSCR")
#pragma pop_macro("PMR")
#pragma pop_macro("CR")
#pragma pop_macro("LR")
#pragma pop_macro("CTR")
#pragma pop_macro("SRR0")
#pragma pop_macro("SRR1")
#pragma pop_macro("CSRR0")
#pragma pop_macro("CSRR1")
#pragma pop_macro("MCSRR0")
#pragma pop_macro("MCSRR1")
#pragma pop_macro("PC")
#pragma pop_macro("NIP")
#pragma pop_macro("HOST_FLAGS")
#pragma pop_macro("HOST_FPU_FLAGS")
#pragma pop_macro("CACHE_LINE_SIZE")

#endif
