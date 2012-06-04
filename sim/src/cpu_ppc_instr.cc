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
 */

/* ---------
 * Defines 
 * ---------
 */

// Global defines for this template file

#define spr                      cpu->spr
#define xer                      spr[SPRN_XER]
#define gpr                      cpu->gpr
#define pmr                      cpu->pmr
#define msr                      cpu->msr
#define cr                       cpu->cr
#define m_reghash                cpu->m_reghash
#define update_cr0               cpu->update_cr0
#define update_crF               cpu->update_crF
#define update_crf               cpu->update_crf
#define update_xer               cpu->update_xer
#define update_xerf              cpu->update_xerf
#define get_xer_so               cpu->get_xer_so
#define get_crf                  cpu->get_crf
#define get_crF                  cpu->get_crF
#define get_xerF                 cpu->get_xerF
#define get_cr                   cpu->get_cr
#define set_cr                   cpu->set_cr
#define host_flags               cpu->host_state.flags
#define dummy_flags              cpu->host_state.dummy

// In case of register operands, their pointers are instead passed in corresponding ARG parameter.
#define REG0   reg(ARG0)
#define REG1   reg(ARG1)
#define REG2   reg(ARG2)
#define REG3   reg(ARG3)
#define REG4   reg(ARG4)

// target mode , ut -> unsigned target, st -> signed target
// If target_bits == 32
//     UMODE -> uint32_t
//     SMODE -> int32_t
// else if target_bits == 64
//     UMODE -> uint64_t
//     SMODE -> int64_t
// endif    
#define ut(arg)   ((UMODE)(arg))
#define st(arg)   ((SMODE)(arg))

#if 0
#ifndef CHECK_FOR_FPU_EXCEPTION
#define CHECK_FOR_FPU_EXCEPTION { if (!(cpu->cd.ppc.msr & PPC_MSR_FP)) { \
		/*  Synchronize the PC, and cause an FPU exception:  */  \
		uint64_t low_pc = ((size_t)ic -				 \
		    (size_t)cpu->cd.ppc.cur_ic_page)			 \
		    / sizeof(struct ppc_instr_call);			 \
		cpu->pc = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) <<	 \
		    PPC_INSTR_ALIGNMENT_SHIFT)) + (low_pc <<		 \
		    PPC_INSTR_ALIGNMENT_SHIFT);				 \
		ppc_exception(cpu, PPC_EXCEPTION_FPU);			 \
		return; } }
#endif
#endif

/*----------------------------------------------------------------------------
 * x86/x86_64 assembly.
 * Instead of calculating Overflow, carry bits etc, we are leveraging host flag
 * register directly.
 * ---------------------------------------------------------------------------*/
/*
 * NOTE: All operands are memory operands, so that pointers could also be
 *       used as arguments
 *
 * add
 * arg0 -> result
 * arg1 -> source
 * arg2 -> source
 *
 * d -> double word ( = 64 bits )
 * w -> word ( = 32 bits )
 *
 * flags -> x86_64 %rflags
 */
#define addd_x64(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movq %3, %%r15\n"                 \
            "addq %2, %%r15\n"                 \
            "movq %%r15, %0\n"                 \
            "pushfq\n"                         \
            "pop %%r14\n"                      \
            "movq %%r14, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%r14", "%r15"                   \
       )
#define addw_x86(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movl %3, %%ebx\n"                 \
            "addl %2, %%ebx\n"                 \
            "movl %%ebx, %0\n"                 \
            "pushfl\n"                         \
            "pop %%ecx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%ebx", "%ecx"                   \
       )
/* 32 bit add on x86_64 */
#define addw_x64(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movl %3, %%ebx\n"                 \
            "addl %2, %%ebx\n"                 \
            "movl %%ebx, %0\n"                 \
            "pushfq\n"                         \
            "pop %%rcx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%rbx", "%rcx"                   \
       )
/*
 * sub
 *
 * arg0 <- arg1 - arg2
 */
#define subd_x64(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movq %2, %%r15\n"                 \
            "subq %3, %%r15\n"                 \
            "movq %%r15, %0\n"                 \
            "pushfq\n"                         \
            "pop %%r14\n"                      \
            "movq %%r14, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%r14", "%r15"                   \
       )
#define subw_x86(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movl %2, %%ebx\n"                 \
            "subl %3, %%ebx\n"                 \
            "movl %%ebx, %0\n"                 \
            "pushfl\n"                         \
            "pop %%ecx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%ebx", "%ecx"                   \
       )
/* 32 bit subtract on x86_64 */
#define subw_x64(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movl %2, %%ebx\n"                 \
            "subl %3, %%ebx\n"                 \
            "movl %%ebx, %0\n"                 \
            "pushfq\n"                         \
            "pop %%rcx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%rbx", "%rcx"                   \
       )

// Negate
#define negd_x64(arg0, arg1, flags)            \
    asm(                                       \
            "movq %2, %%r15\n"                 \
            "negq %%r15\n"                     \
            "movq %%r15, %0\n"                 \
            "pushfq\n"                         \
            "pop %%r14\n"                      \
            "movq %%r14, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1)                        \
            : "%r14", "%r15"                   \
       )
#define negw_x64(arg0, arg1, flags)            \
    asm(                                       \
            "movl %2, %%ebx\n"                 \
            "negl %%ebx\n"                     \
            "movl %%ebx, %0\n"                 \
            "pushfq\n"                         \
            "pop %%rcx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1)                        \
            : "%rbx", "%rcx"                   \
       )
#define negw_x86(arg0, arg1, arg2, flags)      \
    asm(                                       \
            "movl %2, %%ebx\n"                 \
            "negl %%ebx\n"                     \
            "movl %%ebx, %0\n"                 \
            "pushfl\n"                         \
            "pop %%ecx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%ebx", "%ecx"                   \
       )


/* Use host and target identification to properly select the add variant */
/*
 *  These are not correct for time being.
 *  factors on which it all depends :-
 *  1. 64 bits target on 32 bits host ( only full emulation )
 *  2. 64 bits target on 64 bits host ( partial emulation )
 *  3. 32 bits target on 32 bits hist ( partial emulation )
 *  4. 32 bits target on 64 bits host ( partial emulation ) :- Not started yet.
 *
 *  host -> host machine's cpu ( x86 for eg. )
 *  target -> target machine's cpu ( ppc in our case )
 */

#if HOST_ARCH == x86_64

#define addw   addw_x64 
#define subw   subw_x64
#define negw   negw_x64

#elif HOST_ARCH == i686

#define addw   addw_x86
#define subw   subw_x86
#define negw   negw_x86

#endif

/* invalid:  To catch bugs. */
X(invalid)
{
    cerr << "PPC: invalid(): INTERNAL ERROR\n";
    exit(1);
}

/*----------------------------------------------------------------------------------------------------*/

/*
 * add: Add variants.
 *
 * arg[0] = pointer to uint64_t target
 * arg[1] = pointer to uint64_t source0
 * arg[2] = pointer to uint64_t source1
 *
 * update_* functions update appropriate registers ( CR/xer ) via x86 flag register
 */
X(add)
{
#define add_code(rD, rA, rB)           \
    addw(rD, rA, rB, host_flags);

    add_code(REG0, REG1, REG2);
}
X(add.)
{
    add_code(REG0, REG1, REG2);
    update_cr0(1);
}
X(addo)
{
    add_code(REG0, REG1, REG2);
    update_xer(1);
}
X(addo.)
{
    add_code(REG0, REG1, REG2);
    update_xer(1);
    update_cr0(1);
}
/* Not sure if there's a diff between add and addc 
 * ( the pseudocode for both are exactly same )
 * */
X(addc)
{
    add_code(REG0, REG1, REG2);
}
X(addc.)
{
    add_code(REG0, REG1, REG2);
    update_cr0(1);
}
X(addco)
{
    add_code(REG0, REG1, REG2);
    update_xer(1);
}
X(addco.)
{
    add_code(REG0, REG1, REG2);
    update_xer(1);
    update_cr0(1);
}
/* Add extended : rA + rB + CA */
X(adde)
{
    UMODE tmp = REG2 + ((xer >> XER_CA_SHIFT) & XER_CA);
    add_code(REG0, REG1, tmp);
}
X(adde.)
{
    UMODE tmp = REG2 + ((xer >> XER_CA_SHIFT) & XER_CA);
    add_code(REG0, REG1, tmp);
    update_cr0(1);
}
X(addeo)
{
    UMODE tmp = REG2 + ((xer >> XER_CA_SHIFT) & XER_CA);
    add_code(REG0, REG1, tmp);
    update_xer(1);
}
X(addeo.)
{
    UMODE tmp = REG2 + ((xer >> XER_CA_SHIFT) & XER_CA);
    add_code(REG0, REG1, tmp);
    update_xer(1);
    update_cr0(1);
}
/*
 *  addi:  Add immediate.
 *
 *  arg[0] = pointer to destination uint64_t
 *  arg[1] = pointer to source uint64_t
 *  arg[2] = immediate value (int16_t or larger)
 *
 *  Also flags(CR) shouldn't change on just assignment
 *  
 */
X(addi)
{
    SMODE tmp = (int16_t)ARG2;
    if(ARG1){ add_code(REG0, REG1, tmp); }
    else    { REG0 = (int16_t)ARG2;             }
}
X(li)
{
    REG0 = (int16_t)ARG1;
}
X(la)
{
    SMODE tmp = (int16_t)ARG1;
    if(ARG2){ add_code(REG0, REG2, tmp); }
    else    { REG0 = (int16_t)ARG1;             }
}
X(subi)
{
    SMODE tmp = (int16_t)ARG2;
    if(ARG1){ subw(REG0, REG1, tmp, host_flags); }
    else    { REG0 = -(int16_t)ARG2;            }
}
X(li_0)
{
    REG0 = 0;
}
/*  addic:  Add immediate, Carry.
 *
 *  arg[0] = pointer to dest register
 *  arg[1] = pointer to source register
 *  arg[1] = immediate value (int16_t )
 */
/* Don't understand if there's any diff. between addi and addic
 * (pseudocode for both are same)
 */
X(addic)
{
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    update_xer(1);
}
X(subic)
{
    SMODE tmp = (int16_t)ARG2;
    subw(REG0, REG1, tmp, host_flags);
    update_xer(1);
}
X(addic.)
{
    SMODE tmp = (int16_t)ARG2;
    add_code(REG0, REG1, tmp);
    update_xer(1);
    update_cr0(1);
}
X(subic.)
{
    SMODE tmp = (int16_t)ARG2;
    subw(REG0, REG1, tmp, host_flags);
    update_xer(1);
    update_cr0(1);
}
/*
 *
 * addis: Add immediate shifted.
 *
 * arg[0] = pointer to uint64_t target
 * arg[1] = pointer to uint64_t source
 * arg[2] = int16_t immediate value
 */
X(addis)
{
    SMODE tmp = (((int16_t)ARG2) << 16);
    if(ARG1){ add_code(REG0, REG1, tmp); }
    else    { REG0 = ((int16_t)ARG2) << 16;     }
}
X(lis)
{
    REG0 = ((int16_t)ARG1) << 16;
}
X(subis)
{
    SMODE tmp = (((int16_t)ARG2) << 16);
    if(ARG1){ subw(REG0, REG1, tmp, host_flags); }
    else    { REG0 = -((int16_t)ARG2 << 16);    }
}
/* addme
 * rA + xer[CA] - 1  
 *  */
X(addme)
{
    SMODE tmp = ((xer >> XER_CA_SHIFT) & XER_CA) - 1;
    add_code(REG0, REG1, tmp);
}
X(addme.)
{
    SMODE tmp = ((xer >> XER_CA_SHIFT) & XER_CA) - 1;
    add_code(REG0, REG1, tmp);
    update_cr0(1);
}
X(addmeo)
{
    SMODE tmp = ((xer >> XER_CA_SHIFT) & XER_CA) - 1;
    add_code(REG0, REG1, tmp);
    update_xer(1);
}
X(addmeo.)
{
    SMODE tmp = ((xer >> XER_CA_SHIFT) & XER_CA) - 1;
    add_code(REG0, REG1, tmp);
    update_xer(1);
    update_cr0(1);
}
/*
 * addze
 * rA + xer[CA]
 */
X(addze)
{
    UMODE tmp = (xer >> XER_CA_SHIFT) & XER_CA;
    add_code(REG0, REG1, tmp);
}
X(addze.)
{
    UMODE tmp = (xer >> XER_CA_SHIFT) & XER_CA;
    add_code(REG0, REG1, tmp);
    update_cr0(1);
}
X(addzeo)
{
    UMODE tmp = (xer >> XER_CA_SHIFT) & XER_CA;
    add_code(REG0, REG1, tmp);
    update_xer(1);
}
X(addzeo.)
{
    UMODE tmp = (xer >> XER_CA_SHIFT) & XER_CA;
    add_code(REG0, REG1, tmp);
    update_xer(1);
    update_cr0(1);
}

/* and variants */
X(and)
{
    UMODE tmp = REG1 & REG2;
    REG0 = tmp;
}
X(and.)
{
    REG0 = REG1 & REG2;
    update_cr0(0, ut(REG0));
}
X(andc)
{
    UMODE tmp = REG1 & (~REG2);
    REG0 = tmp;
}
X(andc.)
{
    REG0 = REG1 & (~REG2);
    update_cr0(0, ut(REG0));
}
/* andi_dot:  AND immediate, update CR.
 * rA <- rS + 16(0) || UIMM
 */
X(andi.)
{
    REG0 = REG1 & (uint16_t)ARG2;
    update_cr0(0, ut(REG0));
}
X(andis.)
{
    REG0 = REG1 & (((uint16_t)ARG2) << 16);
    update_cr0(0, ut(REG0));
}

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
X(cmplw)
{
    cmpl_code(ARG0, 0, REG1, REG2);
}
X(cmplwi)
{
    cmpli_code(ARG0, 0, REG1, ARG2);
}
X(cmpw)
{
    cmp_code(ARG0, 0, REG1, REG2);
}
X(cmpwi)
{
    cmpi_code(ARG0, 0, REG1, ARG2);
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
    update_cr0(0,i);
}

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
X(crnot)
{
    crnor_code(ARG0, ARG1, ARG1);
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
X(crset)
{
    creqv_code(ARG0, ARG0, ARG0);
}
X(crxor)
{
#define crxor_code(crD, crA, crB)                        \
    update_crf(crD, (get_crf(crA) ^ get_crf(crB)));

    crxor_code(ARG0, ARG1, ARG2);
}
X(crmove)
{
    cror_code(ARG0, ARG1, ARG1);
}
X(crclr)
{
    crxor_code(ARG0, ARG0, ARG0);
}

// Barrier
X(mbar)
{
    //Do nothing
}

X(mcrf)
{
#define mcrf_code(crfD, crfS)            \
    update_crF(crfD, get_crF(crfS));

    mcrf_code(ARG0, ARG1);
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
    rD = (get_cr() & (uint64_t)0xffffffff);

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
    set_cr((rS & mask) | (get_cr() & ~mask));

    mtcrf_code(ARG0, REG1);
}
X(mtcr)
{
    mtcrf_code(0xff, REG0);
}

//  Move to/from MSR
X(mfmsr)
{
#define mfmsr_code(rD)                         \
    rD = msr;

    mfmsr_code(REG0);
}
X(mfpmr)
{
#define mfpmr_code(rD, PMRN)                  \
    rD = pmr[PMRN];

    mfpmr_code(REG0, ARG1);
}

// Move to/from SPRs referred to by spr name in mnemonic
#define MNEC_MFR(rD, regname)                 \
    X(mf##regname){                           \
        rD = *(m_reghash[#regname]);          \
    }
#define MNEC_MTR(regname, rS)                 \
    X(mt##regname){                           \
        *(m_reghash[#regname]) = rS;          \
    }

MNEC_MFR(REG0, xer)
MNEC_MFR(REG0, ctr)

MNEC_MTR(xer, REG0)
MNEC_MTR(ctr, REG0)

X(mfspr)
{
#define mfspr_code(rD, SPRN)                   \
    rD = spr[SPRN];

    mfspr_code(REG0, ARG1);
}
// Move to MSR
X(mtmsr)
{
#define mtmsr_code(rS)                        \
    msr = rS;

    mtmsr_code(REG0);
}
// Move to PMR
X(mtpmr)
{
#define mtpmr_code(PMRN, rS)                  \
    pmr[PMRN] = rS;

    mtpmr_code(ARG0, REG1);
}
X(mtspr)
{
#define mtspr_code(SPRN, rS)                  \
    spr[SPRN] = rS;

    mtspr_code(ARG0, REG1);
}

// Move register
X(mr)
{
}
X(msync)
{
    // Do nothing
}

// Add all mul* instrs here
//
//

// Logical instrs
X(nand)
{
#define nand_code(rA, rS, rB)                  \
    rA = ~(rS & rB);

    nand_code(REG0, REG1, REG2);
}
X(nand.)
{
    nand_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
X(neg)
{
#define neg_code(rD, rA)                        \
    negw(rD, rA, host_flags);

    neg_code(REG0, REG1);
}
X(neg.)
{
    neg_code(REG0, REG1);
    update_cr0(1);
}
X(nego)
{
    neg_code(REG0, REG1);
    update_xer(1);
}
X(nego.)
{
    neg_code(REG0, REG1);
    update_xer(1);
    update_cr0(1);
}
X(nor)
{
#define nor_code(rA, rS, rB)                    \
    rA = ~(rS | rB);

    nor_code(REG0, REG1, REG2);
}
X(nor.)
{
    nor_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
X(or)
{
#define or_code(rA, rS, rB)                     \
    rA = rS | rB;

    or_code(REG0, REG1, REG2);
}
X(or.)
{
    or_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
X(orc)
{
#define orc_code(rA, rS, rB)                     \
    rA = rS | ~(rB);

    orc_code(REG0, REG1, REG2);
}
X(orc.)
{
    orc_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
X(ori)
{
#define ori_code(rA, rS, UIMM)                 \
    rA = rS | (uint16_t)UIMM;

    ori_code(REG0, REG1, ARG2);
}
X(oris)
{
#define oris_code(rA, rS, UIMM)                \
    rA = rS | (((uint16_t)UIMM) << 16);

    oris_code(REG0, REG1, ARG2);
}
X(nop)
{
    // Do nothing
}
X(not)
{
    nor_code(REG0, REG1, REG1);
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


// Rotate intrs
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
    update_cr0(0, ut(REG0));
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
    update_cr0(0, ut(REG0));
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
    update_cr0(0, ut(REG0));
}
X(rotlw)
{
    rlwnm_code(REG0, REG1, REG2, 0, 31);
}
X(rotlwi)
{
    rlwinm_code(REG0, REG1, ARG2, 0, 31);
}
X(rotrwi)
{
    rlwinm_code(REG0, REG1, (32-ARG2), 0, 31);
}

// System call
X(sc)
{
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
    update_cr0(0, ut(REG0));
}
// slwi rA, rS, n; ( n < 32 )
X(slwi)
{
    rlwinm_code(REG0, REG1, ARG2, 0, (31-ARG2));
}
X(sraw)
{
#define sraw_code(rA, rS, rB)                                                            \
    uint32_t tmp = (rS >> (rB & 0x1f)) | (rS << (32 - (rB & 0x1f)));                     \
    uint64_t mask = (((rB >> 5) & 0x1) == 0) ? ~((1L << (32 - (rB & 0x1f))) - 1) : 0;    \
    int sign = ((rS >> 31) & 0x1);                                                       \
    rA = (tmp & mask) | (((sign) ? 0xffffffffffffffffL : 0L) & ~mask);                   \
    update_xerf(0, (sign & ((tmp & ~mask) != 0)));

    sraw_code(REG0, REG1, REG2);
}
X(sraw.)
{
    sraw_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
X(srawi)
{
#define srawi_code(rA, rS, SH)                                                           \
    uint32_t tmp = (rS >> (SH & 0x1f)) | (rS << (32 - (SH & 0x1f)));                     \
    uint64_t mask = ~((1L << (32 - (SH & 0x1f))) - 1);                                   \
    int sign = ((rS >> 31) & 0x1);                                                       \
    rA = (tmp & mask) | (((sign) ? 0xffffffffffffffffL : 0L) & ~mask);                   \
    update_xerf(0, (sign & ((tmp & ~mask) != 0)));

    srawi_code(REG0, REG1, ARG2);
}
X(srawi.)
{
    srawi_code(REG0, REG1, ARG2);
    update_cr0(0, ut(REG0));
}
X(srw)
{
#define srw_code(rA, rS, rB)                                                             \
    uint32_t tmp = (rS >> (rB & 0x1f)) | (rS << (32 - (rB & 0x1f)));                     \
    uint32_t mask = (((rB >> 5) & 0x1) == 0) ? ~((1L << (32 - (rB & 0x1f))) - 1) : 0;    \
    rA = (tmp & mask);

    srw_code(REG0, REG1, REG2);
}
X(srw.)
{
    srw_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
// srwi rA, rS, n; ( n < 32 )
X(srwi)
{
    rlwinm_code(REG0, REG1, (32-ARG2), ARG2, 31);
}

// wrtee variants
X(wrtee)
{
#define wrtee_code(rS)            \
    msr &= ~(1L << 15);           \
    msr |= rS & (1L << 15);

    wrtee_code(REG0);
}
X(wrteei)
{
#define wrteei_code(E)            \
    msr &= ~(1 << 15);            \
    msr |= ((E & 0x1) << 15);

    wrteei_code(ARG0);
}

// xor variants
X(xor)
{
#define xor_code(rA, rS, rB)      \
    rA = rS ^ rB;

    xor_code(REG0, REG1, REG2);
}
X(xor.)
{
    xor_code(REG0, REG1, REG2);
    update_cr0(0, ut(REG0));
}
X(xori)
{
#define xori_code(rA, rS, UIMM)   \
    rA = rS ^ ((uint16_t)UIMM);

    xori_code(REG0, REG1, ARG2);
}
X(xoris)
{
#define xoris_code(rA, rS, UIMM)            \
    rA = rS ^ (((uint16_t)UIMM) << 16);

    xoris_code(REG0, REG1, REG2);
}
