/*
 *  POWER/PowerPC instructions.
 *
 *  This file format was taken from a similar file from "gxemu" project by Anders Gavare,
 *  but it doesn't contain any of the code in any of those files.
 *
 *  -----------------------------------------------------------------------------------
 *  Author : Vikas Chouhan ( presentisgood@gmail.com )
 *  copyright (C) 2012.
 *  
 *  All instructions have been heavily conditioned for Freescale ppc processors.
 *  e500v2 is taken as a reference, but the code is generic and can be used for higher
 *  64 bit cores.
 *
 *  NOTE:
 *  This file is not a regular C/C++ file ( although it looks like it :) ).
 *  It's just a template for defining instructions'
 *  pseudocode implementation. An external utility uses this template to generate a new
 *  header/C++ file before direct use.
 *
 *  Pseudocodes are written in accordance with Power ISA 2.06 provided by power.org and
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
#define PPCREG(regid)            (CPU->reg(regid))
#define PPCREGN(reg_name)        (CPU->regn(reg_name))

// MSR_CM
#define CM                       ((PPCREG(REG_MSR) & MSR_CM) ? 1 : 0)

#define GPR(gprno)               PPCREG(REG_GPR0 + gprno)
#define SPR(sprno)               PPCREG(REG_SPR0 + sprno)
#define XER                      SPR(SPRN_XER) 
#define MSR                      PPCREG(REG_MSR) 
#define PMR(pmrno)               PPCREG(REG_PMR0 + pmrno) 
#define CR                       PPCREG(REG_CR)
#define LR                       PPCREG(REG_LR)
#define CTR                      PPCREG(REG_CTR)
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
#define HOST_FLAGS               CPU->host_state.flags
#define dummy_flags              CPU->host_state.dummy

// Generic macros
#define UPDATE_CA()               update_xer_ca_host()
#define UPDATE_SO_OV()            update_xer_host()
#define UPDATE_SO_OV_V            update_xer_so_ov 
#define UPDATE_CR0()              update_cr0_host()
#define GET_CA()                  get_xer_ca()
#define GET_SO()                  get_xer_so()
#define UPDATE_CR0_V              update_cr0

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
#define UT(arg)   ((UMODE)(arg))
#define ST(arg)   ((SMODE)(arg))

// for sign extension ( extending sign bits from source type to destination type )
#define EXTS(t_tgt, t_src, val)    ((t_tgt)((t_src)(val)))
#define EXTS_2N(t_src, val)        EXTS(SMODE, t_src, val)    // sign extension to native type
#define EXTS_B2N(val)              EXTS(SMODE, int8_t, val)   // sign extension : byte to native
#define EXTS_H2N(val)              EXTS(SMODE, int16_t, val)  // sign extension : half word to native
#define EXTS_W2N(val)              EXTS(SMODE, int32_t, val)  // sign extension : word to native

// rotation macros
#define ROTL64(x, y)               ((uint64_t)(((x) << (y)) | ((x) >> (64 - (y)))))                           // x=64bit, y=64bit
#define ROTL32(x, y)               ROTL64((((x) & 0xffffffff) | (((x) & 0xffffffff) << 32)), (y))             // x=32bit, y=64bit

// mask
#define BITMASK(x)                 ((x) ? ((1ULL << (64 - (x))) - 1) : -1)                                                // 64 bit Bitmask for bit pos x
#define MASK(x, y)                 (((x) < (y)) ? (BITMASK(x) ^ BITMASK(y+1)) : ~(BITMASK(x+1) ^ BITMASK(y)))             // Generate mask of 1's from x to y

// 32_63
#define B_32_63(x)                 ((x) & 0xffffffff)

// Byte reversing macros
#define SWAPB32(data)              ((((data >> 24) & 0xff) <<  0) ||  \
                                    (((data >> 16) & 0xff) <<  8) ||  \
                                    (((data >>  8) & 0xff) << 16) ||  \
                                    (((data >>  0) & 0xff) << 24) )
#define SWAPB16(data)              ((((data >>  8) & 0xff) <<  0) ||  \
                                    (((data >>  0) & 0xff) <<  8) )

// PPC Exceptions
#define  PPC_EXCEPT         sim_except_ppc

/*----------------------------------------------------------------------------
 * x86/x86_64 assembly.
 * Instead of calculating Overflow, carry bits etc, we are leveraging host flag
 * register directly.
 * ---------------------------------------------------------------------------*/
#if HOST_ARCH == x86_64

#define EBX       "%rbx"
#define ECX       "%rcx"
#define POP_ECX   "pop %%rcx\n"

#elif HOST_ARCH == i686

#define EBX       "%ebx"
#define ECX       "%ecx"
#define POP_ECX   "pop %%ecx\n"

#endif

// NOTE: 1. Division routines don't check for divide by zero conditions. We will
//          catch a SIGFPE signal, if this isn't handled in advanced.
#define addw(arg0, arg1, arg2)                                                   \
    asm volatile(                                                                \
            "mov %3, %%ebx\n"                                                    \
            "add %2, %%ebx\n"                                                    \
            "mov %%ebx, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define subw(arg0, arg1, arg2)                                                   \
    asm volatile(                                                                \
            "mov %2, %%ebx\n"                                                    \
            "sub %3, %%ebx\n"                                                    \
            "mov %%ebx, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define divw(arg0, arg1, arg2)                                                   \
    asm volatile(                                                                \
            "mov %3, %%ebx\n"                                                    \
            "mov %2, %%eax\n"                                                    \
            "cdq\n"                                                              \
            "idiv %%ebx\n"                                                       \
            "mov %%eax, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define divuw(arg0, arg1, arg2)                                                  \
    asm volatile(                                                                \
            "mov %3, %%ebx\n"                                                    \
            "mov %2, %%eax\n"                                                    \
            "xor %%edx, %%edx\n"                                                 \
            "div %%ebx\n"                                                        \
            "mov %%eax, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define andw(arg0, arg1, arg2)                                                   \
    asm volatile(                                                                \
            "mov %3, %%ebx\n"                                                    \
            "and %2, %%ebx\n"                                                    \
            "mov %%ebx, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define orw(arg0, arg1, arg2)                                                    \
    asm volatile(                                                                \
            "mov %3, %%ebx\n"                                                    \
            "or %2, %%ebx\n"                                                     \
            "mov %%ebx, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define xorw(arg0, arg1, arg2)                                                   \
    asm volatile(                                                                \
            "mov %3, %%ebx\n"                                                    \
            "xor %2, %%ebx\n"                                                    \
            "mov %%ebx, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1), "m"(arg2) : EBX, ECX     \
       )
#define negw(arg0, arg1)                                                         \
    asm volatile(                                                                \
            "mov %2, %%ebx\n"                                                    \
            "neg %%ebx\n"                                                        \
            "mov %%ebx, %0\n"                                                    \
            "pushf\n"                                                            \
            POP_ECX                                                              \
            "movl %%ecx, %1\n"                                                   \
            : "=m"(arg0), "=m"(HOST_FLAGS) : "m"(arg1) : EBX, ECX                \
       )


// ------------------------------------- INTEGER ARITHMETIC -------------------------
// mnemonics :
//             add    ( add., addo, addo. )
//             addc   ( addc., addco, addco. )
//             adde   ( adde., addeo, addeo. )
//             addi,
//             addic  ( addic. )
//             addis,
//             addme  ( addme., addmeo, addmeo. )
//             addze  ( addze., addzeo, addzeo. )
//             subf   ( subf., subfo, subfo. )
//             subfc  ( subfc., subfco, subfco. )
//             subfe  ( subfe., subfeo, subfeo. )
//             subfic,
//             subfme ( subfme., subfmeo, subfmeo. )
//             subfze ( subfze., subfzeo, subfzeo. )
//             divw   ( divw., divwo, divwo. )
//             divwu  ( divwu., divwuo, divwuo. )

X(add)
{
#define add_code(rD, rA, rB)           \
    addw(rD, rA, rB);

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
    if(rB){ divw(rD, rA, rB); }

    divw_code(REG0, REG1, REG2);
}
X(divw.)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X(divwo)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
}
X(divwo.)
{
    divw_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
    if(REG2 == 0 || (((REG1 & 0xffffffff) == 0x80000000) && REG2 == 0xffffffff)){ UPDATE_SO_OV_V(1); }
}

X(divwu)
{
#define divwu_code(rD, rA, rB)         \
    if(rB){ divuw(rD, rA, rB); }

    divwu_code(REG0, REG1, REG2);
}
X(divwu.)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X(divwuo)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }   // Set OV=1 if division by zero
}
X(divwuo.)
{
    divwu_code(REG0, REG1, REG2);
    UPDATE_SO_OV();
    UPDATE_CR0();
    if(REG2 == 0){ UPDATE_SO_OV_V(1); }
}


// ---------------------------------- INTEGER LOGICAL -------------------------------

/* and variants */
X(and)
{
#define and_code(rA, rS, rB)             \
    andw(rA, rS, rB)

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

/* andi_dot:  AND immediate, update CR.
 * rA <- rS + 16(0) || UIMM
 */
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
    uint32_t tmp = rS;                          \
    int i;                                      \
    for (i=0; i<32; i++) {                      \
        if (tmp & 0x80000000)                   \
            break;                              \
        tmp <<= 1;                              \
    }                                           \
    rA = i;

    cntlzw_code(REG0, REG1);
}
X(cntlzw.)
{
    cntlzw_code(REG0, REG1);
    UPDATE_CR0_V(i);
}

X(nand)
{
#define nand_code(rA, rS, rB)                  \
    and_code(rA, rS, rB);                      \
    rA = ~rA

    nand_code(REG0, REG1, REG2);
}
X(nand.)
{
    nand_code(REG0, REG1, REG2);
    UPDATE_CR0();
}
X(neg)
{
#define neg_code(rD, rA)                        \
    negw(rD, rA);

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
    orw(rA, rS, rB);                            \
    rA = ~rA

    nor_code(REG0, REG1, REG2);
}
X(nor.)
{
    nor_code(REG0, REG1, REG2);
    UPDATE_CR0();
}

X(or)
{
#define or_code(rA, rS, rB)                     \
    orw(rA, rS, rB)

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
    orw(rA, rS, tmp)

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
    xorw(rA, rS, rB)

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

// --------------------------------- branch -----------------------------------------
// branch unconditional
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
    int ctr_ok  = BO2(BO) | ((((CM) ? CTR: (CTR & 0xffffffff)) != 0) ^ BO3(BO));     \
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
    int ctr_ok = BO2(BO) | ((((CM) ? CTR: (CTR & 0xffffffff)) != 0) ^ BO3(BO));     \
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


// ------------------------------------- INTEGER COMPARE ------------------------------------
/* cmp variants */
// cmp crD, L, rA, rB 
X(cmp)
{
#define cmp_code(crD, L, rA, rB)                                       \
    unsigned res = 0;                                                  \
    switch(L){                                                         \
        case 0:                                                        \
            if((int32_t)rA > (int32_t)rB){ res |= 0x2; }               \
            else if((int32_t)rA < (int32_t)rB){ res |= 0x4; }          \
            else { res |= 0x1; }                                       \
            break;                                                     \
        case 1:                                                        \
            if((int64_t)rA > (int64_t)rB){ res |= 0x2; }               \
            else if((int64_t)rA < (int64_t)rB){ res |= 0x4; }          \
            else { res |= 0x1; }                                       \
            break;                                                     \
    }                                                                  \
    update_crF(crD, (res << 1));                                       \
    update_crf((crD*4+3), get_xer_so());

    cmp_code(ARG0, ARG1, REG2, REG3);
}
X(cmpi)
{
#define cmpi_code(crD, L, rA, SIMM)                                    \
    unsigned res = 0;                                                  \
    switch(L){                                                         \
        case 0:                                                        \
            if((int32_t)rA > (int32_t)SIMM){ res |= 0x2; }             \
            else if((int32_t)rA < (int32_t)SIMM){ res |= 0x4; }        \
            else { res |= 0x1; }                                       \
            break;                                                     \
        case 1:                                                        \
            if((int64_t)rA > (int64_t)SIMM){ res |= 0x2; }             \
            else if((int64_t)rA < (int64_t)SIMM){ res |= 0x4; }        \
            else { res |= 0x1; }                                       \
            break;                                                     \
    }                                                                  \
    update_crF(ARG0, (res << 1));                                      \
    update_crf((ARG0*4+3), get_xer_so());

    cmpi_code(ARG0, ARG1, REG2, ARG3);
}
X(cmpl)
{
#define cmpl_code(crD, L, rA, rB)                                      \
    unsigned res = 0;                                                  \
    switch(L){                                                         \
        case 0:                                                        \
            if((uint32_t)rA > (uint32_t)rB){ res |= 0x2; }             \
            else if((uint32_t)rA < (uint32_t)rB){ res |= 0x4; }        \
            else { res |= 0x1; }                                       \
            break;                                                     \
        case 1:                                                        \
            if((uint64_t)rA > (uint64_t)rB){ res |= 0x2; }             \
            else if((uint64_t)rA < (uint64_t)rB){ res |= 0x4; }        \
            else { res |= 0x1; }                                       \
            break;                                                     \
    }                                                                  \
    update_crF(ARG0, (res << 1));                                      \
    update_crf((ARG0*4+3), get_xer_so());

    cmpl_code(ARG0, ARG1, REG2, REG3);
}
X(cmpli)
{
#define cmpli_code(crD, L, rA, UIMM)                                   \
    unsigned res = 0;                                                  \
    switch(L){                                                         \
        case 0:                                                        \
            if((uint32_t)rA > (uint32_t)UIMM){ res |= 0x2; }           \
            else if((uint32_t)rA < (uint32_t)UIMM){ res |= 0x4; }      \
            else { res |= 0x1; }                                       \
            break;                                                     \
        case 1:                                                        \
            if((uint64_t)rA > (uint64_t)UIMM){ res |= 0x2; }           \
            else if((uint64_t)rA < (uint64_t)UIMM){ res |= 0x4; }      \
            else { res |= 0x1; }                                       \
            break;                                                     \
    }                                                                  \
    update_crF(ARG0, (res << 1));                                      \
    update_crf((ARG0*4+3), get_xer_so());

    cmpli_code(ARG0, ARG1, REG2, ARG3);
}


// ------------------------- CONDITION REGISTER LOGICAL -------------------------
// condition register instrs
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

// ----------------------------------- SYSTEM ------------------------------
X(isync)
{
    // Do nothing
}

// Barrier
X(mbar)
{
    //Do nothing
}


X(mcrxr)
{
#define mcrxr_code(crfD)                 \
    update_crF(crfD, get_xerF(0));

    mcrxr_code(ARG0);
}
X(mfcr)
{
#define mfcr_code(rD)                               \
    rD = (CR & (uint64_t)0xffffffff);

    mfcr_code(ARG0);
}
X(mtcrf)
{
#define mtcrf_code(CRM, rS)                         \
    uint64_t mask = 0, i;                           \
    uint8_t tmp = CRM;                              \
    for(i=0; i<8; i++){                             \
        mask |= (tmp & 0x80)?(0xf << (7 - i)*4):0;  \
        tmp <<= 1;                                  \
    }                                               \
    CR = ((rS & mask) | (CR & ~mask));

    mtcrf_code(ARG0, REG1);
}

// Move to/from MSR
X(mfmsr)
{
    REG0 = MSR;
}
X(mtmsr)
{
    MSR = REG0;
}

// Move to/from SPR
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

    mtspr_code(ARG0, REG1);
}

// Move to/from PMR
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


X(msync)
{
    // Do nothing
}
X(sync)
{
    // Do nothing ( same as msync )
}



// Return from Interrupt
X(rfi)
{
}
X(rfci)
{
}
X(rfmci)
{
}

// System call
X(sc)
{
}

// wrtee variants
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

// -------------------------- INTEGER SHIFT AND ROTATE -----------------------------
X(rlwimi)
{
#define rlwimi_code(rA, rS, SH, MB, ME)                                            \
    uint32_t tmp = (rS << (SH & 0x1f)) | (rS >> (32 - (SH & 0x1f)));               \
    uint32_t mask = (MB <= ME) ? (((1L << (ME - MB + 1)) - 1) << (31 - ME)) :      \
                     ~(((1L << (MB - ME + 1)) - 1) << (31 - MB));                  \
    rA = (tmp & mask) | (rA & ~mask);

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
    uint32_t tmp = (rS << (SH & 0x1f)) | (rS >> (32 - (SH & 0x1f)));               \
    uint32_t mask = (MB <= ME) ? (((1L << (ME - MB + 1)) - 1) << (31 - ME)) :      \
                     ~(((1L << (MB - ME + 1)) - 1) << (31 - MB));                  \
    rA = (tmp & mask);

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
    uint32_t tmp = (rS << (rB & 0x1f)) | (rS >> (32 - (rB & 0x1f)));               \
    uint32_t mask = (MB <= ME) ? (((1L << (ME - MB + 1)) - 1) << (31 - ME)) :      \
                     ~(((1L << (MB - ME + 1)) - 1) << (31 - MB));                  \
    rA = (tmp & mask);

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
    uint32_t tmp = (rS << (rB & 0x1f)) | (rS >> (32 - (rB & 0x1f)));               \
    uint32_t mask = (((rB >> 5) & 0x1) == 0) ? ~((1L << (rB & 0x1f)) - 1) : 0;     \
    rA = (tmp & mask);

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
    uint64_t n = rB & 0x1f;                                                              \
    uint64_t tmp = ROTL32(B_32_63(rS), (64 - n));                                        \
    uint64_t mask;                                                                       \
    if(((rB >> 5) & 0x1) == 0)  mask = MASK(n+32, 63);                                   \
    else                        mask = 0;                                                \
    int sign = ((rS >> 31) & 0x1);                                                       \
    rA = (tmp & mask) | (((sign) ? 0xffffffffffffffffL : 0L) & ~mask);                   \
    update_xer_ca(sign & ((tmp & ~mask) != 0));

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
    uint64_t n = SH & 0x1f;                                                              \
    uint64_t tmp = ROTL32(B_32_63(rS), (64 - n));                                        \
    uint64_t mask = MASK(n+32, 63);                                                      \
    int sign = ((rS >> 31) & 0x1);                                                       \
    rA = (tmp & mask) | (((sign) ? 0xffffffffffffffffL : 0L) & ~mask);                   \
    update_xer_ca(sign & (B_32_63(tmp & ~mask) != 0));

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
    uint64_t n = rB & 0x1f;                                                              \
    uint64_t tmp = ROTL32(B_32_63(rS), (64 - n));                                        \
    uint64_t mask;                                                                       \
    if (((rB >> 5) & 0x1) == 0)   mask = MASK(n+32, 63);                                 \
    else                          mask = 0;                                              \
    rA = (tmp & mask);

    srw_code(REG0, REG1, REG2);
}
X(srw.)
{
    srw_code(REG0, REG1, REG2);
    UPDATE_CR0_V(UT(REG0));
}

// ------------------------------ load/store ---------------------------------------
// mnemonics : lbz, lbzu, lbzux, lbzx, lha, lhau, lhaux, lhax, lhbrx, lhz,
//             lhzu, lhzux, lhzx, lmw, lwarx, lwbrx, lwz, lwzu, lwzux, lwzx,
//             stb, stbu, stbux, stbx, sth, sthbrx, sthu, sthux, sthx, stmw,
//             stw, stwbrx, stwcx., stwu, stwux, stwx

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

// ------------------------------ TLB ----------------------------------------------
X(tlbwe)
{
    TLBWE();
}

// ------------------------------ SPE -----------------------------------------------
X(brinc)
{
    // Not implemented right now
}

X(evxor)
{
    REG0 = REG1 ^ REG2;
}
