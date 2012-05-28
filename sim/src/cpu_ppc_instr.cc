/*
 *  Copyright (C) 2005-2009  Anders Gavare.  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright  
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE   
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *
 *  POWER/PowerPC instructions.
 *
 *  Individual functions should keep track of cpu->n_translated_instrs.
 *  (If no instruction was executed, then it should be decreased. If, say, 4
 *  instructions were combined into one function and executed, then it should
 *  be increased by 3.)
 *
 *  -----------------------------------------------------------------------------------
 *  Modified heavily by Vikas Chouhan ( presentisgood@gmail.com )
 *  copyright (C) 2012.
 *  
 *  All instructions have been heavily conditioned for Freescale ppc processors.
 *  e500v2 is taken as a reference, but the code is generic and can be used for higher
 *  64 bit cores.
 *
 *  NOTE:
 *  This file is not a regular C/C++ file. It's just a template for defining instructions'
 *  pseudocode implementation. An external utility uses this template to generate a new
 *  header/C++ file before direct use.
 */

/* ---------
 * Defines 
 * ---------
 */

// SPR, GPR, ARG0, ARG1, etc should come from outside ( preferably some top level utility )
//
#define XER         SPR[SPRN_XER]

#define REG0   reg(ARG0)
#define REG1   reg(ARG1)
#define REG2   reg(ARG2)
#define REG3   reg(ARG3)
#define REG4   reg(ARG4)

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
 * flags -> x86_64 %rflags
 */
#define ADD_X64(arg0, arg1, arg2, flags)       \
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
#define ADD_X86(arg0, arg1, arg2, flags)       \
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
#define ADD_X86_X64(arg0, arg1, arg2, flags)   \
    asm(                                       \
            "movl %3, %%ebx\n"                 \
            "addl %2, %%ebx\n"                 \
            "movl %%ebx, %0\n"                 \
            "pushf\n"                          \
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
#define SUB_X64(arg0, arg1, arg2, flags)       \
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
#define SUB_X86(arg0, arg1, arg2, flags)       \
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
#define SUB_X86_X64(arg0, arg1, arg2, flags)   \
    asm(                                       \
            "movl %2, %%ebx\n"                 \
            "subl %3, %%ebx\n"                 \
            "movl %%ebx, %0\n"                 \
            "pushf\n"                          \
            "pop %%rcx\n"                      \
            "movl %%ecx, %1\n"                 \
            : "=m"(arg0), "=m"(flags)          \
            : "m"(arg1), "m"(arg2)             \
            : "%rbx", "%rcx"                   \
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
#define ADD    ADD_X86_X64 
#define ADDI   ADDI_X86_X64
#define SUB    SUB_X86_X64    
#define SUBI   SUBI_X86_X64 



/* nop:  Do nothing. */
X(nop)
{
}
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
 * ADDR -> add record ( records status into flags )
 * ADD  -> add simple
 *
 * update_* functions update appropriate registers ( CR/XER ) via x86 flag register
 */
#define _ADDR ADD(REG0, REG1, REG2, HOST_FLAGS)
#define _ADD  ADD(REG0, REG1, REG2, DUMMY)
X(add)
{
    _ADDR;
}
X(add.)
{
    _ADDR;
    UPDATE_CR0(1);
}
X(addo)
{
    _ADDR;
    UPDATE_XER(1);
}
X(addo.)
{
    _ADDR;
    UPDATE_XER(1);
    UPDATE_CR0(1);
}
/* Not sure if there's a diff between add and addc 
 * ( the pseudocode for both are exactly same )
 * */
X(addc)
{
    _ADDR;
}
X(addc.)
{
    _ADDR;
    UPDATE_CR0(1);
}
X(addco)
{
    _ADDR;
    UPDATE_XER(1);
}
X(addco.)
{
    _ADDR;
    UPDATE_XER(1);
    UPDATE_CR0(1);
}
/* Add extended : rA + rB + CA */
X(adde)
{
    UMODE tmp = REG2 + ((XER >> XER_CA_SHIFT) & XER_CA);
    ADD(REG0, REG1, tmp, HOST_FLAGS);
}
X(adde.)
{
    UMODE tmp = REG2 + ((XER >> XER_CA_SHIFT) & XER_CA);
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_CR0(1);
}
X(addeo)
{
    UMODE tmp = REG2 + ((XER >> XER_CA_SHIFT) & XER_CA);
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
}
X(addeo.)
{
    UMODE tmp = REG2 + ((XER >> XER_CA_SHIFT) & XER_CA);
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
    UPDATE_CR0(1);
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
    if(ARG1){ ADD(REG0, REG1, tmp, HOST_FLAGS); }
    else    { REG0 = (int16_t)ARG2;             }
}
X(li)
{
	REG0 = (int16_t)ARG1;
}
X(la)
{
    SMODE tmp = (int16_t)ARG1;
    if(ARG2){ ADD(REG0, REG2, tmp, HOST_FLAGS); }
    else    { REG0 = (int16_t)ARG1;             }
}
X(subi)
{
    SMODE tmp = (int16_t)ARG2;
    if(ARG1){ SUB(REG0, REG1, tmp, HOST_FLAGS); }
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
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
}
X(subic)
{
    SMODE tmp = (int16_t)ARG2;
    SUB(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
}
X(addic.)
{
    SMODE tmp = (int16_t)ARG2;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
    UPDATE_CR0(1);
}
X(subic.)
{
    SMODE tmp = (int16_t)ARG2;
    SUB(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
    UPDATE_CR0(1);
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
    if(ARG1){ ADD(REG0, REG1, tmp, HOST_FLAGS); }
    else    { REG0 = ((int16_t)ARG2) << 16;     }
}
X(lis)
{
    REG0 = ((int16_t)ARG1) << 16;
}
X(subis)
{
    SMODE tmp = (((int16_t)ARG2) << 16);
    if(ARG1){ SUB(REG0, REG1, tmp, HOST_FLAGS); }
    else    { REG0 = -((int16_t)ARG2 << 16);    }
}
/* addme
 * rA + XER[CA] - 1  
 *  */
X(addme)
{
    SMODE tmp = ((XER >> XER_CA_SHIFT) & XER_CA) - 1;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
}
X(addme.)
{
    SMODE tmp = ((XER >> XER_CA_SHIFT) & XER_CA) - 1;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_CR0(1);
}
X(addmeo)
{
    SMODE tmp = ((XER >> XER_CA_SHIFT) & XER_CA) - 1;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
}
X(addmeo.)
{
    SMODE tmp = ((XER >> XER_CA_SHIFT) & XER_CA) - 1;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
    UPDATE_CR0(1);
}
/*
 * addze
 * rA + XER[CA]
 */
X(addze)
{
    UMODE tmp = (XER >> XER_CA_SHIFT) & XER_CA;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
}
X(addze.)
{
    UMODE tmp = (XER >> XER_CA_SHIFT) & XER_CA;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_CR0(1);
}
X(addzeo)
{
    UMODE tmp = (XER >> XER_CA_SHIFT) & XER_CA;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
}
X(addzeo.)
{
    UMODE tmp = (XER >> XER_CA_SHIFT) & XER_CA;
    ADD(REG0, REG1, tmp, HOST_FLAGS);
    UPDATE_XER(1);
    UPDATE_CR0(1);
}

/*---------------------------------------------------------------------------------------------*/
/* and variants */
X(and)
{
    UMODE tmp = REG1 & REG2;
    REG0 = tmp;
}
X(and.)
{
    UMODE tmp = REG1 & REG2;
    REG0 = tmp;
    UPDATE_CR0(0, tmp);
}
X(andc)
{
    UMODE tmp = REG1 & (~REG2);
    REG0 = tmp;
}
X(andc.)
{
    UMODE tmp = REG1 & (~REG2);
    REG0 = tmp;
    UPDATE_CR0(0, tmp);
}
/* andi_dot:  AND immediate, update CR.
 * rA <- rS + 16(0) || UIMM
 */
X(andi.)
{
	UMODE tmp = REG1 & (uint16_t)ARG2;
	REG0 = tmp;
	UPDATE_CR0(0, tmp);
}
X(andis.)
{
    UMODE tmp = REG1 & (((uint16_t)ARG2) << 16);
    REG0 = tmp;
    UPDATE_CR0(0, tmp);
}

/*-----------------------------------------------------------------------------------------------*/
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
    update_crf(crD, res);                                              \
    update_cr_f((crD*4+3), get_xer_so());

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
    update_crf(ARG0, res);                                             \
    update_cr_f((ARG0*4+3), get_xer_so());

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
    update_crf(ARG0, res);                                             \
    update_cr_f((ARG0*4+3), get_xer_so());

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
    update_crf(ARG0, res);                                             \
    update_cr_f((ARG0*4+3), get_xer_so());

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


#if 0

/*
 *  cntlzw:  Count leading zeroes (32-bit word).
 *
 *  arg[0] = ptr to rs
 *  arg[1] = ptr to ra
 */
X(cntlzw)
{
	uint32_t tmp = reg(ic->arg[0]);
	int i;
	for (i=0; i<32; i++) {
		if (tmp & 0x80000000)
			break;
		tmp <<= 1;
	}
	reg(ic->arg[1]) = i;
}


/*
 *  cmpd:  Compare Doubleword
 *
 *  arg[0] = ptr to ra
 *  arg[1] = ptr to rb
 *  arg[2] = 28 - 4*bf
 */
X(cmpd)
{
	int64_t tmp = reg(ic->arg[0]), tmp2 = reg(ic->arg[1]);
	int bf_shift = ic->arg[2], c;
	if (tmp < tmp2)
		c = 8;
	else if (tmp > tmp2)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}


/*
 *  cmpld:  Compare Doubleword, unsigned
 *
 *  arg[0] = ptr to ra
 *  arg[1] = ptr to rb
 *  arg[2] = 28 - 4*bf
 */
X(cmpld)
{
	uint64_t tmp = reg(ic->arg[0]), tmp2 = reg(ic->arg[1]);
	int bf_shift = ic->arg[2], c;
	if (tmp < tmp2)
		c = 8;
	else if (tmp > tmp2)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}


/*
 *  cmpdi:  Compare Doubleword immediate
 *
 *  arg[0] = ptr to ra
 *  arg[1] = int32_t imm
 *  arg[2] = 28 - 4*bf
 */
X(cmpdi)
{
	int64_t tmp = reg(ic->arg[0]), imm = (int32_t)ic->arg[1];
	int bf_shift = ic->arg[2], c;
	if (tmp < imm)
		c = 8;
	else if (tmp > imm)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}


/*
 *  cmpldi:  Compare Doubleword immediate, logical
 *
 *  arg[0] = ptr to ra
 *  arg[1] = int32_t imm
 *  arg[2] = 28 - 4*bf
 */
X(cmpldi)
{
	uint64_t tmp = reg(ic->arg[0]), imm = (uint32_t)ic->arg[1];
	int bf_shift = ic->arg[2], c;
	if (tmp < imm)
		c = 8;
	else if (tmp > imm)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}


/*
 *  cmpw:  Compare Word
 *
 *  arg[0] = ptr to ra
 *  arg[1] = ptr to rb
 *  arg[2] = 28 - 4*bf
 */
X(cmpw)
{
	int32_t tmp = reg(ic->arg[0]), tmp2 = reg(ic->arg[1]);
	int bf_shift = ic->arg[2], c;
	if (tmp < tmp2)
		c = 8;
	else if (tmp > tmp2)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}
X(cmpw_cr0)
{
	/*  arg[2] is assumed to be 28  */
	int32_t tmp = reg(ic->arg[0]), tmp2 = reg(ic->arg[1]);
	cpu->cd.ppc.cr &= ~(0xf0000000);
	if (tmp < tmp2)
		cpu->cd.ppc.cr |= 0x80000000;
	else if (tmp > tmp2)
		cpu->cd.ppc.cr |= 0x40000000;
	else
		cpu->cd.ppc.cr |= 0x20000000;
	cpu->cd.ppc.cr |= ((cpu->cd.ppc.spr[SPR_XER] >> 3) & 0x10000000);
}


/*
 *  cmplw:  Compare Word, unsigned
 *
 *  arg[0] = ptr to ra
 *  arg[1] = ptr to rb
 *  arg[2] = 28 - 4*bf
 */
X(cmplw)
{
	uint32_t tmp = reg(ic->arg[0]), tmp2 = reg(ic->arg[1]);
	int bf_shift = ic->arg[2], c;
	if (tmp < tmp2)
		c = 8;
	else if (tmp > tmp2)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}


/*
 *  cmpwi:  Compare Word immediate
 *
 *  arg[0] = ptr to ra
 *  arg[1] = int32_t imm
 *  arg[2] = 28 - 4*bf
 */
X(cmpwi)
{
	int32_t tmp = reg(ic->arg[0]), imm = ic->arg[1];
	int bf_shift = ic->arg[2], c;
	if (tmp < imm)
		c = 8;
	else if (tmp > imm)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}
X(cmpwi_cr0)
{
	/*  arg[2] is assumed to be 28  */
	int32_t tmp = reg(ic->arg[0]), imm = ic->arg[1];
	cpu->cd.ppc.cr &= ~(0xf0000000);
	if (tmp < imm)
		cpu->cd.ppc.cr |= 0x80000000;
	else if (tmp > imm)
		cpu->cd.ppc.cr |= 0x40000000;
	else
		cpu->cd.ppc.cr |= 0x20000000;
	cpu->cd.ppc.cr |= ((cpu->cd.ppc.spr[SPR_XER] >> 3) & 0x10000000);
}


/*
 *  cmplwi:  Compare Word immediate, logical
 *
 *  arg[0] = ptr to ra
 *  arg[1] = int32_t imm
 *  arg[2] = 28 - 4*bf
 */
X(cmplwi)
{
	uint32_t tmp = reg(ic->arg[0]), imm = ic->arg[1];
	int bf_shift = ic->arg[2], c;
	if (tmp < imm)
		c = 8;
	else if (tmp > imm)
		c = 4;
	else
		c = 2;
	/*  SO bit, copied from XER  */
	c |= ((cpu->cd.ppc.spr[SPR_XER] >> 31) & 1);
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (c << bf_shift);
}


/*
 *  dcbz:  Data-Cache Block Zero
 *
 *  arg[0] = ptr to ra (or zero)
 *  arg[1] = ptr to rb
 */
X(dcbz)
{
	MODE_uint_t addr = reg(ic->arg[0]) + reg(ic->arg[1]);
	unsigned char cacheline[128];
	size_t cacheline_size = 1 << cpu->cd.ppc.cpu_type.dlinesize;
	size_t cleared = 0;

	/*  Synchronize the PC first:  */
	cpu->pc = (cpu->pc & ~0xfff) + ic->arg[2];

	addr &= ~(cacheline_size - 1);
	memset(cacheline, 0, sizeof(cacheline));

	while (cleared < cacheline_size) {
		int to_clear = cacheline_size < sizeof(cacheline)?
		    cacheline_size : sizeof(cacheline);
#ifdef MODE32
		unsigned char *page = cpu->cd.ppc.host_store[addr >> 12];
		if (page != NULL) {
			memset(page + (addr & 0xfff), 0, to_clear);
		} else
#endif
		if (cpu->memory_rw(cpu, cpu->mem, addr, cacheline,
		    to_clear, MEM_WRITE, CACHE_DATA) != MEMORY_ACCESS_OK) {
			/*  exception  */
			return;
		}

		cleared += to_clear;
		addr += to_clear;
	}
}


/*
 *  mtfsf:  Copy FPR into the FPSCR.
 *
 *  arg[0] = ptr to frb
 *  arg[1] = mask
 */
X(mtfsf)
{
	CHECK_FOR_FPU_EXCEPTION;
	cpu->cd.ppc.fpscr &= ~ic->arg[1];
	cpu->cd.ppc.fpscr |= (ic->arg[1] & (*(uint64_t *)ic->arg[0]));
}


/*
 *  mffs:  Copy FPSCR into a FPR.
 *
 *  arg[0] = ptr to frt
 */
X(mffs)
{
	CHECK_FOR_FPU_EXCEPTION;
	(*(uint64_t *)ic->arg[0]) = cpu->cd.ppc.fpscr;
}


/*
 *  fmr:  Floating-point Move
 *
 *  arg[0] = ptr to frb
 *  arg[1] = ptr to frt
 */
X(fmr)
{
	/*
	 *  This works like a normal register to register copy, but
	 *  a) it can cause an FPU exception, and b) the move is always
	 *  64-bit, even when running in 32-bit mode.
	 */
	CHECK_FOR_FPU_EXCEPTION;
	*(uint64_t *)ic->arg[1] = *(uint64_t *)ic->arg[0];
}


/*
 *  fabs:  Floating-point Absulute Value
 *
 *  arg[0] = ptr to frb
 *  arg[1] = ptr to frt
 */
X(fabs)
{
	uint64_t v;
	CHECK_FOR_FPU_EXCEPTION;
	v = *(uint64_t *)ic->arg[0];
	*(uint64_t *)ic->arg[1] = v & 0x7fffffffffffffffULL;
}


/*
 *  fneg:  Floating-point Negate
 *
 *  arg[0] = ptr to frb
 *  arg[1] = ptr to frt
 */
X(fneg)
{
	uint64_t v;
	CHECK_FOR_FPU_EXCEPTION;
	v = *(uint64_t *)ic->arg[0];
	*(uint64_t *)ic->arg[1] = v ^ 0x8000000000000000ULL;
}


/*
 *  fcmpu:  Floating-point Compare Unordered
 *
 *  arg[0] = 28 - 4*bf  (bitfield shift)
 *  arg[1] = ptr to fra
 *  arg[2] = ptr to frb
 */
X(fcmpu)
{
	struct ieee_float_value fra, frb;
	int bf_shift = ic->arg[0], c = 0;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(*(uint64_t *)ic->arg[2], &frb, IEEE_FMT_D);
	if (fra.nan | frb.nan) {
		c = 1;
	} else {
		if (fra.f < frb.f)
			c = 8;
		else if (fra.f > frb.f)
			c = 4;
		else
			c = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= ((c&0xe) << bf_shift);
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (c << PPC_FPSCR_FPCC_SHIFT);
}


/*
 *  frsp:  Floating-point Round to Single Precision
 *
 *  arg[0] = ptr to frb
 *  arg[1] = ptr to frt
 */
X(frsp)
{
	struct ieee_float_value frb;
	float fl = 0.0;
	int c = 0;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[0], &frb, IEEE_FMT_D);
	if (frb.nan) {
		c = 1;
	} else {
		fl = frb.f;
		if (fl < 0.0)
			c = 8;
		else if (fl > 0.0)
			c = 4;
		else
			c = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (c << PPC_FPSCR_FPCC_SHIFT);
	(*(uint64_t *)ic->arg[1]) =
	    ieee_store_float_value(fl, IEEE_FMT_D, frb.nan);
}


/*
 *  fctiwz:  Floating-point Convert to Integer Word, Round to Zero
 *
 *  arg[0] = ptr to frb
 *  arg[1] = ptr to frt
 */
X(fctiwz)
{
	struct ieee_float_value frb;
	uint32_t res = 0;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[0], &frb, IEEE_FMT_D);
	if (!frb.nan) {
		if (frb.f >= 2147483647.0)
			res = 0x7fffffff;
		else if (frb.f <= -2147483648.0)
			res = 0x80000000;
		else
			res = (int32_t) frb.f;
	}

	*(uint64_t *)ic->arg[1] = (uint32_t)res;
}


/*
 *  fmul:  Floating-point Multiply
 *
 *  arg[0] = ptr to frt
 *  arg[1] = ptr to fra
 *  arg[2] = ptr to frc
 */
X(fmul)
{
	struct ieee_float_value fra;
	struct ieee_float_value frc;
	double result = 0.0;
	int c, nan = 0;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(*(uint64_t *)ic->arg[2], &frc, IEEE_FMT_D);
	if (fra.nan || frc.nan)
		nan = 1;
	else
		result = fra.f * frc.f;
	if (nan)
		c = 1;
	else {
		if (result < 0.0)
			c = 8;
		else if (result > 0.0)
			c = 4;
		else
			c = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (c << PPC_FPSCR_FPCC_SHIFT);

	(*(uint64_t *)ic->arg[0]) =
	    ieee_store_float_value(result, IEEE_FMT_D, nan);
}
X(fmuls)
{
	/*  TODO  */
	instr(fmul)(cpu, ic);
}


/*
 *  fmadd:  Floating-point Multiply and Add
 *
 *  arg[0] = ptr to frt
 *  arg[1] = ptr to fra
 *  arg[2] = copy of the instruction word
 */
X(fmadd)
{
	uint32_t iw = ic->arg[2];
	int b = (iw >> 11) & 31, c = (iw >> 6) & 31;
	struct ieee_float_value fra;
	struct ieee_float_value frb;
	struct ieee_float_value frc;
	double result = 0.0;
	int nan = 0, cc;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(cpu->cd.ppc.fpr[b], &frb, IEEE_FMT_D);
	ieee_interpret_float_value(cpu->cd.ppc.fpr[c], &frc, IEEE_FMT_D);
	if (fra.nan || frb.nan || frc.nan)
		nan = 1;
	else
		result = fra.f * frc.f + frb.f;
	if (nan)
		cc = 1;
	else {
		if (result < 0.0)
			cc = 8;
		else if (result > 0.0)
			cc = 4;
		else
			cc = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (cc << PPC_FPSCR_FPCC_SHIFT);

	(*(uint64_t *)ic->arg[0]) =
	    ieee_store_float_value(result, IEEE_FMT_D, nan);
}


/*
 *  fmsub:  Floating-point Multiply and Sub
 *
 *  arg[0] = ptr to frt
 *  arg[1] = ptr to fra
 *  arg[2] = copy of the instruction word
 */
X(fmsub)
{
	uint32_t iw = ic->arg[2];
	int b = (iw >> 11) & 31, c = (iw >> 6) & 31;
	struct ieee_float_value fra;
	struct ieee_float_value frb;
	struct ieee_float_value frc;
	double result = 0.0;
	int nan = 0, cc;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(cpu->cd.ppc.fpr[b], &frb, IEEE_FMT_D);
	ieee_interpret_float_value(cpu->cd.ppc.fpr[c], &frc, IEEE_FMT_D);
	if (fra.nan || frb.nan || frc.nan)
		nan = 1;
	else
		result = fra.f * frc.f - frb.f;
	if (nan)
		cc = 1;
	else {
		if (result < 0.0)
			cc = 8;
		else if (result > 0.0)
			cc = 4;
		else
			cc = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (cc << PPC_FPSCR_FPCC_SHIFT);

	(*(uint64_t *)ic->arg[0]) =
	    ieee_store_float_value(result, IEEE_FMT_D, nan);
}


/*
 *  fadd, fsub, fdiv:  Various Floating-point operationgs
 *
 *  arg[0] = ptr to fra
 *  arg[1] = ptr to frb
 *  arg[2] = ptr to frt
 */
X(fadd)
{
	struct ieee_float_value fra;
	struct ieee_float_value frb;
	double result = 0.0;
	int nan = 0, c;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[0], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &frb, IEEE_FMT_D);
	if (fra.nan || frb.nan)
		nan = 1;
	else
		result = fra.f + frb.f;
	if (nan)
		c = 1;
	else {
		if (result < 0.0)
			c = 8;
		else if (result > 0.0)
			c = 4;
		else
			c = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (c << PPC_FPSCR_FPCC_SHIFT);

	(*(uint64_t *)ic->arg[2]) =
	    ieee_store_float_value(result, IEEE_FMT_D, nan);
}
X(fadds)
{
	/*  TODO  */
	instr(fadd)(cpu, ic);
}
X(fsub)
{
	struct ieee_float_value fra;
	struct ieee_float_value frb;
	double result = 0.0;
	int nan = 0, c;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[0], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &frb, IEEE_FMT_D);
	if (fra.nan || frb.nan)
		nan = 1;
	else
		result = fra.f - frb.f;
	if (nan)
		c = 1;
	else {
		if (result < 0.0)
			c = 8;
		else if (result > 0.0)
			c = 4;
		else
			c = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (c << PPC_FPSCR_FPCC_SHIFT);

	(*(uint64_t *)ic->arg[2]) =
	    ieee_store_float_value(result, IEEE_FMT_D, nan);
}
X(fsubs)
{
	/*  TODO  */
	instr(fsub)(cpu, ic);
}
X(fdiv)
{
	struct ieee_float_value fra;
	struct ieee_float_value frb;
	double result = 0.0;
	int nan = 0, c;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*(uint64_t *)ic->arg[0], &fra, IEEE_FMT_D);
	ieee_interpret_float_value(*(uint64_t *)ic->arg[1], &frb, IEEE_FMT_D);
	if (fra.nan || frb.nan || frb.f == 0)
		nan = 1;
	else
		result = fra.f / frb.f;
	if (nan)
		c = 1;
	else {
		if (result < 0.0)
			c = 8;
		else if (result > 0.0)
			c = 4;
		else
			c = 2;
	}
	/*  TODO: Signaling vs Quiet NaN  */
	cpu->cd.ppc.fpscr &= ~(PPC_FPSCR_FPCC | PPC_FPSCR_VXNAN);
	cpu->cd.ppc.fpscr |= (c << PPC_FPSCR_FPCC_SHIFT);

	(*(uint64_t *)ic->arg[2]) =
	    ieee_store_float_value(result, IEEE_FMT_D, nan);
}
X(fdivs)
{
	/*  TODO  */
	instr(fdiv)(cpu, ic);
}


/*
 *  llsc: Load-linked and store conditional
 *
 *  arg[0] = copy of the instruction word.
 */
X(llsc)
{
	int iw = ic->arg[0], len = 4, load = 0, xo = (iw >> 1) & 1023;
	int i, rc = iw & 1, rt, ra, rb;
	uint64_t addr = 0, value;
	unsigned char d[8];

	switch (xo) {
	case PPC_31_LDARX:
		len = 8;
	case PPC_31_LWARX:
		load = 1;
		break;
	case PPC_31_STDCX_DOT:
		len = 8;
	case PPC_31_STWCX_DOT:
		break;
	}

	rt = (iw >> 21) & 31;
	ra = (iw >> 16) & 31;
	rb = (iw >> 11) & 31;

	if (ra != 0)
		addr = cpu->cd.ppc.gpr[ra];
	addr += cpu->cd.ppc.gpr[rb];

	if (load) {
		if (rc) {
			fatal("ll: rc-bit set?\n");
			exit(1);
		}
		if (cpu->memory_rw(cpu, cpu->mem, addr, d, len,
		    MEM_READ, CACHE_DATA) != MEMORY_ACCESS_OK) {
			fatal("ll: error: TODO\n");
			exit(1);
		}

		value = 0;
		for (i=0; i<len; i++) {
			value <<= 8;
			if (cpu->byte_order == EMUL_BIG_ENDIAN)
				value |= d[i];
			else
				value |= d[len - 1 - i];
		}

		cpu->cd.ppc.gpr[rt] = value;
		cpu->cd.ppc.ll_addr = addr;
		cpu->cd.ppc.ll_bit = 1;
	} else {
		uint32_t old_so = cpu->cd.ppc.spr[SPR_XER] & PPC_XER_SO;
		if (!rc) {
			fatal("sc: rc-bit not set?\n");
			exit(1);
		}

		value = cpu->cd.ppc.gpr[rt];

		/*  "If the store is performed, bits 0-2 of Condition
		    Register Field 0 are set to 0b001, otherwise, they are
		    set to 0b000. The SO bit of the XER is copied to to bit
		    4 of Condition Register Field 0.  */
		if (!cpu->cd.ppc.ll_bit || cpu->cd.ppc.ll_addr != addr) {
			cpu->cd.ppc.cr &= 0x0fffffff;
			if (old_so)
				cpu->cd.ppc.cr |= 0x10000000;
			cpu->cd.ppc.ll_bit = 0;
			return;
		}

		for (i=0; i<len; i++) {
			if (cpu->byte_order == EMUL_BIG_ENDIAN)
				d[len - 1 - i] = value >> (8*i);
			else
				d[i] = value >> (8*i);
		}

		if (cpu->memory_rw(cpu, cpu->mem, addr, d, len,
		    MEM_WRITE, CACHE_DATA) != MEMORY_ACCESS_OK) {
			fatal("sc: error: TODO\n");
			exit(1);
		}

		cpu->cd.ppc.cr &= 0x0fffffff;
		cpu->cd.ppc.cr |= 0x20000000;	/*  success!  */
		if (old_so)
			cpu->cd.ppc.cr |= 0x10000000;

		/*  Clear _all_ CPUs' ll_bits:  */
		for (i=0; i<cpu->machine->ncpus; i++)
			cpu->machine->cpus[i]->cd.ppc.ll_bit = 0;
	}
}


/*
 *  mtsr, mtsrin:  Move To Segment Register [Indirect]
 *
 *  arg[0] = sr number, or for indirect mode: ptr to rb
 *  arg[1] = ptr to rt
 *
 *  TODO: These only work for 32-bit mode!
 */
X(mtsr)
{
	int sr_num = ic->arg[0];
	uint32_t old = cpu->cd.ppc.sr[sr_num];
	cpu->cd.ppc.sr[sr_num] = reg(ic->arg[1]);

	if (cpu->cd.ppc.sr[sr_num] != old)
		cpu->invalidate_translation_caches(cpu, ic->arg[0] << 28,
		    INVALIDATE_ALL | INVALIDATE_VADDR_UPPER4);
}
X(mtsrin)
{
	int sr_num = reg(ic->arg[0]) >> 28;
	uint32_t old = cpu->cd.ppc.sr[sr_num];
	cpu->cd.ppc.sr[sr_num] = reg(ic->arg[1]);

	if (cpu->cd.ppc.sr[sr_num] != old)
		cpu->invalidate_translation_caches(cpu, sr_num << 28,
		    INVALIDATE_ALL | INVALIDATE_VADDR_UPPER4);
}


/*
 *  mfsrin, mtsrin:  Move From/To Segment Register Indirect
 *
 *  arg[0] = sr number, or for indirect mode: ptr to rb
 *  arg[1] = ptr to rt
 */
X(mfsr)
{
	/*  TODO: This only works for 32-bit mode  */
	reg(ic->arg[1]) = cpu->cd.ppc.sr[ic->arg[0]];
}
X(mfsrin)
{
	/*  TODO: This only works for 32-bit mode  */
	uint32_t sr_num = reg(ic->arg[0]) >> 28;
	reg(ic->arg[1]) = cpu->cd.ppc.sr[sr_num];
}


/*
 *  rldicl:
 *
 *  arg[0] = copy of the instruction word
 */
X(rldicl)
{
	int rs = (ic->arg[0] >> 21) & 31;
	int ra = (ic->arg[0] >> 16) & 31;
	int sh = ((ic->arg[0] >> 11) & 31) | ((ic->arg[0] & 2) << 4);
	int mb = ((ic->arg[0] >> 6) & 31) | (ic->arg[0] & 0x20);
	int rc = ic->arg[0] & 1;
	uint64_t tmp = cpu->cd.ppc.gpr[rs], tmp2;
	/*  TODO: Fix this, its performance is awful:  */
	while (sh-- != 0) {
		int b = (tmp >> 63) & 1;
		tmp = (tmp << 1) | b;
	}
	tmp2 = 0;
	while (mb <= 63) {
		tmp |= ((uint64_t)1 << (63-mb));
		mb ++;
	}
	cpu->cd.ppc.gpr[ra] = tmp & tmp2;
	if (rc)
		UPDATE_CR0(cpu, cpu->cd.ppc.gpr[ra]);
}


/*
 *  rldicr:
 *
 *  arg[0] = copy of the instruction word
 */
X(rldicr)
{
	int rs = (ic->arg[0] >> 21) & 31;
	int ra = (ic->arg[0] >> 16) & 31;
	int sh = ((ic->arg[0] >> 11) & 31) | ((ic->arg[0] & 2) << 4);
	int me = ((ic->arg[0] >> 6) & 31) | (ic->arg[0] & 0x20);
	int rc = ic->arg[0] & 1;
	uint64_t tmp = cpu->cd.ppc.gpr[rs];
	/*  TODO: Fix this, its performance is awful:  */
	while (sh-- != 0) {
		int b = (tmp >> 63) & 1;
		tmp = (tmp << 1) | b;
	}
	while (me++ < 63)
		tmp &= ~((uint64_t)1 << (63-me));
	cpu->cd.ppc.gpr[ra] = tmp;
	if (rc)
		UPDATE_CR0(cpu, tmp);
}


/*
 *  rldimi:
 *
 *  arg[0] = copy of the instruction word
 */
X(rldimi)
{
	uint32_t iw = ic->arg[0];
	int rs = (iw >> 21) & 31, ra = (iw >> 16) & 31;
	int sh = ((iw >> 11) & 31) | ((iw & 2) << 4);
	int mb = ((iw >> 6) & 31) | (iw & 0x20);
	int rc = ic->arg[0] & 1;
	int m;
	uint64_t tmp, s = cpu->cd.ppc.gpr[rs];
	/*  TODO: Fix this, its performance is awful:  */
	while (sh-- != 0) {
		int b = (s >> 63) & 1;
		s = (s << 1) | b;
	}
	m = mb; tmp = 0;
	do {
		tmp |= ((uint64_t)1 << (63-m));
		m ++;
	} while (m != 63 - sh);
	cpu->cd.ppc.gpr[ra] &= ~tmp;
	cpu->cd.ppc.gpr[ra] |= (tmp & s);
	if (rc)
		UPDATE_CR0(cpu, cpu->cd.ppc.gpr[ra]);
}


/*
 *  rlwnm:
 *
 *  arg[0] = ptr to ra
 *  arg[1] = mask
 *  arg[2] = copy of the instruction word
 */
X(rlwnm)
{
	uint32_t tmp, iword = ic->arg[2];
	int rs = (iword >> 21) & 31;
	int rb = (iword >> 11) & 31;
	int sh = cpu->cd.ppc.gpr[rb] & 0x1f;
	tmp = (uint32_t)cpu->cd.ppc.gpr[rs];
	tmp = (tmp << sh) | (tmp >> (32-sh));
	tmp &= (uint32_t)ic->arg[1];
	reg(ic->arg[0]) = tmp;
}
DOT0(rlwnm)


/*
 *  rlwinm:
 *
 *  arg[0] = ptr to ra
 *  arg[1] = mask
 *  arg[2] = copy of the instruction word
 */
X(rlwinm)
{
	uint32_t tmp, iword = ic->arg[2];
	int rs = (iword >> 21) & 31;
	int sh = (iword >> 11) & 31;
	tmp = (uint32_t)cpu->cd.ppc.gpr[rs];
	tmp = (tmp << sh) | (tmp >> (32-sh));
	tmp &= (uint32_t)ic->arg[1];
	reg(ic->arg[0]) = tmp;
}
DOT0(rlwinm)


/*
 *  rlwimi:
 *
 *  arg[0] = ptr to rs
 *  arg[1] = ptr to ra
 *  arg[2] = copy of the instruction word
 */
X(rlwimi)
{
	MODE_uint_t tmp = reg(ic->arg[0]), ra = reg(ic->arg[1]);
	uint32_t iword = ic->arg[2];
	int sh = (iword >> 11) & 31;
	int mb = (iword >> 6) & 31;
	int me = (iword >> 1) & 31;   
	int rc = iword & 1;

	tmp = (tmp << sh) | (tmp >> (32-sh));

	for (;;) {
		uint64_t mask;
		mask = (uint64_t)1 << (31-mb);
		ra &= ~mask;
		ra |= (tmp & mask);
		if (mb == me)
			break;
		mb ++;
		if (mb == 32)
			mb = 0;
	}
	reg(ic->arg[1]) = ra;
	if (rc)
		UPDATE_CR0(cpu, ra);
}


/*
 *  srawi:
 *
 *  arg[0] = ptr to rs
 *  arg[1] = ptr to ra
 *  arg[2] = sh (shift amount)
 */
X(srawi)
{
	uint32_t tmp = reg(ic->arg[0]);
	int i = 0, j = 0, sh = ic->arg[2];

	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (tmp & 0x80000000)
		i = 1;
	while (sh-- > 0) {
		if (tmp & 1)
			j ++;
		tmp >>= 1;
		if (tmp & 0x40000000)
			tmp |= 0x80000000;
	}
	if (i && j>0)
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[1]) = (int64_t)(int32_t)tmp;
}
DOT1(srawi)


/*
 *  mcrf:  Move inside condition register
 *
 *  arg[0] = 28-4*bf,  arg[1] = 28-4*bfa
 */
X(mcrf)
{
	int bf_shift = ic->arg[0], bfa_shift = ic->arg[1];
	uint32_t tmp = (cpu->cd.ppc.cr >> bfa_shift) & 0xf;
	cpu->cd.ppc.cr &= ~(0xf << bf_shift);
	cpu->cd.ppc.cr |= (tmp << bf_shift);
}


/*
 *  crand, crxor etc:  Condition Register operations
 *
 *  arg[0] = copy of the instruction word
 */
X(crand) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (ba & bb)
		cpu->cd.ppc.cr |= (1 << (31-bt));
}
X(crandc) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (!(ba & bb))
		cpu->cd.ppc.cr |= (1 << (31-bt));
}
X(creqv) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (!(ba ^ bb))
		cpu->cd.ppc.cr |= (1 << (31-bt));
}
X(cror) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (ba | bb)
		cpu->cd.ppc.cr |= (1 << (31-bt));
}
X(crorc) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (!(ba | bb))
		cpu->cd.ppc.cr |= (1 << (31-bt));
}
X(crnor) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (!(ba | bb))
		cpu->cd.ppc.cr |= (1 << (31-bt));
}
X(crxor) {
	uint32_t iword = ic->arg[0]; int bt = (iword >> 21) & 31;
	int ba = (iword >> 16) & 31, bb = (iword >> 11) & 31;
	ba = (cpu->cd.ppc.cr >> (31-ba)) & 1;
	bb = (cpu->cd.ppc.cr >> (31-bb)) & 1;
	cpu->cd.ppc.cr &= ~(1 << (31-bt));
	if (ba ^ bb)
		cpu->cd.ppc.cr |= (1 << (31-bt));
}


/*
 *  mfspr: Move from SPR
 *
 *  arg[0] = pointer to destination register
 *  arg[1] = pointer to source SPR
 */
X(mfspr) {
	/*  TODO: Check permission  */
	reg(ic->arg[0]) = reg(ic->arg[1]);
}
X(mfspr_pmc1) {
	/*
	 *  TODO: This is a temporary hack to make NetBSD/ppc detect
	 *  a CPU of the correct (emulated) speed.
	 */
	reg(ic->arg[0]) = cpu->machine->emulated_hz / 10;
}
X(mftb) {
	/*  NOTE/TODO: This increments the time base (slowly) if it
	    is being polled.  */
	if (++cpu->cd.ppc.spr[SPR_TBL] == 0)
		cpu->cd.ppc.spr[SPR_TBU] ++;
	reg(ic->arg[0]) = cpu->cd.ppc.spr[SPR_TBL];
}
X(mftbu) {
	reg(ic->arg[0]) = cpu->cd.ppc.spr[SPR_TBU];
}


/*
 *  mtspr: Move to SPR.
 *
 *  arg[0] = pointer to source register
 *  arg[1] = pointer to the SPR
 */
X(mtspr) {
	/*  TODO: Check permission  */
	reg(ic->arg[1]) = reg(ic->arg[0]);
}
X(mtlr) {
	cpu->cd.ppc.spr[SPR_LR] = reg(ic->arg[0]);
}
X(mtctr) {
	cpu->cd.ppc.spr[SPR_CTR] = reg(ic->arg[0]);
}


/*
 *  rfi[d]:  Return from Interrupt
 */
X(rfi)
{
	uint64_t tmp;

	reg_access_msr(cpu, &tmp, 0, 0);
	tmp &= ~0xffff;
	tmp |= (cpu->cd.ppc.spr[SPR_SRR1] & 0xffff);
	reg_access_msr(cpu, &tmp, 1, 0);

	cpu->pc = cpu->cd.ppc.spr[SPR_SRR0];
	quick_pc_to_pointers(cpu);
}
X(rfid)
{
	uint64_t tmp, mask = 0x800000000000ff73ULL;

	reg_access_msr(cpu, &tmp, 0, 0);
	tmp &= ~mask;
	tmp |= (cpu->cd.ppc.spr[SPR_SRR1] & mask);
	reg_access_msr(cpu, &tmp, 1, 0);

	cpu->pc = cpu->cd.ppc.spr[SPR_SRR0];
	if (!(tmp & PPC_MSR_SF))
		cpu->pc = (uint32_t)cpu->pc;
	quick_pc_to_pointers(cpu);
}


/*
 *  mfcr:  Move From Condition Register
 *
 *  arg[0] = pointer to destination register
 */
X(mfcr)
{
	reg(ic->arg[0]) = cpu->cd.ppc.cr;
}


/*
 *  mfmsr:  Move From MSR
 *
 *  arg[0] = pointer to destination register
 */
X(mfmsr)
{
	reg_access_msr(cpu, (uint64_t*)ic->arg[0], 0, 0);
}


/*
 *  mtmsr:  Move To MSR
 *
 *  arg[0] = pointer to source register
 *  arg[1] = page offset of the next instruction
 *  arg[2] = 0 for 32-bit (mtmsr), 1 for 64-bit (mtmsrd)
 */
X(mtmsr)
{
	MODE_uint_t old_pc;
	uint64_t x = reg(ic->arg[0]);

	/*  TODO: check permission!  */

	/*  Synchronize the PC (pointing to _after_ this instruction)  */
	cpu->pc = (cpu->pc & ~0xfff) + ic->arg[1];
	old_pc = cpu->pc;

	if (!ic->arg[2]) {
		uint64_t y;
		reg_access_msr(cpu, &y, 0, 0);
		x = (y & 0xffffffff00000000ULL) | (x & 0xffffffffULL);
	}

	reg_access_msr(cpu, &x, 1, 1);

	/*
	 *  Super-ugly hack:  If the pc wasn't changed (i.e. if there was no
	 *  exception while accessing the msr), then we _decrease_ the PC by 4
	 *  again. This is because the next ic could be an end_of_page.
	 */
	if ((MODE_uint_t)cpu->pc == old_pc)
		cpu->pc -= 4;
}


/*
 *  wrteei:  Write EE immediate  (on PPC405GP)
 *
 *  arg[0] = either 0 or 0x8000
 */
X(wrteei)
{
	/*  TODO: check permission!  */
	uint64_t x;

	/*  Synchronize the PC (pointing to _after_ this instruction)  */
	cpu->pc = (cpu->pc & ~0xfff) + ic->arg[1];

	reg_access_msr(cpu, &x, 0, 0);
	x = (x & ~0x8000) | ic->arg[0];
	reg_access_msr(cpu, &x, 1, 1);
}


/*
 *  mtcrf:  Move To Condition Register Fields
 *
 *  arg[0] = pointer to source register
 */
X(mtcrf)
{
	cpu->cd.ppc.cr &= ~ic->arg[1];
	cpu->cd.ppc.cr |= (reg(ic->arg[0]) & ic->arg[1]);
}


/*
 *  mulli:  Multiply Low Immediate.
 *
 *  arg[0] = pointer to source register ra
 *  arg[1] = int32_t immediate
 *  arg[2] = pointer to destination register rt
 */
X(mulli)
{
	reg(ic->arg[2]) = (uint32_t)(reg(ic->arg[0]) * (int32_t)ic->arg[1]);
}


/*
 *  Load/Store Multiple:
 *
 *  arg[0] = rs  (or rt for loads)  NOTE: not a pointer
 *  arg[1] = ptr to ra
 *  arg[2] = int32_t immediate offset
 */
X(lmw) {
	MODE_uint_t addr = reg(ic->arg[1]) + (int32_t)ic->arg[2];
	unsigned char d[4];
	int rs = ic->arg[0];

	int low_pc = ((size_t)ic - (size_t)cpu->cd.ppc.cur_ic_page)
	    / sizeof(struct ppc_instr_call);
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc |= (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	while (rs <= 31) {
		if (cpu->memory_rw(cpu, cpu->mem, addr, d, sizeof(d),
		    MEM_READ, CACHE_DATA) != MEMORY_ACCESS_OK) {
			/*  exception  */
			return;
		}

		if (cpu->byte_order == EMUL_BIG_ENDIAN)
			cpu->cd.ppc.gpr[rs] = (d[0] << 24) + (d[1] << 16)
			    + (d[2] << 8) + d[3];
		else
			cpu->cd.ppc.gpr[rs] = (d[3] << 24) + (d[2] << 16)
			    + (d[1] << 8) + d[0];

		rs ++;
		addr += sizeof(uint32_t);
	}
}
X(stmw) {
	MODE_uint_t addr = reg(ic->arg[1]) + (int32_t)ic->arg[2];
	unsigned char d[4];
	int rs = ic->arg[0];

	int low_pc = ((size_t)ic - (size_t)cpu->cd.ppc.cur_ic_page)
	    / sizeof(struct ppc_instr_call);
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	while (rs <= 31) {
		uint32_t tmp = cpu->cd.ppc.gpr[rs];
		if (cpu->byte_order == EMUL_BIG_ENDIAN) {
			d[3] = tmp; d[2] = tmp >> 8;
			d[1] = tmp >> 16; d[0] = tmp >> 24;
		} else {
			d[0] = tmp; d[1] = tmp >> 8;
			d[2] = tmp >> 16; d[3] = tmp >> 24;
		}
		if (cpu->memory_rw(cpu, cpu->mem, addr, d, sizeof(d),
		    MEM_WRITE, CACHE_DATA) != MEMORY_ACCESS_OK) {
			/*  exception  */
			return;
		}

		rs ++;
		addr += sizeof(uint32_t);
	}
}


/*
 *  Load/store string:
 *
 *  arg[0] = rs   (well, rt for lswi)
 *  arg[1] = ptr to ra (or ptr to zero)
 *  arg[2] = nb
 */
X(lswi)
{
	MODE_uint_t addr = reg(ic->arg[1]);
	int rt = ic->arg[0], nb = ic->arg[2];
	int sub = 0;

	int low_pc = ((size_t)ic - (size_t)cpu->cd.ppc.cur_ic_page)
	    / sizeof(struct ppc_instr_call);
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	while (nb > 0) {
		unsigned char d;
		if (cpu->memory_rw(cpu, cpu->mem, addr, &d, 1,
		    MEM_READ, CACHE_DATA) != MEMORY_ACCESS_OK) {
			/*  exception  */
			return;
		}

		if (cpu->cd.ppc.mode == MODE_POWER && sub == 0)
			cpu->cd.ppc.gpr[rt] = 0;
		cpu->cd.ppc.gpr[rt] &= ~(0xff << (24-8*sub));
		cpu->cd.ppc.gpr[rt] |= (d << (24-8*sub));
		sub ++;
		if (sub == 4) {
			rt = (rt + 1) & 31;
			sub = 0;
		}
		addr ++;
		nb --;
	}
}
X(stswi)
{
	MODE_uint_t addr = reg(ic->arg[1]);
	int rs = ic->arg[0], nb = ic->arg[2];
	uint32_t cur = cpu->cd.ppc.gpr[rs];
	int sub = 0;

	int low_pc = ((size_t)ic - (size_t)cpu->cd.ppc.cur_ic_page)
	    / sizeof(struct ppc_instr_call);
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	while (nb > 0) {
		unsigned char d = cur >> 24;
		if (cpu->memory_rw(cpu, cpu->mem, addr, &d, 1,
		    MEM_WRITE, CACHE_DATA) != MEMORY_ACCESS_OK) {
			/*  exception  */
			return;
		}
		cur <<= 8;
		sub ++;
		if (sub == 4) {
			rs = (rs + 1) & 31;
			sub = 0;
			cur = cpu->cd.ppc.gpr[rs];
		}
		addr ++;
		nb --;
	}
}


/*
 *  Shifts, and, or, xor, etc.
 *
 *  arg[0] = pointer to source register rs
 *  arg[1] = pointer to source register rb
 *  arg[2] = pointer to destination register ra
 */
X(extsb) {
#ifdef MODE32
	reg(ic->arg[2]) = (int32_t)(int8_t)reg(ic->arg[0]);
#else
	reg(ic->arg[2]) = (int64_t)(int8_t)reg(ic->arg[0]);
#endif
}
DOT2(extsb)
X(extsh) {
#ifdef MODE32
	reg(ic->arg[2]) = (int32_t)(int16_t)reg(ic->arg[0]);
#else
	reg(ic->arg[2]) = (int64_t)(int16_t)reg(ic->arg[0]);
#endif
}
DOT2(extsh)
X(extsw) {
#ifdef MODE32
	fatal("TODO: extsw: invalid instruction\n");
#else
	reg(ic->arg[2]) = (int64_t)(int32_t)reg(ic->arg[0]);
#endif
}
DOT2(extsw)
X(slw) {	reg(ic->arg[2]) = (uint64_t)reg(ic->arg[0])
		    << (reg(ic->arg[1]) & 31); }
DOT2(slw)
X(sld) {int sa = reg(ic->arg[1]) & 127;
	if (sa >= 64)	reg(ic->arg[2]) = 0;
	else reg(ic->arg[2]) = (uint64_t)reg(ic->arg[0]) << (sa & 63); }
DOT2(sld)
X(sraw)
{
	uint32_t tmp = reg(ic->arg[0]);
	int i = 0, j = 0, sh = reg(ic->arg[1]) & 31;

	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (tmp & 0x80000000)
		i = 1;
	while (sh-- > 0) {
		if (tmp & 1)
			j ++;
		tmp >>= 1;
		if (tmp & 0x40000000)
			tmp |= 0x80000000;
	}
	if (i && j>0)
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (int64_t)(int32_t)tmp;
}
DOT2(sraw)
X(srw) {	reg(ic->arg[2]) = (uint64_t)reg(ic->arg[0])
		    >> (reg(ic->arg[1]) & 31); }
DOT2(srw)
X(and) {	reg(ic->arg[2]) = reg(ic->arg[0]) & reg(ic->arg[1]); }
DOT2(and)
X(nand) {	reg(ic->arg[2]) = ~(reg(ic->arg[0]) & reg(ic->arg[1])); }
DOT2(nand)
X(andc) {	reg(ic->arg[2]) = reg(ic->arg[0]) & (~reg(ic->arg[1])); }
DOT2(andc)
X(nor) {	reg(ic->arg[2]) = ~(reg(ic->arg[0]) | reg(ic->arg[1])); }
DOT2(nor)
X(mr) {		reg(ic->arg[2]) = reg(ic->arg[1]); }
X(or) {		reg(ic->arg[2]) = reg(ic->arg[0]) | reg(ic->arg[1]); }
DOT2(or)
X(orc) {	reg(ic->arg[2]) = reg(ic->arg[0]) | (~reg(ic->arg[1])); }
DOT2(orc)
X(xor) {	reg(ic->arg[2]) = reg(ic->arg[0]) ^ reg(ic->arg[1]); }
DOT2(xor)
X(eqv) {	reg(ic->arg[2]) = ~(reg(ic->arg[0]) ^ reg(ic->arg[1])); }
DOT2(eqv)


/*
 *  neg:
 *
 *  arg[0] = pointer to source register ra
 *  arg[1] = pointer to destination register rt
 */
X(neg) {	reg(ic->arg[1]) = -reg(ic->arg[0]); }
DOT1(neg)


/*
 *  mullw, mulhw[u], divw[u]:
 *
 *  arg[0] = pointer to source register ra
 *  arg[1] = pointer to source register rb
 *  arg[2] = pointer to destination register rt
 */
X(mullw)
{
	int32_t sum = (int32_t)reg(ic->arg[0]) * (int32_t)reg(ic->arg[1]);
	reg(ic->arg[2]) = (int32_t)sum;
}
DOT2(mullw)
X(mulhw)
{
	int64_t sum;
	sum = (int64_t)(int32_t)reg(ic->arg[0])
	    * (int64_t)(int32_t)reg(ic->arg[1]);
	reg(ic->arg[2]) = sum >> 32;
}
DOT2(mulhw)
X(mulhwu)
{
	uint64_t sum;
	sum = (uint64_t)(uint32_t)reg(ic->arg[0])
	    * (uint64_t)(uint32_t)reg(ic->arg[1]);
	reg(ic->arg[2]) = sum >> 32;
}
DOT2(mulhwu)
X(divw)
{
	int32_t a = reg(ic->arg[0]), b = reg(ic->arg[1]);
	int32_t sum;
	if (b == 0)
		sum = 0;
	else
		sum = a / b;
	reg(ic->arg[2]) = (uint32_t)sum;
}
DOT2(divw)
X(divwu)
{
	uint32_t a = reg(ic->arg[0]), b = reg(ic->arg[1]);
	uint32_t sum;
	if (b == 0)
		sum = 0;
	else
		sum = a / b;
	reg(ic->arg[2]) = sum;
}
DOT2(divwu)


/*
 *  addc:  Add carrying.
 *
 *  arg[0] = pointer to source register ra
 *  arg[1] = pointer to source register rb
 *  arg[2] = pointer to destination register rt
 */
X(addc)
{
	/*  TODO: this only works in 32-bit mode  */
	uint64_t tmp = (uint32_t)reg(ic->arg[0]);
	uint64_t tmp2 = tmp;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	tmp += (uint32_t)reg(ic->arg[1]);
	if ((tmp >> 32) != (tmp2 >> 32))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (uint32_t)tmp;
}


/*
 *  adde:  Add extended, etc.
 *
 *  arg[0] = pointer to source register ra
 *  arg[1] = pointer to source register rb
 *  arg[2] = pointer to destination register rt
 */
X(adde)
{
	/*  TODO: this only works in 32-bit mode  */
	int old_ca = cpu->cd.ppc.spr[SPR_XER] & PPC_XER_CA;
	uint64_t tmp = (uint32_t)reg(ic->arg[0]);
	uint64_t tmp2 = tmp;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	tmp += (uint32_t)reg(ic->arg[1]);
	if (old_ca)
		tmp ++;
	if ((tmp >> 32) != (tmp2 >> 32))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (uint32_t)tmp;
}
DOT2(adde)
X(addme)
{
	/*  TODO: this only works in 32-bit mode  */
	int old_ca = cpu->cd.ppc.spr[SPR_XER] & PPC_XER_CA;
	uint64_t tmp = (uint32_t)reg(ic->arg[0]);
	uint64_t tmp2 = tmp;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (old_ca)
		tmp ++;
	tmp += 0xffffffffULL;
	if ((tmp >> 32) != (tmp2 >> 32))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (uint32_t)tmp;
}
DOT2(addme)
X(addze)
{
	/*  TODO: this only works in 32-bit mode  */
	int old_ca = cpu->cd.ppc.spr[SPR_XER] & PPC_XER_CA;
	uint64_t tmp = (uint32_t)reg(ic->arg[0]);
	uint64_t tmp2 = tmp;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (old_ca)
		tmp ++;
	if ((tmp >> 32) != (tmp2 >> 32))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (uint32_t)tmp;
}
DOT2(addze)


/*
 *  subf:  Subf, etc.
 *
 *  arg[0] = pointer to source register ra
 *  arg[1] = pointer to source register rb
 *  arg[2] = pointer to destination register rt
 */
X(subf)
{
	reg(ic->arg[2]) = reg(ic->arg[1]) - reg(ic->arg[0]);
}
DOT2(subf)
X(subfc)
{
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (reg(ic->arg[1]) >= reg(ic->arg[0]))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = reg(ic->arg[1]) - reg(ic->arg[0]);
}
DOT2(subfc)
X(subfe)
{
	int old_ca = (cpu->cd.ppc.spr[SPR_XER] & PPC_XER_CA)? 1 : 0;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (reg(ic->arg[1]) == reg(ic->arg[0])) {
		if (old_ca)
			cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	} else if (reg(ic->arg[1]) >= reg(ic->arg[0]))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;

	/*
	 *  TODO: The register value calculation should be correct,
	 *  but the CA bit calculation above is probably not.
	 */

	reg(ic->arg[2]) = reg(ic->arg[1]) - reg(ic->arg[0]) - (old_ca? 0 : 1);
}
DOT2(subfe)
X(subfme)
{
	int old_ca = cpu->cd.ppc.spr[SPR_XER] & PPC_XER_CA;
	uint64_t tmp = (uint32_t)(~reg(ic->arg[0]));
	tmp += 0xffffffffULL;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (old_ca)
		tmp ++;
	if ((tmp >> 32) != 0)
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (uint32_t)tmp;
}
DOT2(subfme)
X(subfze)
{
	int old_ca = cpu->cd.ppc.spr[SPR_XER] & PPC_XER_CA;
	uint64_t tmp = (uint32_t)(~reg(ic->arg[0]));
	uint64_t tmp2 = tmp;
	cpu->cd.ppc.spr[SPR_XER] &= ~PPC_XER_CA;
	if (old_ca)
		tmp ++;
	if ((tmp >> 32) != (tmp2 >> 32))
		cpu->cd.ppc.spr[SPR_XER] |= PPC_XER_CA;
	reg(ic->arg[2]) = (uint32_t)tmp;
}
DOT2(subfze)


/*
 *  ori, xori etc.:
 *
 *  arg[0] = pointer to source uint64_t
 *  arg[1] = immediate value (uint32_t or larger)
 *  arg[2] = pointer to destination uint64_t
 */
X(ori)  { reg(ic->arg[2]) = reg(ic->arg[0]) | (uint32_t)ic->arg[1]; }
X(xori) { reg(ic->arg[2]) = reg(ic->arg[0]) ^ (uint32_t)ic->arg[1]; }


#include "tmp_ppc_loadstore.cc"


/*
 *  lfs, stfs: Load/Store Floating-point Single precision
 */
X(lfs)
{
	/*  Sync. PC in case of an exception, and remember it:  */
	uint64_t old_pc, low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call);
	old_pc = cpu->pc = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) <<
	    PPC_INSTR_ALIGNMENT_SHIFT)) + (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);
	if (!(cpu->cd.ppc.msr & PPC_MSR_FP)) {
		ppc_exception(cpu, PPC_EXCEPTION_FPU);
		return;
	}

	/*  Perform a 32-bit load:  */
#ifdef MODE32
	ppc32_loadstore
#else
	ppc_loadstore
#endif
	    [2 + 4 + 8](cpu, ic);

	if (old_pc == cpu->pc) {
		/*  The load succeeded. Let's convert the value:  */
		struct ieee_float_value val;
		(*(uint64_t *)ic->arg[0]) &= 0xffffffff;
		ieee_interpret_float_value(*(uint64_t *)ic->arg[0],
		    &val, IEEE_FMT_S);
		(*(uint64_t *)ic->arg[0]) =
		    ieee_store_float_value(val.f, IEEE_FMT_D, val.nan);
	}
}
X(lfsx)
{
	/*  Sync. PC in case of an exception, and remember it:  */
	uint64_t old_pc, low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call);
	old_pc = cpu->pc = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) <<
	    PPC_INSTR_ALIGNMENT_SHIFT)) + (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);
	if (!(cpu->cd.ppc.msr & PPC_MSR_FP)) {
		ppc_exception(cpu, PPC_EXCEPTION_FPU);
		return;
	}

	/*  Perform a 32-bit load:  */
#ifdef MODE32
	ppc32_loadstore_indexed
#else
	ppc_loadstore_indexed
#endif
	    [2 + 4 + 8](cpu, ic);

	if (old_pc == cpu->pc) {
		/*  The load succeeded. Let's convert the value:  */
		struct ieee_float_value val;
		(*(uint64_t *)ic->arg[0]) &= 0xffffffff;
		ieee_interpret_float_value(*(uint64_t *)ic->arg[0],
		    &val, IEEE_FMT_S);
		(*(uint64_t *)ic->arg[0]) =
		    ieee_store_float_value(val.f, IEEE_FMT_D, val.nan);
	}
}
X(lfd)
{
	CHECK_FOR_FPU_EXCEPTION;

	/*  Perform a 64-bit load:  */
#ifdef MODE32
	ppc32_loadstore
#else
	ppc_loadstore
#endif
	    [3 + 4 + 8](cpu, ic);
}
X(lfdx)
{
	CHECK_FOR_FPU_EXCEPTION;

	/*  Perform a 64-bit load:  */
#ifdef MODE32
	ppc32_loadstore_indexed
#else
	ppc_loadstore_indexed
#endif
	    [3 + 4 + 8](cpu, ic);
}
X(stfs)
{
	uint64_t *old_arg0 = (uint64_t *) ic->arg[0];
	struct ieee_float_value val;
	uint64_t tmp_val;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*old_arg0, &val, IEEE_FMT_D);
	tmp_val = ieee_store_float_value(val.f, IEEE_FMT_S, val.nan);

	ic->arg[0] = (size_t)&tmp_val;

	/*  Perform a 32-bit store:  */
#ifdef MODE32
	ppc32_loadstore
#else
	ppc_loadstore
#endif
	    [2 + 4](cpu, ic);

	ic->arg[0] = (size_t)old_arg0;
}
X(stfsx)
{
	uint64_t *old_arg0 = (uint64_t *)ic->arg[0];
	struct ieee_float_value val;
	uint64_t tmp_val;

	CHECK_FOR_FPU_EXCEPTION;

	ieee_interpret_float_value(*old_arg0, &val, IEEE_FMT_D);
	tmp_val = ieee_store_float_value(val.f, IEEE_FMT_S, val.nan);

	ic->arg[0] = (size_t)&tmp_val;

	/*  Perform a 32-bit store:  */
#ifdef MODE32
	ppc32_loadstore_indexed
#else
	ppc_loadstore_indexed
#endif
	    [2 + 4](cpu, ic);

	ic->arg[0] = (size_t)old_arg0;
}
X(stfd)
{
	CHECK_FOR_FPU_EXCEPTION;

	/*  Perform a 64-bit store:  */
#ifdef MODE32
	ppc32_loadstore
#else
	ppc_loadstore
#endif
	    [3 + 4](cpu, ic);
}
X(stfdx)
{
	CHECK_FOR_FPU_EXCEPTION;

	/*  Perform a 64-bit store:  */
#ifdef MODE32
	ppc32_loadstore_indexed
#else
	ppc_loadstore_indexed
#endif
	    [3 + 4](cpu, ic);
}


/*
 *  lvx, stvx:  Vector (16-byte) load/store  (slow implementation)
 *
 *  arg[0] = v-register nr of rs
 *  arg[1] = pointer to ra
 *  arg[2] = pointer to rb
 */
X(lvx)
{
	MODE_uint_t addr = reg(ic->arg[1]) + reg(ic->arg[2]);
	uint8_t data[16];
	uint64_t hi, lo;
	int rs = ic->arg[0];

	if (cpu->memory_rw(cpu, cpu->mem, addr, data, sizeof(data),
	    MEM_READ, CACHE_DATA) != MEMORY_ACCESS_OK) {
		/*  exception  */
		return;
	}

	hi = ((uint64_t)data[0] << 56) +
	     ((uint64_t)data[1] << 48) +
	     ((uint64_t)data[2] << 40) +
	     ((uint64_t)data[3] << 32) +
	     ((uint64_t)data[4] << 24) +
	     ((uint64_t)data[5] << 16) +
	     ((uint64_t)data[6] << 8) +
	     ((uint64_t)data[7]);
	lo = ((uint64_t)data[8] << 56) +
	     ((uint64_t)data[9] << 48) +
	     ((uint64_t)data[10] << 40) +
	     ((uint64_t)data[11] << 32) +
	     ((uint64_t)data[12] << 24) +
	     ((uint64_t)data[13] << 16) +
	     ((uint64_t)data[14] << 8) +
	     ((uint64_t)data[15]);

	cpu->cd.ppc.vr_hi[rs] = hi; cpu->cd.ppc.vr_lo[rs] = lo;
}
X(stvx)
{
	uint8_t data[16];
	MODE_uint_t addr = reg(ic->arg[1]) + reg(ic->arg[2]);
	int rs = ic->arg[0];
	uint64_t hi = cpu->cd.ppc.vr_hi[rs], lo = cpu->cd.ppc.vr_lo[rs];

	data[0] = hi >> 56;
	data[1] = hi >> 48;
	data[2] = hi >> 40;
	data[3] = hi >> 32;
	data[4] = hi >> 24;
	data[5] = hi >> 16;
	data[6] = hi >> 8;
	data[7] = hi;
	data[8] = lo >> 56;
	data[9] = lo >> 48;
	data[10] = lo >> 40;
	data[11] = lo >> 32;
	data[12] = lo >> 24;
	data[13] = lo >> 16;
	data[14] = lo >> 8;
	data[15] = lo;

	cpu->memory_rw(cpu, cpu->mem, addr, data,
	    sizeof(data), MEM_WRITE, CACHE_DATA);
}


/*
 *  vxor:  Vector (16-byte) XOR
 *
 *  arg[0] = v-register nr of source 1
 *  arg[1] = v-register nr of source 2
 *  arg[2] = v-register nr of destination
 */
X(vxor)
{
	cpu->cd.ppc.vr_hi[ic->arg[2]] =
	    cpu->cd.ppc.vr_hi[ic->arg[0]] ^ cpu->cd.ppc.vr_hi[ic->arg[1]];
	cpu->cd.ppc.vr_lo[ic->arg[2]] =
	    cpu->cd.ppc.vr_lo[ic->arg[0]] ^ cpu->cd.ppc.vr_lo[ic->arg[1]];
}


/*
 *  tlbia:  TLB invalidate all
 */
X(tlbia)
{
	fatal("[ tlbia ]\n");
	cpu->invalidate_translation_caches(cpu, 0, INVALIDATE_ALL);
}


/*
 *  tlbie:  TLB invalidate
 */
X(tlbie)
{
	/*  fatal("[ tlbie ]\n");  */
	cpu->invalidate_translation_caches(cpu, reg(ic->arg[0]),
	    INVALIDATE_VADDR);
}


/*
 *  sc: Syscall.
 */
X(sc)
{
	/*  Synchronize the PC (pointing to _after_ this instruction)  */
	cpu->pc = (cpu->pc & ~0xfff) + ic->arg[1];

	ppc_exception(cpu, PPC_EXCEPTION_SC);

	/*  This caused an update to the PC register, so there is no need
	    to worry about the next instruction being an end_of_page.  */
}


/*
 *  openfirmware:
 */
X(openfirmware)
{
	of_emul(cpu);
	if (cpu->running == 0) {
		cpu->n_translated_instrs --;
		cpu->cd.ppc.next_ic = &nothing_call;
	}
	cpu->pc = cpu->cd.ppc.spr[SPR_LR];
	if (cpu->machine->show_trace_tree)
		cpu_functioncall_trace_return(cpu);
	quick_pc_to_pointers(cpu);
}


/*
 *  tlbsx_dot: TLB scan
 */
X(tlbsx_dot)
{
	/*  TODO  */
	cpu->cd.ppc.cr &= ~(0xf0000000);
	cpu->cd.ppc.cr |= 0x20000000;
	cpu->cd.ppc.cr |= ((cpu->cd.ppc.spr[SPR_XER] >> 3) & 0x10000000);
}


/*
 *  tlbli:
 */
X(tlbli)
{
	fatal("tlbli\n");
	cpu->invalidate_translation_caches(cpu, 0, INVALIDATE_ALL);
}


/*
 *  tlbld:
 */
X(tlbld)
{
	/*  MODE_uint_t vaddr = reg(ic->arg[0]);
	    MODE_uint_t paddr = cpu->cd.ppc.spr[SPR_RPA];  */

	fatal("tlbld\n");
	cpu->invalidate_translation_caches(cpu, 0, INVALIDATE_ALL);
}


/*****************************************************************************/


X(end_of_page)
{
	/*  Update the PC:  (offset 0, but on the next page)  */
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (PPC_IC_ENTRIES_PER_PAGE << PPC_INSTR_ALIGNMENT_SHIFT);

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);

	/*  end_of_page doesn't count as an executed instruction:  */
	cpu->n_translated_instrs --;
}

#endif

