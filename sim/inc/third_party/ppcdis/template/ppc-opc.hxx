/* ppc-opc.c -- PowerPC opcode list
   Copyright 1994, 1995, 1996, 1997, 1998, 2000, 2001, 2002, 2003, 2004,
   2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012
   Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support

   This file is part of the GNU opcodes library.

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
   MA 02110-1301, USA.  */

// NOTE:
// Changes for ppcsimbooke :-
//  1. Renamed original ppc-opc.c to ppc-opc.h, since it's going to be used as a header only 
//  2. Retagged all "const struct" type'ish tables/constants to "static const"
//  3. Inserted 
//         #ifndef _PPC_OPC_H
//         #define _PPC_OPC_H
//     type statements, now we have converted the C file to header file
//  4. This file now only acts as a template file. Actual header file gets generate later on.
//  5. FIXME :
//     Removed 8 instrs :-
//         mtibatu   mfibatu
//         mtibatl   mfibatl
//         mtdbatu   mfdbatu
//         mtdbatl   mfdbatl
//
//     There seems to be some bug in the original GNU binutils ppc-opc.c file due to which
//     mtibat* opcodes are selected for IVORs > 32. The issue is that of proper ordering.
//     Since we didn't need these opcodes, we removed them entirely, although the problem
//     could have been solved just by moving the entries corresponding to these instrs a
//     few rows lower.
//
//     Removed all b**+ instrs ( branch mnemonics with forward jump hint )
//     Removed all b**- instrs ( branch mnemonics with backward jump hint )
//
//     25 Aug, 2012
//     Removed all mfspr/mtspr extended mnemonics.
//     Removed all branch extended mnemonics.
//     Removed all trap(primary opcode 2 and 3) extended mnemonics.
//     Removed all possible extended mnemonics.

#ifndef _PPC_OPC_H
#define _PPC_OPC_H

/* This file holds the PowerPC opcode table.  The opcode table
   includes almost all of the extended instruction mnemonics.  This
   permits the disassembler to use them, and simplifies the assembler
   logic, at the cost of increasing the table size.  The table is
   strictly constant data, so the compiler should be able to put it in
   the .text section.

   This file also holds the operand table.  All knowledge about
   inserting operands into instructions and vice-versa is kept in this
   file.  */

/* Local insertion and extraction functions.  */

static unsigned long insert_bat (unsigned long, long, ppc_cpu_t, const char **);
static long extract_bat (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_bba (unsigned long, long, ppc_cpu_t, const char **);
static long extract_bba (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_bdm (unsigned long, long, ppc_cpu_t, const char **);
static long extract_bdm (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_bdp (unsigned long, long, ppc_cpu_t, const char **);
static long extract_bdp (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_bo (unsigned long, long, ppc_cpu_t, const char **);
static long extract_bo (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_boe (unsigned long, long, ppc_cpu_t, const char **);
static long extract_boe (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_fxm (unsigned long, long, ppc_cpu_t, const char **);
static long extract_fxm (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_ls (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_mbe (unsigned long, long, ppc_cpu_t, const char **);
static long extract_mbe (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_mb6 (unsigned long, long, ppc_cpu_t, const char **);
static long extract_mb6 (unsigned long, ppc_cpu_t, int *);
static long extract_nb (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_nbi (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_nsi (unsigned long, long, ppc_cpu_t, const char **);
static long extract_nsi (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_ral (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_ram (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_raq (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_ras (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_rbs (unsigned long, long, ppc_cpu_t, const char **);
static long extract_rbs (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_rbx (unsigned long, long, ppc_cpu_t, const char **);
static unsigned long insert_sh6 (unsigned long, long, ppc_cpu_t, const char **);
static long extract_sh6 (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_spr (unsigned long, long, ppc_cpu_t, const char **);
static long extract_spr (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_sprg (unsigned long, long, ppc_cpu_t, const char **);
static long extract_sprg (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_tbr (unsigned long, long, ppc_cpu_t, const char **);
static long extract_tbr (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_xt6 (unsigned long, long, ppc_cpu_t, const char **);
static long extract_xt6 (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_xa6 (unsigned long, long, ppc_cpu_t, const char **);
static long extract_xa6 (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_xb6 (unsigned long, long, ppc_cpu_t, const char **);
static long extract_xb6 (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_xb6s (unsigned long, long, ppc_cpu_t, const char **);
static long extract_xb6s (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_xc6 (unsigned long, long, ppc_cpu_t, const char **);
static long extract_xc6 (unsigned long, ppc_cpu_t, int *);
static unsigned long insert_dm (unsigned long, long, ppc_cpu_t, const char **);
static long extract_dm (unsigned long, ppc_cpu_t, int *);

/* The operands table.

   The fields are bitm, shift, insert, extract, flags.

   We used to put parens around the various additions, like the one
   for BA just below.  However, that caused trouble with feeble
   compilers with a limit on depth of a parenthesized expression, like
   (reportedly) the compiler in Microsoft Developer Studio 5.  So we
   omit the parens, since the macros are never used in a context where
   the addition will be ambiguous.  */

static const struct powerpc_operand powerpc_operands[] =
{
  /* The zero index is used to indicate the end of the list of
     operands.  */
#define UNUSED 0
  { 0, 0, NULL, NULL, 0 },

  /* The BA field in an XL form instruction.  */
#define BA UNUSED + 1
  /* The BI field in a B form or XL form instruction.  */
#define BI BA
#define BI_MASK (0x1f << 16)
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_CR },

  /* The BA field in an XL form instruction when it must be the same
     as the BT field in the same instruction.  */
#define BAT BA + 1
  { 0x1f, 16, insert_bat, extract_bat, PPC_OPERAND_FAKE },

  /* The BB field in an XL form instruction.  */
#define BB BAT + 1
#define BB_MASK (0x1f << 11)
  { 0x1f, 11, NULL, NULL, PPC_OPERAND_CR },

  /* The BB field in an XL form instruction when it must be the same
     as the BA field in the same instruction.  */
#define BBA BB + 1
  { 0x1f, 11, insert_bba, extract_bba, PPC_OPERAND_FAKE },

  /* The BD field in a B form instruction.  The lower two bits are
     forced to zero.  */
#define BD BBA + 1
  { 0xfffc, 0, NULL, NULL, PPC_OPERAND_RELATIVE | PPC_OPERAND_SIGNED },

  /* The BD field in a B form instruction when absolute addressing is
     used.  */
#define BDA BD + 1
  { 0xfffc, 0, NULL, NULL, PPC_OPERAND_ABSOLUTE | PPC_OPERAND_SIGNED },

  /* The BD field in a B form instruction when the - modifier is used.
     This sets the y bit of the BO field appropriately.  */
#define BDM BDA + 1
  { 0xfffc, 0, insert_bdm, extract_bdm,
      PPC_OPERAND_RELATIVE | PPC_OPERAND_SIGNED },

  /* The BD field in a B form instruction when the - modifier is used
     and absolute address is used.  */
#define BDMA BDM + 1
  { 0xfffc, 0, insert_bdm, extract_bdm,
      PPC_OPERAND_ABSOLUTE | PPC_OPERAND_SIGNED },

  /* The BD field in a B form instruction when the + modifier is used.
     This sets the y bit of the BO field appropriately.  */
#define BDP BDMA + 1
  { 0xfffc, 0, insert_bdp, extract_bdp,
      PPC_OPERAND_RELATIVE | PPC_OPERAND_SIGNED },

  /* The BD field in a B form instruction when the + modifier is used
     and absolute addressing is used.  */
#define BDPA BDP + 1
  { 0xfffc, 0, insert_bdp, extract_bdp,
      PPC_OPERAND_ABSOLUTE | PPC_OPERAND_SIGNED },

  /* The BF field in an X or XL form instruction.  */
#define BF BDPA + 1
  /* The CRFD field in an X form instruction.  */
#define CRFD BF
  { 0x7, 23, NULL, NULL, PPC_OPERAND_CR },

  /* The BF field in an X or XL form instruction.  */
#define BFF BF + 1
  { 0x7, 23, NULL, NULL, 0 },

  /* An optional BF field.  This is used for comparison instructions,
     in which an omitted BF field is taken as zero.  */
#define OBF BFF + 1
  { 0x7, 23, NULL, NULL, PPC_OPERAND_CR | PPC_OPERAND_OPTIONAL },

  /* The BFA field in an X or XL form instruction.  */
#define BFA OBF + 1
  { 0x7, 18, NULL, NULL, PPC_OPERAND_CR },

  /* The BO field in a B form instruction.  Certain values are
     illegal.  */
#define BO BFA + 1
#define BO_MASK (0x1f << 21)
  { 0x1f, 21, insert_bo, extract_bo, 0 },

  /* The BO field in a B form instruction when the + or - modifier is
     used.  This is like the BO field, but it must be even.  */
#define BOE BO + 1
  { 0x1e, 21, insert_boe, extract_boe, 0 },

#define BH BOE + 1
  { 0x3, 11, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The BT field in an X or XL form instruction.  */
#define BT BH + 1
  { 0x1f, 21, NULL, NULL, PPC_OPERAND_CR },

  /* The condition register number portion of the BI field in a B form
     or XL form instruction.  This is used for the extended
     conditional branch mnemonics, which set the lower two bits of the
     BI field.  This field is optional.  */
#define CR BT + 1
  { 0x7, 18, NULL, NULL, PPC_OPERAND_CR | PPC_OPERAND_OPTIONAL },

  /* The CRB field in an X form instruction.  */
#define CRB CR + 1
  /* The MB field in an M form instruction.  */
#define MB CRB
#define MB_MASK (0x1f << 6)
  { 0x1f, 6, NULL, NULL, 0 },

  /* The CRFS field in an X form instruction.  */
#define CRFS CRB + 1
  { 0x7, 0, NULL, NULL, PPC_OPERAND_CR },

  /* The CT field in an X form instruction.  */
#define CT CRFS + 1
  /* The MO field in an mbar instruction.  */
#define MO CT
  { 0x1f, 21, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The D field in a D form instruction.  This is a displacement off
     a register, and implies that the next operand is a register in
     parentheses.  */
#define D CT + 1
  { 0xffff, 0, NULL, NULL, PPC_OPERAND_PARENS | PPC_OPERAND_SIGNED },

  /* The DQ field in a DQ form instruction.  This is like D, but the
     lower four bits are forced to zero. */
#define DQ D + 1
  { 0xfff0, 0, NULL, NULL,
    PPC_OPERAND_PARENS | PPC_OPERAND_SIGNED | PPC_OPERAND_DQ },

  /* The DS field in a DS form instruction.  This is like D, but the
     lower two bits are forced to zero.  */
#define DS DQ + 1
  { 0xfffc, 0, NULL, NULL,
    PPC_OPERAND_PARENS | PPC_OPERAND_SIGNED | PPC_OPERAND_DS },

  /* The DUIS field in a XFX form instruction, 10 bits unsigned imediate */
#define DUIS DS + 1
  { 0x3ff, 11, NULL, NULL, 0 },

  /* The E field in a wrteei instruction.  */
  /* And the W bit in the pair singles instructions.  */
#define E DUIS + 1
#define PSW E
  { 0x1, 15, NULL, NULL, 0 },

  /* The FL1 field in a POWER SC form instruction.  */
#define FL1 E + 1
  /* The U field in an X form instruction.  */
#define U FL1
  { 0xf, 12, NULL, NULL, 0 },

  /* The FL2 field in a POWER SC form instruction.  */
#define FL2 FL1 + 1
  { 0x7, 2, NULL, NULL, 0 },

  /* The FLM field in an XFL form instruction.  */
#define FLM FL2 + 1
  { 0xff, 17, NULL, NULL, 0 },

  /* The FRA field in an X or A form instruction.  */
#define FRA FLM + 1
#define FRA_MASK (0x1f << 16)
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_FPR },

  /* The FRAp field of DFP instructions.  */
#define FRAp FRA + 1
  { 0x1e, 16, NULL, NULL, PPC_OPERAND_FPR },

  /* The FRB field in an X or A form instruction.  */
#define FRB FRAp + 1
#define FRB_MASK (0x1f << 11)
  { 0x1f, 11, NULL, NULL, PPC_OPERAND_FPR },

  /* The FRBp field of DFP instructions.  */
#define FRBp FRB + 1
  { 0x1e, 11, NULL, NULL, PPC_OPERAND_FPR },

  /* The FRC field in an A form instruction.  */
#define FRC FRBp + 1
#define FRC_MASK (0x1f << 6)
  { 0x1f, 6, NULL, NULL, PPC_OPERAND_FPR },

  /* The FRS field in an X form instruction or the FRT field in a D, X
     or A form instruction.  */
#define FRS FRC + 1
#define FRT FRS
  { 0x1f, 21, NULL, NULL, PPC_OPERAND_FPR },

  /* The FRSp field of stfdp or the FRTp field of lfdp and DFP
     instructions.  */
#define FRSp FRS + 1
#define FRTp FRSp
  { 0x1e, 21, NULL, NULL, PPC_OPERAND_FPR },

  /* The FXM field in an XFX instruction.  */
#define FXM FRSp + 1
  { 0xff, 12, insert_fxm, extract_fxm, 0 },

  /* Power4 version for mfcr.  */
#define FXM4 FXM + 1
  { 0xff, 12, insert_fxm, extract_fxm, PPC_OPERAND_OPTIONAL },

  /* The L field in a D or X form instruction.  */
#define L FXM4 + 1
  { 0x1, 21, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The LEV field in a POWER SVC form instruction.  */
#define SVC_LEV L + 1
  { 0x7f, 5, NULL, NULL, 0 },

  /* The LEV field in an SC form instruction.  */
#define LEV SVC_LEV + 1
  { 0x7f, 5, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The LI field in an I form instruction.  The lower two bits are
     forced to zero.  */
#define LI LEV + 1
  { 0x3fffffc, 0, NULL, NULL, PPC_OPERAND_RELATIVE | PPC_OPERAND_SIGNED },

  /* The LI field in an I form instruction when used as an absolute
     address.  */
#define LIA LI + 1
  { 0x3fffffc, 0, NULL, NULL, PPC_OPERAND_ABSOLUTE | PPC_OPERAND_SIGNED },

  /* The LS or WC field in an X (sync or wait) form instruction.  */
#define LS LIA + 1
#define WC LS
  { 0x3, 21, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The ME field in an M form instruction.  */
#define ME LS + 1
#define ME_MASK (0x1f << 1)
  { 0x1f, 1, NULL, NULL, 0 },

  /* The MB and ME fields in an M form instruction expressed a single
     operand which is a bitmask indicating which bits to select.  This
     is a two operand form using PPC_OPERAND_NEXT.  See the
     description in opcode/ppc.h for what this means.  */
#define MBE ME + 1
  { 0x1f, 6, NULL, NULL, PPC_OPERAND_OPTIONAL | PPC_OPERAND_NEXT },
  { -1, 0, insert_mbe, extract_mbe, 0 },

  /* The MB or ME field in an MD or MDS form instruction.  The high
     bit is wrapped to the low end.  */
#define MB6 MBE + 2
#define ME6 MB6
#define MB6_MASK (0x3f << 5)
  { 0x3f, 5, insert_mb6, extract_mb6, 0 },

  /* The NB field in an X form instruction.  The value 32 is stored as
     0.  */
#define NB MB6 + 1
  { 0x1f, 11, NULL, extract_nb, PPC_OPERAND_PLUS1 },

  /* The NBI field in an lswi instruction, which has special value
     restrictions.  The value 32 is stored as 0.  */
#define NBI NB + 1
  { 0x1f, 11, insert_nbi, extract_nb, PPC_OPERAND_PLUS1 },

  /* The NSI field in a D form instruction.  This is the same as the
     SI field, only negated.  */
#define NSI NBI + 1
  { 0xffff, 0, insert_nsi, extract_nsi,
      PPC_OPERAND_NEGATIVE | PPC_OPERAND_SIGNED },

  /* The RA field in an D, DS, DQ, X, XO, M, or MDS form instruction.  */
#define RA NSI + 1
#define RA_MASK (0x1f << 16)
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_GPR },

  /* As above, but 0 in the RA field means zero, not r0.  */
#define RA0 RA + 1
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_GPR_0 },

  /* The RA field in the DQ form lq or an lswx instruction, which have special
     value restrictions.  */
#define RAQ RA0 + 1
#define RAX RAQ
  { 0x1f, 16, insert_raq, NULL, PPC_OPERAND_GPR_0 },

  /* The RA field in a D or X form instruction which is an updating
     load, which means that the RA field may not be zero and may not
     equal the RT field.  */
#define RAL RAQ + 1
  { 0x1f, 16, insert_ral, NULL, PPC_OPERAND_GPR_0 },

  /* The RA field in an lmw instruction, which has special value
     restrictions.  */
#define RAM RAL + 1
  { 0x1f, 16, insert_ram, NULL, PPC_OPERAND_GPR_0 },

  /* The RA field in a D or X form instruction which is an updating
     store or an updating floating point load, which means that the RA
     field may not be zero.  */
#define RAS RAM + 1
  { 0x1f, 16, insert_ras, NULL, PPC_OPERAND_GPR_0 },

  /* The RA field of the tlbwe, dccci and iccci instructions,
     which are optional.  */
#define RAOPT RAS + 1
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_GPR | PPC_OPERAND_OPTIONAL },

  /* The RB field in an X, XO, M, or MDS form instruction.  */
#define RB RAOPT + 1
#define RB_MASK (0x1f << 11)
  { 0x1f, 11, NULL, NULL, PPC_OPERAND_GPR },

  /* The RB field in an X form instruction when it must be the same as
     the RS field in the instruction.  This is used for extended
     mnemonics like mr.  */
#define RBS RB + 1
  { 0x1f, 11, insert_rbs, extract_rbs, PPC_OPERAND_FAKE },

  /* The RB field in an lswx instruction, which has special value
     restrictions.  */
#define RBX RBS + 1
  { 0x1f, 11, insert_rbx, NULL, PPC_OPERAND_GPR },

  /* The RB field of the dccci and iccci instructions, which are optional.  */
#define RBOPT RBX + 1
  { 0x1f, 11, NULL, NULL, PPC_OPERAND_GPR | PPC_OPERAND_OPTIONAL },

  /* The RS field in a D, DS, X, XFX, XS, M, MD or MDS form
     instruction or the RT field in a D, DS, X, XFX or XO form
     instruction.  */
#define RS RBOPT + 1
#define RT RS
#define RT_MASK (0x1f << 21)
  { 0x1f, 21, NULL, NULL, PPC_OPERAND_GPR },

  /* The RS and RT fields of the DS form stq instruction, which have
     special value restrictions.  */
#define RSQ RS + 1
#define RTQ RSQ
  { 0x1e, 21, NULL, NULL, PPC_OPERAND_GPR_0 },

  /* The RS field of the tlbwe instruction, which is optional.  */
#define RSO RSQ + 1
#define RTO RSO
  { 0x1f, 21, NULL, NULL, PPC_OPERAND_GPR | PPC_OPERAND_OPTIONAL },

  /* The SH field in an X or M form instruction.  */
#define SH RSO + 1
#define SH_MASK (0x1f << 11)
  /* The other UIMM field in a EVX form instruction.  */
#define EVUIMM SH
  { 0x1f, 11, NULL, NULL, 0 },

  /* The SH field in an MD form instruction.  This is split.  */
#define SH6 SH + 1
#define SH6_MASK ((0x1f << 11) | (1 << 1))
  { 0x3f, -1, insert_sh6, extract_sh6, 0 },

  /* The SH field of the tlbwe instruction, which is optional.  */
#define SHO SH6 + 1
  { 0x1f, 11, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The SI field in a D form instruction.  */
#define SI SHO + 1
  { 0xffff, 0, NULL, NULL, PPC_OPERAND_SIGNED },

  /* The SI field in a D form instruction when we accept a wide range
     of positive values.  */
#define SISIGNOPT SI + 1
  { 0xffff, 0, NULL, NULL, PPC_OPERAND_SIGNED | PPC_OPERAND_SIGNOPT },

  /* The SPR field in an XFX form instruction.  This is flipped--the
     lower 5 bits are stored in the upper 5 and vice- versa.  */
#define SPR SISIGNOPT + 1
#define PMR SPR
#define TMR SPR
#define SPR_MASK (0x3ff << 11)
  { 0x3ff, 11, insert_spr, extract_spr, 0 },

  /* The BAT index number in an XFX form m[ft]ibat[lu] instruction.  */
#define SPRBAT SPR + 1
#define SPRBAT_MASK (0x3 << 17)
  { 0x3, 17, NULL, NULL, 0 },

  /* The SPRG register number in an XFX form m[ft]sprg instruction.  */
#define SPRG SPRBAT + 1
  { 0x1f, 16, insert_sprg, extract_sprg, 0 },

  /* The SR field in an X form instruction.  */
#define SR SPRG + 1
  { 0xf, 16, NULL, NULL, 0 },

  /* The STRM field in an X AltiVec form instruction.  */
#define STRM SR + 1
  /* The T field in a tlbilx form instruction.  */
#define T STRM
  { 0x3, 21, NULL, NULL, 0 },

  /* The ESYNC field in an X (sync) form instruction.  */
#define ESYNC STRM + 1
  { 0xf, 16, insert_ls, NULL, PPC_OPERAND_OPTIONAL },

  /* The SV field in a POWER SC form instruction.  */
#define SV ESYNC + 1
  { 0x3fff, 2, NULL, NULL, 0 },

  /* The TBR field in an XFX form instruction.  This is like the SPR
     field, but it is optional.  */
#define TBR SV + 1
  { 0x3ff, 11, insert_tbr, extract_tbr, PPC_OPERAND_OPTIONAL },

  /* The TO field in a D or X form instruction.  */
#define TO TBR + 1
#define DUI TO
#define TO_MASK (0x1f << 21)
  { 0x1f, 21, NULL, NULL, 0 },

  /* The UI field in a D form instruction.  */
#define UI TO + 1
  { 0xffff, 0, NULL, NULL, 0 },

  /* The VA field in a VA, VX or VXR form instruction.  */
#define VA UI + 1
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_VR },

  /* The VB field in a VA, VX or VXR form instruction.  */
#define VB VA + 1
  { 0x1f, 11, NULL, NULL, PPC_OPERAND_VR },

  /* The VC field in a VA form instruction.  */
#define VC VB + 1
  { 0x1f, 6, NULL, NULL, PPC_OPERAND_VR },

  /* The VD or VS field in a VA, VX, VXR or X form instruction.  */
#define VD VC + 1
#define VS VD
  { 0x1f, 21, NULL, NULL, PPC_OPERAND_VR },

  /* The SIMM field in a VX form instruction, and TE in Z form.  */
#define SIMM VD + 1
#define TE SIMM
  { 0x1f, 16, NULL, NULL, PPC_OPERAND_SIGNED},

  /* The UIMM field in a VX form instruction.  */
#define UIMM SIMM + 1
#define DCTL UIMM
  { 0x1f, 16, NULL, NULL, 0 },

  /* The SHB field in a VA form instruction.  */
#define SHB UIMM + 1
  { 0xf, 6, NULL, NULL, 0 },

  /* The other UIMM field in a half word EVX form instruction.  */
#define EVUIMM_2 SHB + 1
  { 0x3e, 10, NULL, NULL, PPC_OPERAND_PARENS },

  /* The other UIMM field in a word EVX form instruction.  */
#define EVUIMM_4 EVUIMM_2 + 1
  { 0x7c, 9, NULL, NULL, PPC_OPERAND_PARENS },

  /* The other UIMM field in a double EVX form instruction.  */
#define EVUIMM_8 EVUIMM_4 + 1
  { 0xf8, 8, NULL, NULL, PPC_OPERAND_PARENS },

  /* The WS field.  */
#define WS EVUIMM_8 + 1
  { 0x7, 11, NULL, NULL, 0 },

  /* PowerPC paired singles extensions.  */
  /* W bit in the pair singles instructions for x type instructions.  */
#define PSWM WS + 1
  {  0x1, 10, 0, 0, 0 },

  /* IDX bits for quantization in the pair singles instructions.  */
#define PSQ PSWM + 1
  {  0x7, 12, 0, 0, 0 },

  /* IDX bits for quantization in the pair singles x-type instructions.  */
#define PSQM PSQ + 1
  {  0x7, 7, 0, 0, 0 },

  /* Smaller D field for quantization in the pair singles instructions.  */
#define PSD PSQM + 1
  {  0xfff, 0, 0, 0,  PPC_OPERAND_PARENS | PPC_OPERAND_SIGNED },

#define A_L PSD + 1
#define W A_L
#define MTMSRD_L W
  { 0x1, 16, NULL, NULL, PPC_OPERAND_OPTIONAL },

#define RMC MTMSRD_L + 1
  { 0x3, 9, NULL, NULL, 0 },

#define R RMC + 1
  { 0x1, 16, NULL, NULL, 0 },

#define SP R + 1
  { 0x3, 19, NULL, NULL, 0 },

#define S SP + 1
  { 0x1, 20, NULL, NULL, 0 },

  /* SH field starting at bit position 16.  */
#define SH16 S + 1
  /* The DCM and DGM fields in a Z form instruction.  */
#define DCM SH16
#define DGM DCM
  { 0x3f, 10, NULL, NULL, 0 },

  /* The EH field in larx instruction.  */
#define EH SH16 + 1
  { 0x1, 0, NULL, NULL, PPC_OPERAND_OPTIONAL },

  /* The L field in an mtfsf or XFL form instruction.  */
#define XFL_L EH + 1
  { 0x1, 25, NULL, NULL, PPC_OPERAND_OPTIONAL},

  /* Xilinx APU related masks and macros */
#define FCRT XFL_L + 1
#define FCRT_MASK (0x1f << 21)
  { 0x1f, 21, 0, 0, PPC_OPERAND_FCR },

  /* Xilinx FSL related masks and macros */  
#define FSL FCRT + 1
#define FSL_MASK (0x1f << 11)
  { 0x1f, 11, 0, 0, PPC_OPERAND_FSL },  

  /* Xilinx UDI related masks and macros */  
#define URT FSL + 1
  { 0x1f, 21, 0, 0, PPC_OPERAND_UDI },

#define URA URT + 1
  { 0x1f, 16, 0, 0, PPC_OPERAND_UDI },

#define URB URA + 1
  { 0x1f, 11, 0, 0, PPC_OPERAND_UDI },

#define URC URB + 1
  { 0x1f, 6, 0, 0, PPC_OPERAND_UDI },

  /* The XT and XS fields in an XX1 or XX3 form instruction.  This is split.  */
#define XS6 URC + 1
#define XT6 XS6
  { 0x3f, -1, insert_xt6, extract_xt6, PPC_OPERAND_VSR },

  /* The XA field in an XX3 form instruction.  This is split.  */
#define XA6 XT6 + 1
  { 0x3f, -1, insert_xa6, extract_xa6, PPC_OPERAND_VSR },

  /* The XB field in an XX2 or XX3 form instruction.  This is split.  */
#define XB6 XA6 + 1
  { 0x3f, -1, insert_xb6, extract_xb6, PPC_OPERAND_VSR },

  /* The XB field in an XX3 form instruction when it must be the same as
     the XA field in the instruction.  This is used in extended mnemonics
     like xvmovdp.  This is split.  */
#define XB6S XB6 + 1
  { 0x3f, -1, insert_xb6s, extract_xb6s, PPC_OPERAND_FAKE },

  /* The XC field in an XX4 form instruction.  This is split.  */
#define XC6 XB6S + 1
  { 0x3f, -1, insert_xc6, extract_xc6, PPC_OPERAND_VSR },

  /* The DM or SHW field in an XX3 form instruction.  */
#define DM XC6 + 1
#define SHW DM
  { 0x3, 8, NULL, NULL, 0 },

  /* The DM field in an extended mnemonic XX3 form instruction.  */
#define DMEX DM + 1
  { 0x3, 8, insert_dm, extract_dm, 0 },

  /* The UIM field in an XX2 form instruction.  */
#define UIM DMEX + 1
  { 0x3, 16, NULL, NULL, 0 },

#define ERAT_T UIM + 1
  { 0x7, 21, NULL, NULL, 0 },
};

static const unsigned int num_powerpc_operands = (sizeof (powerpc_operands)
					   / sizeof (powerpc_operands[0]));

/* The functions used to insert and extract complicated operands.  */

/* The BA field in an XL form instruction when it must be the same as
   the BT field in the same instruction.  This operand is marked FAKE.
   The insertion function just copies the BT field into the BA field,
   and the extraction function just checks that the fields are the
   same.  */

static unsigned long
insert_bat (unsigned long insn,
	    long value ATTRIBUTE_UNUSED,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | (((insn >> 21) & 0x1f) << 16);
}

static long
extract_bat (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  if (((insn >> 21) & 0x1f) != ((insn >> 16) & 0x1f))
    *invalid = 1;
  return 0;
}

/* The BB field in an XL form instruction when it must be the same as
   the BA field in the same instruction.  This operand is marked FAKE.
   The insertion function just copies the BA field into the BB field,
   and the extraction function just checks that the fields are the
   same.  */

static unsigned long
insert_bba (unsigned long insn,
	    long value ATTRIBUTE_UNUSED,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | (((insn >> 16) & 0x1f) << 11);
}

static long
extract_bba (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  if (((insn >> 16) & 0x1f) != ((insn >> 11) & 0x1f))
    *invalid = 1;
  return 0;
}

/* The BD field in a B form instruction when the - modifier is used.
   This modifier means that the branch is not expected to be taken.
   For chips built to versions of the architecture prior to version 2
   (ie. not Power4 compatible), we set the y bit of the BO field to 1
   if the offset is negative.  When extracting, we require that the y
   bit be 1 and that the offset be positive, since if the y bit is 0
   we just want to print the normal form of the instruction.
   Power4 compatible targets use two bits, "a", and "t", instead of
   the "y" bit.  "at" == 00 => no hint, "at" == 01 => unpredictable,
   "at" == 10 => not taken, "at" == 11 => taken.  The "t" bit is 00001
   in BO field, the "a" bit is 00010 for branch on CR(BI) and 01000
   for branch on CTR.  We only handle the taken/not-taken hint here.
   Note that we don't relax the conditions tested here when
   disassembling with -Many because insns using extract_bdm and
   extract_bdp always occur in pairs.  One or the other will always
   be valid.  */

#define ISA_V2 (PPC_OPCODE_POWER4 | PPC_OPCODE_E500MC | PPC_OPCODE_TITAN)

static unsigned long
insert_bdm (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  if ((dialect & ISA_V2) == 0)
    {
      if ((value & 0x8000) != 0)
	insn |= 1 << 21;
    }
  else
    {
      if ((insn & (0x14 << 21)) == (0x04 << 21))
	insn |= 0x02 << 21;
      else if ((insn & (0x14 << 21)) == (0x10 << 21))
	insn |= 0x08 << 21;
    }
  return insn | (value & 0xfffc);
}

static long
extract_bdm (unsigned long insn,
	     ppc_cpu_t dialect,
	     int *invalid)
{
  if ((dialect & ISA_V2) == 0)
    {
      if (((insn & (1 << 21)) == 0) != ((insn & (1 << 15)) == 0))
	*invalid = 1;
    }
  else
    {
      if ((insn & (0x17 << 21)) != (0x06 << 21)
	  && (insn & (0x1d << 21)) != (0x18 << 21))
	*invalid = 1;
    }

  return ((insn & 0xfffc) ^ 0x8000) - 0x8000;
}

/* The BD field in a B form instruction when the + modifier is used.
   This is like BDM, above, except that the branch is expected to be
   taken.  */

static unsigned long
insert_bdp (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  if ((dialect & ISA_V2) == 0)
    {
      if ((value & 0x8000) == 0)
	insn |= 1 << 21;
    }
  else
    {
      if ((insn & (0x14 << 21)) == (0x04 << 21))
	insn |= 0x03 << 21;
      else if ((insn & (0x14 << 21)) == (0x10 << 21))
	insn |= 0x09 << 21;
    }
  return insn | (value & 0xfffc);
}

static long
extract_bdp (unsigned long insn,
	     ppc_cpu_t dialect,
	     int *invalid)
{
  if ((dialect & ISA_V2) == 0)
    {
      if (((insn & (1 << 21)) == 0) == ((insn & (1 << 15)) == 0))
	*invalid = 1;
    }
  else
    {
      if ((insn & (0x17 << 21)) != (0x07 << 21)
	  && (insn & (0x1d << 21)) != (0x19 << 21))
	*invalid = 1;
    }

  return ((insn & 0xfffc) ^ 0x8000) - 0x8000;
}

static inline int
valid_bo_pre_v2 (long value)
{
  /* Certain encodings have bits that are required to be zero.
     These are (z must be zero, y may be anything):
	 0000y 
	 0001y 
	 001zy
	 0100y 
	 0101y 
	 011zy
	 1z00y
	 1z01y
	 1z1zz
  */
  if ((value & 0x14) == 0)
    return 1;
  else if ((value & 0x14) == 0x4)
    return (value & 0x2) == 0;
  else if ((value & 0x14) == 0x10)
    return (value & 0x8) == 0;
  else
    return value == 0x14;
}

static inline int
valid_bo_post_v2 (long value)
{
  /* Certain encodings have bits that are required to be zero.
     These are (z must be zero, a & t may be anything):
	 0000z
	 0001z
	 001at
	 0100z
	 0101z
	 011at
	 1a00t
	 1a01t
	 1z1zz
  */
  if ((value & 0x14) == 0)
    return (value & 0x1) == 0;
  else if ((value & 0x14) == 0x14)
    return value == 0x14;
  else
    return 1;
}

/* Check for legal values of a BO field.  */

static int
valid_bo (long value, ppc_cpu_t dialect, int extract)
{
  int valid_y = valid_bo_pre_v2 (value);
  int valid_at = valid_bo_post_v2 (value);

  /* When disassembling with -Many, accept either encoding on the
     second pass through opcodes.  */
  if (extract && dialect == ~(ppc_cpu_t) PPC_OPCODE_ANY)
    return valid_y || valid_at;
  if ((dialect & ISA_V2) == 0)
    return valid_y;
  else
    return valid_at;
}

/* The BO field in a B form instruction.  Warn about attempts to set
   the field to an illegal value.  */

static unsigned long
insert_bo (unsigned long insn,
	   long value,
	   ppc_cpu_t dialect,
	   const char **errmsg)
{
  if (!valid_bo (value, dialect, 0))
    *errmsg = _("invalid conditional option");
  else if (PPC_OP (insn) == 19 && (insn & 0x400) && ! (value & 4))
    *errmsg = _("invalid counter access");
  return insn | ((value & 0x1f) << 21);
}

static long
extract_bo (unsigned long insn,
	    ppc_cpu_t dialect,
	    int *invalid)
{
  long value;

  value = (insn >> 21) & 0x1f;
  if (!valid_bo (value, dialect, 1))
    *invalid = 1;
  return value;
}

/* The BO field in a B form instruction when the + or - modifier is
   used.  This is like the BO field, but it must be even.  When
   extracting it, we force it to be even.  */

static unsigned long
insert_boe (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect,
	    const char **errmsg)
{
  if (!valid_bo (value, dialect, 0))
    *errmsg = _("invalid conditional option");
  else if (PPC_OP (insn) == 19 && (insn & 0x400) && ! (value & 4))
    *errmsg = _("invalid counter access");
  else if ((value & 1) != 0)
    *errmsg = _("attempt to set y bit when using + or - modifier");

  return insn | ((value & 0x1f) << 21);
}

static long
extract_boe (unsigned long insn,
	     ppc_cpu_t dialect,
	     int *invalid)
{
  long value;

  value = (insn >> 21) & 0x1f;
  if (!valid_bo (value, dialect, 1))
    *invalid = 1;
  return value & 0x1e;
}

/* FXM mask in mfcr and mtcrf instructions.  */

static unsigned long
insert_fxm (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect,
	    const char **errmsg)
{
  /* If we're handling the mfocrf and mtocrf insns ensure that exactly
     one bit of the mask field is set.  */
  if ((insn & (1 << 20)) != 0)
    {
      if (value == 0 || (value & -value) != value)
	{
	  *errmsg = _("invalid mask field");
	  value = 0;
	}
    }

  /* If the optional field on mfcr is missing that means we want to use
     the old form of the instruction that moves the whole cr.  In that
     case we'll have VALUE zero.  There doesn't seem to be a way to
     distinguish this from the case where someone writes mfcr %r3,0.  */
  else if (value == 0)
    ;

  /* If only one bit of the FXM field is set, we can use the new form
     of the instruction, which is faster.  Unlike the Power4 branch hint
     encoding, this is not backward compatible.  Do not generate the
     new form unless -mpower4 has been given, or -many and the two
     operand form of mfcr was used.  */
  else if ((value & -value) == value
	   && ((dialect & PPC_OPCODE_POWER4) != 0
	       || ((dialect & PPC_OPCODE_ANY) != 0
		   && (insn & (0x3ff << 1)) == 19 << 1)))
    insn |= 1 << 20;

  /* Any other value on mfcr is an error.  */
  else if ((insn & (0x3ff << 1)) == 19 << 1)
    {
      *errmsg = _("ignoring invalid mfcr mask");
      value = 0;
    }

  return insn | ((value & 0xff) << 12);
}

static long
extract_fxm (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  long mask = (insn >> 12) & 0xff;

  /* Is this a Power4 insn?  */
  if ((insn & (1 << 20)) != 0)
    {
      /* Exactly one bit of MASK should be set.  */
      if (mask == 0 || (mask & -mask) != mask)
	*invalid = 1;
    }

  /* Check that non-power4 form of mfcr has a zero MASK.  */
  else if ((insn & (0x3ff << 1)) == 19 << 1)
    {
      if (mask != 0)
	*invalid = 1;
    }

  return mask;
}

/* The LS field in a sync instruction that accepts 2 operands
   Values 2 and 3 are reserved,
     must be treated as 0 for future compatibility
   Values 0 and 1 can be accepted, if field ESYNC is zero
   Otherwise L = complement of ESYNC-bit2 (1<<18) */

static unsigned long
insert_ls (unsigned long insn,
	   long value,
	   ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	   const char **errmsg ATTRIBUTE_UNUSED)
{
  unsigned long ls;

  ls = (insn >> 21) & 0x03;
  if (value == 0)
    {
      if (ls > 1)
	return insn & ~(0x3 << 21);
      return insn;
    }
  if ((value & 0x2) != 0)
    return (insn & ~(0x3 << 21)) | ((value & 0xf) << 16);
  return (insn & ~(0x3 << 21)) | (0x1 << 21) | ((value & 0xf) << 16);
}

/* The MB and ME fields in an M form instruction expressed as a single
   operand which is itself a bitmask.  The extraction function always
   marks it as invalid, since we never want to recognize an
   instruction which uses a field of this type.  */

static unsigned long
insert_mbe (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg)
{
  unsigned long uval, mask;
  int mb, me, mx, count, last;

  uval = value;

  if (uval == 0)
    {
      *errmsg = _("illegal bitmask");
      return insn;
    }

  mb = 0;
  me = 32;
  if ((uval & 1) != 0)
    last = 1;
  else
    last = 0;
  count = 0;

  /* mb: location of last 0->1 transition */
  /* me: location of last 1->0 transition */
  /* count: # transitions */

  for (mx = 0, mask = 1L << 31; mx < 32; ++mx, mask >>= 1)
    {
      if ((uval & mask) && !last)
	{
	  ++count;
	  mb = mx;
	  last = 1;
	}
      else if (!(uval & mask) && last)
	{
	  ++count;
	  me = mx;
	  last = 0;
	}
    }
  if (me == 0)
    me = 32;

  if (count != 2 && (count != 0 || ! last))
    *errmsg = _("illegal bitmask");

  return insn | (mb << 6) | ((me - 1) << 1);
}

static long
extract_mbe (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  long ret;
  int mb, me;
  int i;

  *invalid = 1;

  mb = (insn >> 6) & 0x1f;
  me = (insn >> 1) & 0x1f;
  if (mb < me + 1)
    {
      ret = 0;
      for (i = mb; i <= me; i++)
	ret |= 1L << (31 - i);
    }
  else if (mb == me + 1)
    ret = ~0;
  else /* (mb > me + 1) */
    {
      ret = ~0;
      for (i = me + 1; i < mb; i++)
	ret &= ~(1L << (31 - i));
    }
  return ret;
}

/* The MB or ME field in an MD or MDS form instruction.  The high bit
   is wrapped to the low end.  */

static unsigned long
insert_mb6 (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 6) | (value & 0x20);
}

static long
extract_mb6 (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 6) & 0x1f) | (insn & 0x20);
}

/* The NB field in an X form instruction.  The value 32 is stored as
   0.  */

static long
extract_nb (unsigned long insn,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    int *invalid ATTRIBUTE_UNUSED)
{
  long ret;

  ret = (insn >> 11) & 0x1f;
  if (ret == 0)
    ret = 32;
  return ret;
}

/* The NB field in an lswi instruction, which has special value
   restrictions.  The value 32 is stored as 0.  */

static unsigned long
insert_nbi (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  long rtvalue = (insn & RT_MASK) >> 21;
  long ravalue = (insn & RA_MASK) >> 16;

  if (value == 0)
    value = 32;
  if (rtvalue + (value + 3) / 4 > (rtvalue > ravalue ? ravalue + 32
						     : ravalue))
    *errmsg = _("address register in load range");
  return insn | ((value & 0x1f) << 11);
}

/* The NSI field in a D form instruction.  This is the same as the SI
   field, only negated.  The extraction function always marks it as
   invalid, since we never want to recognize an instruction which uses
   a field of this type.  */

static unsigned long
insert_nsi (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | (-value & 0xffff);
}

static long
extract_nsi (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  *invalid = 1;
  return -(((insn & 0xffff) ^ 0x8000) - 0x8000);
}

/* The RA field in a D or X form instruction which is an updating
   load, which means that the RA field may not be zero and may not
   equal the RT field.  */

static unsigned long
insert_ral (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg)
{
  if (value == 0
      || (unsigned long) value == ((insn >> 21) & 0x1f))
    *errmsg = "invalid register operand when updating";
  return insn | ((value & 0x1f) << 16);
}

/* The RA field in an lmw instruction, which has special value
   restrictions.  */

static unsigned long
insert_ram (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg)
{
  if ((unsigned long) value >= ((insn >> 21) & 0x1f))
    *errmsg = _("index register in load range");
  return insn | ((value & 0x1f) << 16);
}

/* The RA field in the DQ form lq or an lswx instruction, which have special
   value restrictions.  */

static unsigned long
insert_raq (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg)
{
  long rtvalue = (insn & RT_MASK) >> 21;

  if (value == rtvalue)
    *errmsg = _("source and target register operands must be different");
  return insn | ((value & 0x1f) << 16);
}

/* The RA field in a D or X form instruction which is an updating
   store or an updating floating point load, which means that the RA
   field may not be zero.  */

static unsigned long
insert_ras (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg)
{
  if (value == 0)
    *errmsg = _("invalid register operand when updating");
  return insn | ((value & 0x1f) << 16);
}

/* The RB field in an X form instruction when it must be the same as
   the RS field in the instruction.  This is used for extended
   mnemonics like mr.  This operand is marked FAKE.  The insertion
   function just copies the BT field into the BA field, and the
   extraction function just checks that the fields are the same.  */

static unsigned long
insert_rbs (unsigned long insn,
	    long value ATTRIBUTE_UNUSED,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | (((insn >> 21) & 0x1f) << 11);
}

static long
extract_rbs (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  if (((insn >> 21) & 0x1f) != ((insn >> 11) & 0x1f))
    *invalid = 1;
  return 0;
}

/* The RB field in an lswx instruction, which has special value
   restrictions.  */

static unsigned long
insert_rbx (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg)
{
  long rtvalue = (insn & RT_MASK) >> 21;

  if (value == rtvalue)
    *errmsg = _("source and target register operands must be different");
  return insn | ((value & 0x1f) << 11);
}

/* The SH field in an MD form instruction.  This is split.  */

static unsigned long
insert_sh6 (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 11) | ((value & 0x20) >> 4);
}

static long
extract_sh6 (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 11) & 0x1f) | ((insn << 4) & 0x20);
}

/* The SPR field in an XFX form instruction.  This is flipped--the
   lower 5 bits are stored in the upper 5 and vice- versa.  */

static unsigned long
insert_spr (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 16) | ((value & 0x3e0) << 6);
}

static long
extract_spr (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn >> 16) & 0x1f) | ((insn >> 6) & 0x3e0);
}

/* Some dialects have 8 SPRG registers instead of the standard 4.  */

static unsigned long
insert_sprg (unsigned long insn,
	     long value,
	     ppc_cpu_t dialect,
	     const char **errmsg)
{
  if (value > 7
      || (value > 3
	  && (dialect & (PPC_OPCODE_BOOKE | PPC_OPCODE_405)) == 0))
    *errmsg = _("invalid sprg number");

  /* If this is mfsprg4..7 then use spr 260..263 which can be read in
     user mode.  Anything else must use spr 272..279.  */
  if (value <= 3 || (insn & 0x100) != 0)
    value |= 0x10;

  return insn | ((value & 0x17) << 16);
}

static long
extract_sprg (unsigned long insn,
	      ppc_cpu_t dialect,
	      int *invalid)
{
  unsigned long val = (insn >> 16) & 0x1f;

  /* mfsprg can use 260..263 and 272..279.  mtsprg only uses spr 272..279
     If not BOOKE or 405, then both use only 272..275.  */
  if ((val - 0x10 > 3 && (dialect & (PPC_OPCODE_BOOKE | PPC_OPCODE_405)) == 0)
      || (val - 0x10 > 7 && (insn & 0x100) != 0)
      || val <= 3
      || (val & 8) != 0)
    *invalid = 1;
  return val & 7;
}

/* The TBR field in an XFX instruction.  This is just like SPR, but it
   is optional.  When TBR is omitted, it must be inserted as 268 (the
   magic number of the TB register).  These functions treat 0
   (indicating an omitted optional operand) as 268.  This means that
   ``mftb 4,0'' is not handled correctly.  This does not matter very
   much, since the architecture manual does not define mftb as
   accepting any values other than 268 or 269.  */

#define TB (268)

static unsigned long
insert_tbr (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  if (value == 0)
    value = TB;
  return insn | ((value & 0x1f) << 16) | ((value & 0x3e0) << 6);
}

static long
extract_tbr (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  long ret;

  ret = ((insn >> 16) & 0x1f) | ((insn >> 6) & 0x3e0);
  if (ret == TB)
    ret = 0;
  return ret;
}

/* The XT and XS fields in an XX1 or XX3 form instruction.  This is split.  */

static unsigned long
insert_xt6 (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 21) | ((value & 0x20) >> 5);
}

static long
extract_xt6 (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn << 5) & 0x20) | ((insn >> 21) & 0x1f);
}

/* The XA field in an XX3 form instruction.  This is split.  */

static unsigned long
insert_xa6 (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 16) | ((value & 0x20) >> 3);
}

static long
extract_xa6 (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn << 3) & 0x20) | ((insn >> 16) & 0x1f);
}

/* The XB field in an XX3 form instruction.  This is split.  */

static unsigned long
insert_xb6 (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 11) | ((value & 0x20) >> 4);
}

static long
extract_xb6 (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn << 4) & 0x20) | ((insn >> 11) & 0x1f);
}

/* The XB field in an XX3 form instruction when it must be the same as
   the XA field in the instruction.  This is used for extended
   mnemonics like xvmovdp.  This operand is marked FAKE.  The insertion
   function just copies the XA field into the XB field, and the
   extraction function just checks that the fields are the same.  */

static unsigned long
insert_xb6s (unsigned long insn,
	    long value ATTRIBUTE_UNUSED,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | (((insn >> 16) & 0x1f) << 11) | (((insn >> 2) & 0x1) << 1);
}

static long
extract_xb6s (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid)
{
  if ((((insn >> 16) & 0x1f) != ((insn >> 11) & 0x1f))
      || (((insn >> 2) & 0x1) != ((insn >> 1) & 0x1)))
    *invalid = 1;
  return 0;
}

/* The XC field in an XX4 form instruction.  This is split.  */

static unsigned long
insert_xc6 (unsigned long insn,
	    long value,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    const char **errmsg ATTRIBUTE_UNUSED)
{
  return insn | ((value & 0x1f) << 6) | ((value & 0x20) >> 2);
}

static long
extract_xc6 (unsigned long insn,
	     ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	     int *invalid ATTRIBUTE_UNUSED)
{
  return ((insn << 2) & 0x20) | ((insn >> 6) & 0x1f);
}

static unsigned long
insert_dm (unsigned long insn,
	   long value,
	   ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	   const char **errmsg)
{
  if (value != 0 && value != 1)
    *errmsg = _("invalid constant");
  return insn | (((value) ? 3 : 0) << 8);
}

static long
extract_dm (unsigned long insn,
	    ppc_cpu_t dialect ATTRIBUTE_UNUSED,
	    int *invalid)
{
  long value;

  value = (insn >> 8) & 3;
  if (value != 0 && value != 3)
    *invalid = 1;
  return (value) ? 1 : 0;
}

/* Macros used to form opcodes.  */

/* The main opcode.  */
#define OP(x) ((((unsigned long)(x)) & 0x3f) << 26)
#define OP_MASK OP (0x3f)

/* The main opcode combined with a trap code in the TO field of a D
   form instruction.  Used for extended mnemonics for the trap
   instructions.  */
#define OPTO(x,to) (OP (x) | ((((unsigned long)(to)) & 0x1f) << 21))
#define OPTO_MASK (OP_MASK | TO_MASK)

/* The main opcode combined with a comparison size bit in the L field
   of a D form or X form instruction.  Used for extended mnemonics for
   the comparison instructions.  */
#define OPL(x,l) (OP (x) | ((((unsigned long)(l)) & 1) << 21))
#define OPL_MASK OPL (0x3f,1)

/* An A form instruction.  */
#define A(op, xop, rc) (OP (op) | ((((unsigned long)(xop)) & 0x1f) << 1) | (((unsigned long)(rc)) & 1))
#define A_MASK A (0x3f, 0x1f, 1)

/* An A_MASK with the FRB field fixed.  */
#define AFRB_MASK (A_MASK | FRB_MASK)

/* An A_MASK with the FRC field fixed.  */
#define AFRC_MASK (A_MASK | FRC_MASK)

/* An A_MASK with the FRA and FRC fields fixed.  */
#define AFRAFRC_MASK (A_MASK | FRA_MASK | FRC_MASK)

/* An AFRAFRC_MASK, but with L bit clear.  */
#define AFRALFRC_MASK (AFRAFRC_MASK & ~((unsigned long) 1 << 16))

/* A B form instruction.  */
#define B(op, aa, lk) (OP (op) | ((((unsigned long)(aa)) & 1) << 1) | ((lk) & 1))
#define B_MASK B (0x3f, 1, 1)

/* A B form instruction setting the BO field.  */
#define BBO(op, bo, aa, lk) (B ((op), (aa), (lk)) | ((((unsigned long)(bo)) & 0x1f) << 21))
#define BBO_MASK BBO (0x3f, 0x1f, 1, 1)

/* A BBO_MASK with the y bit of the BO field removed.  This permits
   matching a conditional branch regardless of the setting of the y
   bit.  Similarly for the 'at' bits used for power4 branch hints.  */
#define Y_MASK	 (((unsigned long) 1) << 21)
#define AT1_MASK (((unsigned long) 3) << 21)
#define AT2_MASK (((unsigned long) 9) << 21)
#define BBOY_MASK  (BBO_MASK &~ Y_MASK)
#define BBOAT_MASK (BBO_MASK &~ AT1_MASK)

/* A B form instruction setting the BO field and the condition bits of
   the BI field.  */
#define BBOCB(op, bo, cb, aa, lk) \
  (BBO ((op), (bo), (aa), (lk)) | ((((unsigned long)(cb)) & 0x3) << 16))
#define BBOCB_MASK BBOCB (0x3f, 0x1f, 0x3, 1, 1)

/* A BBOCB_MASK with the y bit of the BO field removed.  */
#define BBOYCB_MASK (BBOCB_MASK &~ Y_MASK)
#define BBOATCB_MASK (BBOCB_MASK &~ AT1_MASK)
#define BBOAT2CB_MASK (BBOCB_MASK &~ AT2_MASK)

/* A BBOYCB_MASK in which the BI field is fixed.  */
#define BBOYBI_MASK (BBOYCB_MASK | BI_MASK)
#define BBOATBI_MASK (BBOAT2CB_MASK | BI_MASK)

/* An Context form instruction.  */
#define CTX(op, xop)   (OP (op) | (((unsigned long)(xop)) & 0x7))
#define CTX_MASK CTX(0x3f, 0x7)

/* An User Context form instruction.  */
#define UCTX(op, xop)  (OP (op) | (((unsigned long)(xop)) & 0x1f))
#define UCTX_MASK UCTX(0x3f, 0x1f)

/* The main opcode mask with the RA field clear.  */
#define DRA_MASK (OP_MASK | RA_MASK)

/* A DS form instruction.  */
#define DSO(op, xop) (OP (op) | ((xop) & 0x3))
#define DS_MASK DSO (0x3f, 3)

/* An EVSEL form instruction.  */
#define EVSEL(op, xop) (OP (op) | (((unsigned long)(xop)) & 0xff) << 3)
#define EVSEL_MASK EVSEL(0x3f, 0xff)

/* An M form instruction.  */
#define M(op, rc) (OP (op) | ((rc) & 1))
#define M_MASK M (0x3f, 1)

/* An M form instruction with the ME field specified.  */
#define MME(op, me, rc) (M ((op), (rc)) | ((((unsigned long)(me)) & 0x1f) << 1))

/* An M_MASK with the MB and ME fields fixed.  */
#define MMBME_MASK (M_MASK | MB_MASK | ME_MASK)

/* An M_MASK with the SH and ME fields fixed.  */
#define MSHME_MASK (M_MASK | SH_MASK | ME_MASK)

/* An MD form instruction.  */
#define MD(op, xop, rc) (OP (op) | ((((unsigned long)(xop)) & 0x7) << 2) | ((rc) & 1))
#define MD_MASK MD (0x3f, 0x7, 1)

/* An MD_MASK with the MB field fixed.  */
#define MDMB_MASK (MD_MASK | MB6_MASK)

/* An MD_MASK with the SH field fixed.  */
#define MDSH_MASK (MD_MASK | SH6_MASK)

/* An MDS form instruction.  */
#define MDS(op, xop, rc) (OP (op) | ((((unsigned long)(xop)) & 0xf) << 1) | ((rc) & 1))
#define MDS_MASK MDS (0x3f, 0xf, 1)

/* An MDS_MASK with the MB field fixed.  */
#define MDSMB_MASK (MDS_MASK | MB6_MASK)

/* An SC form instruction.  */
#define SC(op, sa, lk) (OP (op) | ((((unsigned long)(sa)) & 1) << 1) | ((lk) & 1))
#define SC_MASK (OP_MASK | (((unsigned long)0x3ff) << 16) | (((unsigned long)1) << 1) | 1)

/* An VX form instruction.  */
#define VX(op, xop) (OP (op) | (((unsigned long)(xop)) & 0x7ff))

/* The mask for an VX form instruction.  */
#define VX_MASK	VX(0x3f, 0x7ff)

/* An VA form instruction.  */
#define VXA(op, xop) (OP (op) | (((unsigned long)(xop)) & 0x03f))

/* The mask for an VA form instruction.  */
#define VXA_MASK VXA(0x3f, 0x3f)

/* An VXR form instruction.  */
#define VXR(op, xop, rc) (OP (op) | (((rc) & 1) << 10) | (((unsigned long)(xop)) & 0x3ff))

/* The mask for a VXR form instruction.  */
#define VXR_MASK VXR(0x3f, 0x3ff, 1)

/* An X form instruction.  */
#define X(op, xop) (OP (op) | ((((unsigned long)(xop)) & 0x3ff) << 1))

/* An XX2 form instruction.  */
#define XX2(op, xop) (OP (op) | ((((unsigned long)(xop)) & 0x1ff) << 2))

/* An XX3 form instruction.  */
#define XX3(op, xop) (OP (op) | ((((unsigned long)(xop)) & 0xff) << 3))

/* An XX3 form instruction with the RC bit specified.  */
#define XX3RC(op, xop, rc) (OP (op) | (((rc) & 1) << 10) | ((((unsigned long)(xop)) & 0x7f) << 3))

/* An XX4 form instruction.  */
#define XX4(op, xop) (OP (op) | ((((unsigned long)(xop)) & 0x3) << 4))

/* A Z form instruction.  */
#define Z(op, xop) (OP (op) | ((((unsigned long)(xop)) & 0x1ff) << 1))

/* An X form instruction with the RC bit specified.  */
#define XRC(op, xop, rc) (X ((op), (xop)) | ((rc) & 1))

/* A Z form instruction with the RC bit specified.  */
#define ZRC(op, xop, rc) (Z ((op), (xop)) | ((rc) & 1))

/* The mask for an X form instruction.  */
#define X_MASK XRC (0x3f, 0x3ff, 1)

/* An X form wait instruction with everything filled in except the WC field.  */
#define XWC_MASK (XRC (0x3f, 0x3ff, 1) | (7 << 23) | RA_MASK | RB_MASK)

/* The mask for an XX1 form instruction.  */
#define XX1_MASK X (0x3f, 0x3ff)

/* The mask for an XX2 form instruction.  */
#define XX2_MASK (XX2 (0x3f, 0x1ff) | (0x1f << 16))

/* The mask for an XX2 form instruction with the UIM bits specified.  */
#define XX2UIM_MASK (XX2 (0x3f, 0x1ff) | (7 << 18))

/* The mask for an XX2 form instruction with the BF bits specified.  */
#define XX2BF_MASK (XX2_MASK | (3 << 21) | (1))

/* The mask for an XX3 form instruction.  */
#define XX3_MASK XX3 (0x3f, 0xff)

/* The mask for an XX3 form instruction with the BF bits specified.  */
#define XX3BF_MASK (XX3 (0x3f, 0xff) | (3 << 21) | (1))

/* The mask for an XX3 form instruction with the DM or SHW bits specified.  */
#define XX3DM_MASK (XX3 (0x3f, 0x1f) | (1 << 10))
#define XX3SHW_MASK XX3DM_MASK

/* The mask for an XX4 form instruction.  */
#define XX4_MASK XX4 (0x3f, 0x3)

/* An X form wait instruction with everything filled in except the WC field.  */
#define XWC_MASK (XRC (0x3f, 0x3ff, 1) | (7 << 23) | RA_MASK | RB_MASK)

/* The mask for a Z form instruction.  */
#define Z_MASK ZRC (0x3f, 0x1ff, 1)
#define Z2_MASK ZRC (0x3f, 0xff, 1)

/* An X_MASK with the RA field fixed.  */
#define XRA_MASK (X_MASK | RA_MASK)

/* An XRA_MASK with the W field clear.  */
#define XWRA_MASK (XRA_MASK & ~((unsigned long) 1 << 16))

/* An X_MASK with the RB field fixed.  */
#define XRB_MASK (X_MASK | RB_MASK)

/* An X_MASK with the RT field fixed.  */
#define XRT_MASK (X_MASK | RT_MASK)

/* An XRT_MASK mask with the L bits clear.  */
#define XLRT_MASK (XRT_MASK & ~((unsigned long) 0x3 << 21))

/* An X_MASK with the RA and RB fields fixed.  */
#define XRARB_MASK (X_MASK | RA_MASK | RB_MASK)

/* An XRARB_MASK, but with the L bit clear.  */
#define XRLARB_MASK (XRARB_MASK & ~((unsigned long) 1 << 16))

/* An X_MASK with the RT and RA fields fixed.  */
#define XRTRA_MASK (X_MASK | RT_MASK | RA_MASK)

/* An XRTRA_MASK, but with L bit clear.  */
#define XRTLRA_MASK (XRTRA_MASK & ~((unsigned long) 1 << 21))

/* An X form instruction with the L bit specified.  */
#define XOPL(op, xop, l) (X ((op), (xop)) | ((((unsigned long)(l)) & 1) << 21))

/* An X form instruction with the L bits specified.  */
#define XOPL2(op, xop, l) (X ((op), (xop)) | ((((unsigned long)(l)) & 3) << 21))

/* An X form instruction with RT fields specified */
#define XRT(op, xop, rt) (X ((op), (xop)) \
        | ((((unsigned long)(rt)) & 0x1f) << 21))

/* An X form instruction with RT and RA fields specified */
#define XRTRA(op, xop, rt, ra) (X ((op), (xop)) \
        | ((((unsigned long)(rt)) & 0x1f) << 21) \
        | ((((unsigned long)(ra)) & 0x1f) << 16))

/* The mask for an X form comparison instruction.  */
#define XCMP_MASK (X_MASK | (((unsigned long)1) << 22))

/* The mask for an X form comparison instruction with the L field
   fixed.  */
#define XCMPL_MASK (XCMP_MASK | (((unsigned long)1) << 21))

/* An X form trap instruction with the TO field specified.  */
#define XTO(op, xop, to) (X ((op), (xop)) | ((((unsigned long)(to)) & 0x1f) << 21))
#define XTO_MASK (X_MASK | TO_MASK)

/* An X form tlb instruction with the SH field specified.  */
#define XTLB(op, xop, sh) (X ((op), (xop)) | ((((unsigned long)(sh)) & 0x1f) << 11))
#define XTLB_MASK (X_MASK | SH_MASK)

/* An X form sync instruction.  */
#define XSYNC(op, xop, l) (X ((op), (xop)) | ((((unsigned long)(l)) & 3) << 21))

/* An X form sync instruction with everything filled in except the LS field.  */
#define XSYNC_MASK (0xff9fffff)

/* An X form sync instruction with everything filled in except the L and E fields.  */
#define XSYNCLE_MASK (0xff90ffff)

/* An X_MASK, but with the EH bit clear.  */
#define XEH_MASK (X_MASK & ~((unsigned long )1))

/* An X form AltiVec dss instruction.  */
#define XDSS(op, xop, a) (X ((op), (xop)) | ((((unsigned long)(a)) & 1) << 25))
#define XDSS_MASK XDSS(0x3f, 0x3ff, 1)

/* An XFL form instruction.  */
#define XFL(op, xop, rc) (OP (op) | ((((unsigned long)(xop)) & 0x3ff) << 1) | (((unsigned long)(rc)) & 1))
#define XFL_MASK XFL (0x3f, 0x3ff, 1)

/* An X form isel instruction.  */
#define XISEL(op, xop)	(OP (op) | ((((unsigned long)(xop)) & 0x1f) << 1))
#define XISEL_MASK	XISEL(0x3f, 0x1f)

/* An XL form instruction with the LK field set to 0.  */
#define XL(op, xop) (OP (op) | ((((unsigned long)(xop)) & 0x3ff) << 1))

/* An XL form instruction which uses the LK field.  */
#define XLLK(op, xop, lk) (XL ((op), (xop)) | ((lk) & 1))

/* The mask for an XL form instruction.  */
#define XL_MASK XLLK (0x3f, 0x3ff, 1)

/* An XL form instruction which explicitly sets the BO field.  */
#define XLO(op, bo, xop, lk) \
  (XLLK ((op), (xop), (lk)) | ((((unsigned long)(bo)) & 0x1f) << 21))
#define XLO_MASK (XL_MASK | BO_MASK)

/* An XL form instruction which explicitly sets the y bit of the BO
   field.  */
#define XLYLK(op, xop, y, lk) (XLLK ((op), (xop), (lk)) | ((((unsigned long)(y)) & 1) << 21))
#define XLYLK_MASK (XL_MASK | Y_MASK)

/* An XL form instruction which sets the BO field and the condition
   bits of the BI field.  */
#define XLOCB(op, bo, cb, xop, lk) \
  (XLO ((op), (bo), (xop), (lk)) | ((((unsigned long)(cb)) & 3) << 16))
#define XLOCB_MASK XLOCB (0x3f, 0x1f, 0x3, 0x3ff, 1)

/* An XL_MASK or XLYLK_MASK or XLOCB_MASK with the BB field fixed.  */
#define XLBB_MASK (XL_MASK | BB_MASK)
#define XLYBB_MASK (XLYLK_MASK | BB_MASK)
#define XLBOCBBB_MASK (XLOCB_MASK | BB_MASK)

/* A mask for branch instructions using the BH field.  */
#define XLBH_MASK (XL_MASK | (0x1c << 11))

/* An XL_MASK with the BO and BB fields fixed.  */
#define XLBOBB_MASK (XL_MASK | BO_MASK | BB_MASK)

/* An XL_MASK with the BO, BI and BB fields fixed.  */
#define XLBOBIBB_MASK (XL_MASK | BO_MASK | BI_MASK | BB_MASK)

/* An X form mbar instruction with MO field.  */
#define XMBAR(op, xop, mo) (X ((op), (xop)) | ((((unsigned long)(mo)) & 1) << 21))

/* An XO form instruction.  */
#define XO(op, xop, oe, rc) \
  (OP (op) | ((((unsigned long)(xop)) & 0x1ff) << 1) | ((((unsigned long)(oe)) & 1) << 10) | (((unsigned long)(rc)) & 1))
#define XO_MASK XO (0x3f, 0x1ff, 1, 1)

/* An XO_MASK with the RB field fixed.  */
#define XORB_MASK (XO_MASK | RB_MASK)

/* An XOPS form instruction for paired singles.  */
#define XOPS(op, xop, rc) \
  (OP (op) | ((((unsigned long)(xop)) & 0x3ff) << 1) | (((unsigned long)(rc)) & 1))
#define XOPS_MASK XOPS (0x3f, 0x3ff, 1)


/* An XS form instruction.  */
#define XS(op, xop, rc) (OP (op) | ((((unsigned long)(xop)) & 0x1ff) << 2) | (((unsigned long)(rc)) & 1))
#define XS_MASK XS (0x3f, 0x1ff, 1)

/* A mask for the FXM version of an XFX form instruction.  */
#define XFXFXM_MASK (X_MASK | (1 << 11) | (1 << 20))

/* An XFX form instruction with the FXM field filled in.  */
#define XFXM(op, xop, fxm, p4) \
  (X ((op), (xop)) | ((((unsigned long)(fxm)) & 0xff) << 12) \
   | ((unsigned long)(p4) << 20))

/* An XFX form instruction with the SPR field filled in.  */
#define XSPR(op, xop, spr) \
  (X ((op), (xop)) | ((((unsigned long)(spr)) & 0x1f) << 16) | ((((unsigned long)(spr)) & 0x3e0) << 6))
#define XSPR_MASK (X_MASK | SPR_MASK)

/* An XFX form instruction with the SPR field filled in except for the
   SPRBAT field.  */
#define XSPRBAT_MASK (XSPR_MASK &~ SPRBAT_MASK)

/* An XFX form instruction with the SPR field filled in except for the
   SPRG field.  */
#define XSPRG_MASK (XSPR_MASK & ~(0x1f << 16))

/* An X form instruction with everything filled in except the E field.  */
#define XE_MASK (0xffff7fff)

/* An X form user context instruction.  */
#define XUC(op, xop)  (OP (op) | (((unsigned long)(xop)) & 0x1f))
#define XUC_MASK      XUC(0x3f, 0x1f)

/* An XW form instruction.  */
#define XW(op, xop, rc) (OP (op) | ((((unsigned long)(xop)) & 0x3f) << 1) | ((rc) & 1))
/* The mask for a G form instruction. rc not supported at present.  */
#define XW_MASK XW (0x3f, 0x3f, 0)

/* An APU form instruction.  */
#define APU(op, xop, rc) (OP (op) | (((unsigned long)(xop)) & 0x3ff) << 1 | ((rc) & 1))

/* The mask for an APU form instruction.  */
#define APU_MASK APU (0x3f, 0x3ff, 1)
#define APU_RT_MASK (APU_MASK | RT_MASK)
#define APU_RA_MASK (APU_MASK | RA_MASK)

/* The BO encodings used in extended conditional branch mnemonics.  */
#define BODNZF	(0x0)
#define BODNZFP	(0x1)
#define BODZF	(0x2)
#define BODZFP	(0x3)
#define BODNZT	(0x8)
#define BODNZTP	(0x9)
#define BODZT	(0xa)
#define BODZTP	(0xb)

#define BOF	(0x4)
#define BOFP	(0x5)
#define BOFM4	(0x6)
#define BOFP4	(0x7)
#define BOT	(0xc)
#define BOTP	(0xd)
#define BOTM4	(0xe)
#define BOTP4	(0xf)

#define BODNZ	(0x10)
#define BODNZP	(0x11)
#define BODZ	(0x12)
#define BODZP	(0x13)
#define BODNZM4 (0x18)
#define BODNZP4 (0x19)
#define BODZM4	(0x1a)
#define BODZP4	(0x1b)

#define BOU	(0x14)

/* The BI condition bit encodings used in extended conditional branch
   mnemonics.  */
#define CBLT	(0)
#define CBGT	(1)
#define CBEQ	(2)
#define CBSO	(3)

/* The TO encodings used in extended trap mnemonics.  */
#define TOLGT	(0x1)
#define TOLLT	(0x2)
#define TOEQ	(0x4)
#define TOLGE	(0x5)
#define TOLNL	(0x5)
#define TOLLE	(0x6)
#define TOLNG	(0x6)
#define TOGT	(0x8)
#define TOGE	(0xc)
#define TONL	(0xc)
#define TOLT	(0x10)
#define TOLE	(0x14)
#define TONG	(0x14)
#define TONE	(0x18)
#define TOU	(0x1f)

/* Smaller names for the flags so each entry in the opcodes table will
   fit on a single line.  */
#define PPCNONE	0
#undef	PPC
#define PPC	PPC_OPCODE_PPC
#define PPCCOM	PPC_OPCODE_PPC | PPC_OPCODE_COMMON
#define POWER4	PPC_OPCODE_POWER4
#define POWER5	PPC_OPCODE_POWER5
#define POWER6	PPC_OPCODE_POWER6
#define POWER7	PPC_OPCODE_POWER7
#define CELL	PPC_OPCODE_CELL
#define PPC64	PPC_OPCODE_64 | PPC_OPCODE_64_BRIDGE
#define NON32	(PPC_OPCODE_64 | PPC_OPCODE_POWER4	\
		 | PPC_OPCODE_EFS | PPC_OPCODE_E500MC | PPC_OPCODE_TITAN)
#define PPC403	PPC_OPCODE_403
#define PPC405	PPC_OPCODE_405
#define PPC440	PPC_OPCODE_440
#define PPC464	PPC440
#define PPC476	PPC_OPCODE_476
#define PPC750	PPC
#define PPC7450 PPC
#define PPC860	PPC
#define PPCPS	PPC_OPCODE_PPCPS
#define PPCVEC	PPC_OPCODE_ALTIVEC
#define PPCVEC2	PPC_OPCODE_ALTIVEC2
#define COM	PPC_OPCODE_POWER | PPC_OPCODE_PPC | PPC_OPCODE_COMMON
#define BOOKE	PPC_OPCODE_BOOKE
#define NO371	PPC_OPCODE_BOOKE | PPC_OPCODE_EFS
#define PPCE300 PPC_OPCODE_E300
#define PPCSPE	PPC_OPCODE_SPE
#define PPCISEL PPC_OPCODE_ISEL
#define PPCEFS	PPC_OPCODE_EFS
#define PPCBRLK PPC_OPCODE_BRLOCK
#define PPCPMR	PPC_OPCODE_PMR
#define PPCTMR  PPC_OPCODE_TMR
#define PPCCHLK PPC_OPCODE_CACHELCK
#define PPCRFMCI	PPC_OPCODE_RFMCI
#define E500MC  PPC_OPCODE_E500MC
#define PPCA2	PPC_OPCODE_A2
#define TITAN   PPC_OPCODE_TITAN  
#define E500	PPC_OPCODE_E500
#define E6500	PPC_OPCODE_E6500

/* The opcode table.

   The format of the opcode table is:

   NAME		OPCODE		MASK	     FLAGS	ANTI		{OPERANDS}

   NAME is the name of the instruction.
   OPCODE is the instruction opcode.
   MASK is the opcode mask; this is used to tell the disassembler
     which bits in the actual opcode must match OPCODE.
   FLAGS are flags indicating which processors support the instruction.
   ANTI indicates which processors don't support the instruction.
   OPERANDS is the list of operands.

   The disassembler reads the table in order and prints the first
   instruction which matches, so this table is sorted to put more
   specific instructions before more general instructions.

   This table must be sorted by major opcode.  Please try to keep it
   vaguely sorted within major opcode too, except of course where
   constrained otherwise by disassembler operation.  */

static const struct powerpc_opcode powerpc_opcodes[] = {
{"attn",	X(0,256),	X_MASK,   POWER4|PPCA2,	PPC476,		{0}},
{"tdi",		OP(2),		OP_MASK,     PPC64,	PPCNONE,	{TO, RA, SI}},

{"twi",		OP(3),		OP_MASK,     PPCCOM,	PPCNONE,	{TO, RA, SI}},

{"vaddubm",	VX (4,	 0),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmaxub",	VX (4,	 2),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vrlb",	VX (4,	 4),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpequb",	VXR(4,	 6,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmuloub",	VX (4,	 8),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vaddfp",	VX (4,	10),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmrghb",	VX (4,	12),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkuhum",	VX (4,	14),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmhaddshs",	VXA(4,	32),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmhraddshs",	VXA(4,	33),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmladduhm",	VXA(4,	34),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmsumubm",	VXA(4,	36),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmsummbm",	VXA(4,	37),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmsumuhm",	VXA(4,	38),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmsumuhs",	VXA(4,	39),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmsumshm",	VXA(4,	40),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vmsumshs",	VXA(4,	41),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vsel",	VXA(4,	42),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vperm",	VXA(4,	43),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, VC}},
{"vsldoi",	VXA(4,	44),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB, SHB}},
{"vmaddfp",	VXA(4,	46),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VC, VB}},
{"vnmsubfp",	VXA(4,	47),	VXA_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VC, VB}},
{"vadduhm",	VX (4,	64),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmaxuh",	VX (4,	66),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vrlh",	VX (4,	68),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpequh",	VXR(4,	70,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmulouh",	VX (4,	72),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsubfp",	VX (4,	74),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmrghh",	VX (4,	76),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkuwum",	VX (4,	78),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vadduwm",	VX (4,	128),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmaxuw",	VX (4,	130),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vrlw",	VX (4,	132),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpequw",	VXR(4,	134,0), VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmrghw",	VX (4,	140),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkuhus",	VX (4,	142),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vabsdub",	VX (4, 192),	VX_MASK,     PPCVEC2,	PPCNONE,	{VD, VA, VB}},
{"vcmpeqfp",	VXR(4, 198,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkuwus",	VX (4, 206),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vabsduh",	VX (4, 256),	VX_MASK,     PPCVEC2,	PPCNONE,	{VD, VA, VB}},
{"vmaxsb",	VX (4, 258),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vslb",	VX (4, 260),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmulosb",	VX (4, 264),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vrefp",	VX (4, 266),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vmrglb",	VX (4, 268),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkshus",	VX (4, 270),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vabsduw",	VX (4, 320),	VX_MASK,     PPCVEC2,	PPCNONE,	{VD, VA, VB}},
{"vmaxsh",	VX (4, 322),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vslh",	VX (4, 324),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmulosh",	VX (4, 328),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vrsqrtefp",	VX (4, 330),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vmrglh",	VX (4, 332),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkswus",	VX (4, 334),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vaddcuw",	VX (4, 384),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmaxsw",	VX (4, 386),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vslw",	VX (4, 388),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vexptefp",	VX (4, 394),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vmrglw",	VX (4, 396),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vpkshss",	VX (4, 398),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsl",		VX (4, 452),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgefp",	VXR(4, 454,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vlogefp",	VX (4, 458),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vpkswss",	VX (4, 462),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evaddw",	VX (4, 512),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vaddubs",	VX (4, 512),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evaddiw",	VX (4, 514),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB, UIMM}},
{"vminub",	VX (4, 514),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evsubfw",	VX (4, 516),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evsubw",	VX (4, 516),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB, RA}},
{"vsrb",	VX (4, 516),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evsubifw",	VX (4, 518),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, UIMM, RB}},
{"evsubiw",	VX (4, 518),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB, UIMM}},
{"vcmpgtub",	VXR(4, 518,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evabs",	VX (4, 520),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vmuleub",	VX (4, 520),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evneg",	VX (4, 521),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evextsb",	VX (4, 522),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vrfin",	VX (4, 522),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"evextsh",	VX (4, 523),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evrndw",	VX (4, 524),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vspltb",	VX (4, 524),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"evcntlzw",	VX (4, 525),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evcntlsw",	VX (4, 526),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vupkhsb",	VX (4, 526),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"brinc",	VX (4, 527),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evand",	VX (4, 529),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evandc",	VX (4, 530),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evxor",	VX (4, 534),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmr",	VX (4, 535),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, BBA}},
{"evor",	VX (4, 535),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evnor",	VX (4, 536),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evnot",	VX (4, 536),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, BBA}},
{"eveqv",	VX (4, 537),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evorc",	VX (4, 539),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evnand",	VX (4, 542),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evsrwu",	VX (4, 544),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evsrws",	VX (4, 545),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evsrwiu",	VX (4, 546),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, EVUIMM}},
{"evsrwis",	VX (4, 547),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, EVUIMM}},
{"evslw",	VX (4, 548),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evslwi",	VX (4, 550),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, EVUIMM}},
{"evrlw",	VX (4, 552),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evsplati",	VX (4, 553),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, SIMM}},
{"evrlwi",	VX (4, 554),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, EVUIMM}},
{"evsplatfi",	VX (4, 555),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, SIMM}},
{"evmergehi",	VX (4, 556),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmergelo",	VX (4, 557),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmergehilo",	VX (4, 558),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmergelohi",	VX (4, 559),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evcmpgtu",	VX (4, 560),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evcmpgts",	VX (4, 561),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evcmpltu",	VX (4, 562),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evcmplts",	VX (4, 563),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evcmpeq",	VX (4, 564),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"vadduhs",	VX (4, 576),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vminuh",	VX (4, 578),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsrh",	VX (4, 580),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtuh",	VXR(4, 582,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmuleuh",	VX (4, 584),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vrfiz",	VX (4, 586),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vsplth",	VX (4, 588),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"vupkhsh",	VX (4, 590),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"evsel",	EVSEL(4,79),	EVSEL_MASK,  PPCSPE,	PPCNONE,	{RS, RA, RB, CRFS}},
{"evfsadd",	VX (4, 640),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vadduws",	VX (4, 640),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evfssub",	VX (4, 641),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vminuw",	VX (4, 642),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evfsabs",	VX (4, 644),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vsrw",	VX (4, 644),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evfsnabs",	VX (4, 645),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evfsneg",	VX (4, 646),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vcmpgtuw",	VXR(4, 646,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evfsmul",	VX (4, 648),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evfsdiv",	VX (4, 649),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vrfip",	VX (4, 650),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"evfscmpgt",	VX (4, 652),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"vspltw",	VX (4, 652),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"evfscmplt",	VX (4, 653),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evfscmpeq",	VX (4, 654),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"vupklsb",	VX (4, 654),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"evfscfui",	VX (4, 656),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfscfsi",	VX (4, 657),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfscfuf",	VX (4, 658),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfscfsf",	VX (4, 659),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfsctui",	VX (4, 660),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfsctsi",	VX (4, 661),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfsctuf",	VX (4, 662),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfsctsf",	VX (4, 663),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfsctuiz",	VX (4, 664),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfsctsiz",	VX (4, 666),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RB}},
{"evfststgt",	VX (4, 668),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evfststlt",	VX (4, 669),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"evfststeq",	VX (4, 670),	VX_MASK,     PPCSPE,	PPCNONE,	{CRFD, RA, RB}},
{"efsadd",	VX (4, 704),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efssub",	VX (4, 705),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efsabs",	VX (4, 708),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA}},
{"vsr",		VX (4, 708),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"efsnabs",	VX (4, 709),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA}},
{"efsneg",	VX (4, 710),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA}},
{"vcmpgtfp",	VXR(4, 710,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"efsmul",	VX (4, 712),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efsdiv",	VX (4, 713),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"vrfim",	VX (4, 714),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"efscmpgt",	VX (4, 716),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efscmplt",	VX (4, 717),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efscmpeq",	VX (4, 718),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"vupklsh",	VX (4, 718),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"efscfd",	VX (4, 719),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efscfui",	VX (4, 720),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efscfsi",	VX (4, 721),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efscfuf",	VX (4, 722),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efscfsf",	VX (4, 723),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efsctui",	VX (4, 724),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efsctsi",	VX (4, 725),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efsctuf",	VX (4, 726),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efsctsf",	VX (4, 727),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efsctuiz",	VX (4, 728),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efsctsiz",	VX (4, 730),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efststgt",	VX (4, 732),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efststlt",	VX (4, 733),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efststeq",	VX (4, 734),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efdadd",	VX (4, 736),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efdsub",	VX (4, 737),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efdcfuid",	VX (4, 738),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdcfsid",	VX (4, 739),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdabs",	VX (4, 740),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA}},
{"efdnabs",	VX (4, 741),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA}},
{"efdneg",	VX (4, 742),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA}},
{"efdmul",	VX (4, 744),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efddiv",	VX (4, 745),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RA, RB}},
{"efdctuidz",	VX (4, 746),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctsidz",	VX (4, 747),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdcmpgt",	VX (4, 748),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efdcmplt",	VX (4, 749),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efdcmpeq",	VX (4, 750),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efdcfs",	VX (4, 751),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdcfui",	VX (4, 752),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdcfsi",	VX (4, 753),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdcfuf",	VX (4, 754),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdcfsf",	VX (4, 755),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctui",	VX (4, 756),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctsi",	VX (4, 757),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctuf",	VX (4, 758),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctsf",	VX (4, 759),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctuiz",	VX (4, 760),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdctsiz",	VX (4, 762),	VX_MASK,     PPCEFS,	PPCNONE,	{RS, RB}},
{"efdtstgt",	VX (4, 764),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efdtstlt",	VX (4, 765),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"efdtsteq",	VX (4, 766),	VX_MASK,     PPCEFS,	PPCNONE,	{CRFD, RA, RB}},
{"evlddx",	VX (4, 768),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vaddsbs",	VX (4, 768),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evldd",	VX (4, 769),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_8, RA}},
{"evldwx",	VX (4, 770),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vminsb",	VX (4, 770),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evldw",	VX (4, 771),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_8, RA}},
{"evldhx",	VX (4, 772),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vsrab",	VX (4, 772),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evldh",	VX (4, 773),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_8, RA}},
{"vcmpgtsb",	VXR(4, 774,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evlhhesplatx",VX (4, 776),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vmulesb",	VX (4, 776),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evlhhesplat",	VX (4, 777),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_2, RA}},
{"vcfux",	VX (4, 778),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"evlhhousplatx",VX(4, 780),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vspltisb",	VX (4, 780),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, SIMM}},
{"evlhhousplat",VX (4, 781),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_2, RA}},
{"evlhhossplatx",VX(4, 782),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vpkpx",	VX (4, 782),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evlhhossplat",VX (4, 783),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_2, RA}},
{"evlwhex",	VX (4, 784),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evlwhe",	VX (4, 785),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evlwhoux",	VX (4, 788),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evlwhou",	VX (4, 789),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evlwhosx",	VX (4, 790),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evlwhos",	VX (4, 791),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evlwwsplatx",	VX (4, 792),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evlwwsplat",	VX (4, 793),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evlwhsplatx",	VX (4, 796),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evlwhsplat",	VX (4, 797),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evstddx",	VX (4, 800),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstdd",	VX (4, 801),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_8, RA}},
{"evstdwx",	VX (4, 802),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstdw",	VX (4, 803),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_8, RA}},
{"evstdhx",	VX (4, 804),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstdh",	VX (4, 805),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_8, RA}},
{"evstwhex",	VX (4, 816),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstwhe",	VX (4, 817),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evstwhox",	VX (4, 820),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstwho",	VX (4, 821),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evstwwex",	VX (4, 824),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstwwe",	VX (4, 825),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"evstwwox",	VX (4, 828),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evstwwo",	VX (4, 829),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, EVUIMM_4, RA}},
{"vaddshs",	VX (4, 832),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vminsh",	VX (4, 834),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsrah",	VX (4, 836),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtsh",	VXR(4, 838,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vmulesh",	VX (4, 840),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcfsx",	VX (4, 842),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"vspltish",	VX (4, 844),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, SIMM}},
{"vupkhpx",	VX (4, 846),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vaddsws",	VX (4, 896),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vminsw",	VX (4, 898),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsraw",	VX (4, 900),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtsw",	VXR(4, 902,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vctuxs",	VX (4, 906),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"vspltisw",	VX (4, 908),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, SIMM}},
{"vcmpbfp",	VXR(4, 966,0),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vctsxs",	VX (4, 970),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB, UIMM}},
{"vupklpx",	VX (4, 974),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VB}},
{"vsububm",	VX (4,1024),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vavgub",	VX (4,1026),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhessf",	VX (4,1027),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vand",	VX (4,1028),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpequb.",	VXR(4,	 6,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhossf",	VX (4,1031),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmheumi",	VX (4,1032),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmi",	VX (4,1033),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vmaxfp",	VX (4,1034),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhesmf",	VX (4,1035),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhoumi",	VX (4,1036),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vslo",	VX (4,1036),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhosmi",	VX (4,1037),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmf",	VX (4,1039),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhessfa",	VX (4,1059),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhossfa",	VX (4,1063),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmheumia",	VX (4,1064),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmia",	VX (4,1065),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmfa",	VX (4,1067),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhoumia",	VX (4,1068),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmia",	VX (4,1069),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmfa",	VX (4,1071),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vsubuhm",	VX (4,1088),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vavguh",	VX (4,1090),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vandc",	VX (4,1092),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpequh.",	VXR(4,	70,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmwhssf",	VX (4,1095),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlumi",	VX (4,1096),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vminfp",	VX (4,1098),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmwhumi",	VX (4,1100),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vsro",	VX (4,1100),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmwhsmi",	VX (4,1101),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwhsmf",	VX (4,1103),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwssf",	VX (4,1107),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwumi",	VX (4,1112),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmi",	VX (4,1113),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmf",	VX (4,1115),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwhssfa",	VX (4,1127),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlumia",	VX (4,1128),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwhumia",	VX (4,1132),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwhsmia",	VX (4,1133),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwhsmfa",	VX (4,1135),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwssfa",	VX (4,1139),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwumia",	VX (4,1144),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmia",	VX (4,1145),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmfa",	VX (4,1147),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vsubuwm",	VX (4,1152),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vavguw",	VX (4,1154),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vor",		VX (4,1156),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpequw.",	VXR(4, 134,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evaddusiaaw",	VX (4,1216),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evaddssiaaw",	VX (4,1217),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evsubfusiaaw",VX (4,1218),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evsubfssiaaw",VX (4,1219),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evmra",	VX (4,1220),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"vxor",	VX (4,1220),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evdivws",	VX (4,1222),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vcmpeqfp.",	VXR(4, 198,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evdivwu",	VX (4,1223),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evaddumiaaw",	VX (4,1224),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evaddsmiaaw",	VX (4,1225),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evsubfumiaaw",VX (4,1226),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evsubfsmiaaw",VX (4,1227),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA}},
{"evmheusiaaw",	VX (4,1280),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhessiaaw",	VX (4,1281),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vavgsb",	VX (4,1282),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhessfaaw",	VX (4,1283),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhousiaaw",	VX (4,1284),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vnor",	VX (4,1284),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhossiaaw",	VX (4,1285),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhossfaaw",	VX (4,1287),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmheumiaaw",	VX (4,1288),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmiaaw",	VX (4,1289),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmfaaw",	VX (4,1291),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhoumiaaw",	VX (4,1292),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmiaaw",	VX (4,1293),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmfaaw",	VX (4,1295),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhegumiaa",	VX (4,1320),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhegsmiaa",	VX (4,1321),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhegsmfaa",	VX (4,1323),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhogumiaa",	VX (4,1324),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhogsmiaa",	VX (4,1325),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhogsmfaa",	VX (4,1327),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlusiaaw",	VX (4,1344),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlssiaaw",	VX (4,1345),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vavgsh",	VX (4,1346),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmwlumiaaw",	VX (4,1352),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlsmiaaw",	VX (4,1353),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwssfaa",	VX (4,1363),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwumiaa",	VX (4,1368),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmiaa",	VX (4,1369),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmfaa",	VX (4,1371),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmheusianw",	VX (4,1408),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vsubcuw",	VX (4,1408),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhessianw",	VX (4,1409),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vavgsw",	VX (4,1410),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmhessfanw",	VX (4,1411),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhousianw",	VX (4,1412),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhossianw",	VX (4,1413),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhossfanw",	VX (4,1415),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmheumianw",	VX (4,1416),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmianw",	VX (4,1417),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhesmfanw",	VX (4,1419),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhoumianw",	VX (4,1420),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmianw",	VX (4,1421),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhosmfanw",	VX (4,1423),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhegumian",	VX (4,1448),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhegsmian",	VX (4,1449),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhegsmfan",	VX (4,1451),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhogumian",	VX (4,1452),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhogsmian",	VX (4,1453),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmhogsmfan",	VX (4,1455),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlusianw",	VX (4,1472),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlssianw",	VX (4,1473),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vcmpgefp.",	VXR(4, 454,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"evmwlumianw",	VX (4,1480),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwlsmianw",	VX (4,1481),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwssfan",	VX (4,1491),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwumian",	VX (4,1496),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmian",	VX (4,1497),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"evmwsmfan",	VX (4,1499),	VX_MASK,     PPCSPE,	PPCNONE,	{RS, RA, RB}},
{"vsububs",	VX (4,1536),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"mfvscr",	VX (4,1540),	VX_MASK,     PPCVEC,	PPCNONE,	{VD}},
{"vcmpgtub.",	VXR(4, 518,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsum4ubs",	VX (4,1544),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsubuhs",	VX (4,1600),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"mtvscr",	VX (4,1604),	VX_MASK,     PPCVEC,	PPCNONE,	{VB}},
{"vcmpgtuh.",	VXR(4, 582,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsum4shs",	VX (4,1608),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsubuws",	VX (4,1664),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtuw.",	VXR(4, 646,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsum2sws",	VX (4,1672),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtfp.",	VXR(4, 710,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsubsbs",	VX (4,1792),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtsb.",	VXR(4, 774,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsum4sbs",	VX (4,1800),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsubshs",	VX (4,1856),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtsh.",	VXR(4, 838,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsubsws",	VX (4,1920),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpgtsw.",	VXR(4, 902,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vsumsws",	VX (4,1928),	VX_MASK,     PPCVEC,	PPCNONE,	{VD, VA, VB}},
{"vcmpbfp.",	VXR(4, 966,1),	VXR_MASK,    PPCVEC,	PPCNONE,	{VD, VA, VB}},

{"mulli",	OP(7),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA, SI}},

{"subfic",	OP(8),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA, SI}},

{"cmpli",	OP(10),		OP_MASK,     PPC,	PPCNONE,	{BF, L, RA, UI}},

{"cmpi",	OP(11),		OP_MASK,     PPC,	PPCNONE,	{BF, L, RA, SI}},

{"addic",	OP(12),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA, SI}},
{"subic",	OP(12),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA, NSI}},

{"addic.",	OP(13),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA, SI}},
{"subic.",	OP(13),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA, NSI}},

{"addi",	OP(14),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA0, SI}},
{"subi",	OP(14),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA0, NSI}},
{"la",		OP(14),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, D, RA0}},

{"addis",	OP(15),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA0, SISIGNOPT}},
{"subis",	OP(15),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, RA0, NSI}},

{"bc",		B(16,0,0),	B_MASK,      COM,	PPCNONE,	{BO, BI, BD}},
{"bcl",		B(16,0,1),	B_MASK,      COM,	PPCNONE,	{BO, BI, BD}},
{"bca",		B(16,1,0),	B_MASK,      COM,	PPCNONE,	{BO, BI, BDA}},
{"bcla",	B(16,1,1),	B_MASK,      COM,	PPCNONE,	{BO, BI, BDA}},

{"sc",		SC(17,1,0),	SC_MASK,     PPC,	PPCNONE,	{LEV}},

{"b",		B(18,0,0),	B_MASK,      COM,	PPCNONE,	{LI}},
{"bl",		B(18,0,1),	B_MASK,      COM,	PPCNONE,	{LI}},
{"ba",		B(18,1,0),	B_MASK,      COM,	PPCNONE,	{LIA}},
{"bla",		B(18,1,1),	B_MASK,      COM,	PPCNONE,	{LIA}},

{"mcrf",      XL(19,0), XLBB_MASK|(3<<21)|(3<<16), COM,	PPCNONE,	{BF, BFA}},

{"bclr",     XLLK(19,16,0),		XLBH_MASK,     PPCCOM,	 PPCNONE,	{BO, BI, BH}},
{"bclrl",    XLLK(19,16,1),		XLBH_MASK,     PPCCOM,	 PPCNONE,	{BO, BI, BH}},

{"rfid",	XL(19,18),	0xffffffff,  PPC64,	PPCNONE,	{0}},

{"crnor",	XL(19,33),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},
{"rfmci",	X(19,38),   0xffffffff, PPCRFMCI|PPCA2|PPC476, PPCNONE,	{0}},

{"rfdi",	XL(19,39),	0xffffffff,  E500MC,	PPCNONE,	{0}},
{"rfi",		XL(19,50),	0xffffffff,  COM,	PPCNONE,	{0}},
{"rfci",	XL(19,51), 0xffffffff, PPC403|BOOKE|PPCE300|PPCA2|PPC476, PPCNONE, {0}},

{"rfgi",	XL(19,102),   0xffffffff, E500MC|PPCA2,	PPCNONE,	{0}},

{"crandc",	XL(19,129),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"isync",	XL(19,150),	0xffffffff,  PPCCOM,	PPCNONE,	{0}},

{"crxor",	XL(19,193),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"dnh",		X(19,198),	X_MASK,      E500MC,	PPCNONE,	{DUI, DUIS}},

{"crnand",	XL(19,225),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"crand",	XL(19,257),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"hrfid",	XL(19,274),	0xffffffff, POWER5|CELL, PPC476,	{0}},

{"creqv",	XL(19,289),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"doze",	XL(19,402),	0xffffffff,  POWER6,	PPCNONE,	{0}},

{"crorc",	XL(19,417),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"nap",		XL(19,434),	0xffffffff,  POWER6,	PPCNONE,	{0}},

{"cror",	XL(19,449),	XL_MASK,     COM,	PPCNONE,	{BT, BA, BB}},

{"sleep",	XL(19,466),	0xffffffff,  POWER6,	PPCNONE,	{0}},
{"rvwinkle",	XL(19,498),	0xffffffff,  POWER6,	PPCNONE,	{0}},

{"bcctr",   XLLK(19,528,0),		XLBH_MASK,     PPCCOM,	 PPCNONE,	{BO, BI, BH}},
{"bcctrl",  XLLK(19,528,1),		XLBH_MASK,     PPCCOM,	 PPCNONE,	{BO, BI, BH}},

{"rlwimi",	M(20,0),	M_MASK,      PPCCOM,	PPCNONE,	{RA, RS, SH, MBE, ME}},

{"rlwimi.",	M(20,1),	M_MASK,      PPCCOM,	PPCNONE,	{RA, RS, SH, MBE, ME}},

{"rotlwi",	MME(21,31,0),	MMBME_MASK,  PPCCOM,	PPCNONE,	{RA, RS, SH}},
{"clrlwi",	MME(21,31,0),	MSHME_MASK,  PPCCOM,	PPCNONE,	{RA, RS, MB}},
{"rlwinm",	M(21,0),	M_MASK,      PPCCOM,	PPCNONE,	{RA, RS, SH, MBE, ME}},
{"rotlwi.",	MME(21,31,1),	MMBME_MASK,  PPCCOM,	PPCNONE,	{RA, RS, SH}},
{"clrlwi.",	MME(21,31,1),	MSHME_MASK,  PPCCOM,	PPCNONE,	{RA, RS, MB}},
{"rlwinm.",	M(21,1),	M_MASK,      PPCCOM,	PPCNONE,	{RA, RS, SH, MBE, ME}},

{"rotlw",	MME(23,31,0),	MMBME_MASK,  PPCCOM,	PPCNONE,	{RA, RS, RB}},
{"rlwnm",	M(23,0),	M_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB, MBE, ME}},
{"rotlw.",	MME(23,31,1),	MMBME_MASK,  PPCCOM,	PPCNONE,	{RA, RS, RB}},
{"rlwnm.",	M(23,1),	M_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB, MBE, ME}},

{"ori",		OP(24),		OP_MASK,     PPCCOM,	PPCNONE,	{RA, RS, UI}},

{"oris",	OP(25),		OP_MASK,     PPCCOM,	PPCNONE,	{RA, RS, UI}},

{"xori",	OP(26),		OP_MASK,     PPCCOM,	PPCNONE,	{RA, RS, UI}},

{"xoris",	OP(27),		OP_MASK,     PPCCOM,	PPCNONE,	{RA, RS, UI}},

{"andi.",	OP(28),		OP_MASK,     PPCCOM,	PPCNONE,	{RA, RS, UI}},

{"andis.",	OP(29),		OP_MASK,     PPCCOM,	PPCNONE,	{RA, RS, UI}},

{"rotldi",	MD(30,0,0),	MDMB_MASK,   PPC64,	PPCNONE,	{RA, RS, SH6}},
{"clrldi",	MD(30,0,0),	MDSH_MASK,   PPC64,	PPCNONE,	{RA, RS, MB6}},
{"rldicl",	MD(30,0,0),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, MB6}},
{"rotldi.",	MD(30,0,1),	MDMB_MASK,   PPC64,	PPCNONE,	{RA, RS, SH6}},
{"clrldi.",	MD(30,0,1),	MDSH_MASK,   PPC64,	PPCNONE,	{RA, RS, MB6}},
{"rldicl.",	MD(30,0,1),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, MB6}},

{"rldicr",	MD(30,1,0),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, ME6}},
{"rldicr.",	MD(30,1,1),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, ME6}},

{"rldic",	MD(30,2,0),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, MB6}},
{"rldic.",	MD(30,2,1),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, MB6}},

{"rldimi",	MD(30,3,0),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, MB6}},
{"rldimi.",	MD(30,3,1),	MD_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6, MB6}},

{"rotld",	MDS(30,8,0),	MDSMB_MASK,  PPC64,	PPCNONE,	{RA, RS, RB}},
{"rldcl",	MDS(30,8,0),	MDS_MASK,    PPC64,	PPCNONE,	{RA, RS, RB, MB6}},
{"rotld.",	MDS(30,8,1),	MDSMB_MASK,  PPC64,	PPCNONE,	{RA, RS, RB}},
{"rldcl.",	MDS(30,8,1),	MDS_MASK,    PPC64,	PPCNONE,	{RA, RS, RB, MB6}},

{"rldcr",	MDS(30,9,0),	MDS_MASK,    PPC64,	PPCNONE,	{RA, RS, RB, ME6}},
{"rldcr.",	MDS(30,9,1),	MDS_MASK,    PPC64,	PPCNONE,	{RA, RS, RB, ME6}},

{"cmp",		X(31,0),	XCMP_MASK,   PPC,	PPCNONE,	{BF, L, RA, RB}},

{"trap",	XTO(31,4,TOU),	 0xffffffff, PPCCOM,	PPCNONE,	{0}},
{"tw",		X(31,4),	 X_MASK,     PPCCOM,	PPCNONE,	{TO, RA, RB}},

{"lvsl",	X(31,6),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},
{"lvebx",	X(31,7),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},

{"subfc",	XO(31,8,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"subc",	XO(31,8,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RB, RA}},
{"subfc.",	XO(31,8,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"subc.",	XO(31,8,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RB, RA}},

{"mulhdu",	XO(31,9,0,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"mulhdu.",	XO(31,9,0,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"addc",	XO(31,10,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"addc.",	XO(31,10,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"mulhwu",	XO(31,11,0,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"mulhwu.",	XO(31,11,0,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},

{"isellt",	X(31,15),	X_MASK,      PPCISEL,	PPCNONE,	{RT, RA, RB}},

{"tlbilxlpid",	XTO(31,18,0),	XTO_MASK, E500MC|PPCA2,	PPCNONE,	{0}},
{"tlbilxpid",	XTO(31,18,1),	XTO_MASK, E500MC|PPCA2,	PPCNONE,	{0}},
{"tlbilxva",	XTO(31,18,3),	XTO_MASK, E500MC|PPCA2,	PPCNONE,	{RA0, RB}},
{"tlbilx",	X(31,18),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{T, RA0, RB}},

{"mfcr",	XFXM(31,19,0,0), XFXFXM_MASK, POWER4,	PPCNONE,	{RT, FXM4}},
{"mfcr",	XFXM(31,19,0,0), XRARB_MASK, COM,	POWER4,		{RT}},
{"mfocrf",	XFXM(31,19,0,1), XFXFXM_MASK, COM,	PPCNONE,	{RT, FXM}},

{"lwarx",	X(31,20),	XEH_MASK,    PPC,	PPCNONE,	{RT, RA0, RB, EH}},

{"ldx",		X(31,21),	X_MASK,      PPC64,	PPCNONE,	{RT, RA0, RB}},

{"icbt",	X(31,22),	X_MASK, BOOKE|PPCE300|PPCA2|PPC476, PPCNONE, {CT, RA, RB}},

{"lwzx",	X(31,23),	X_MASK,      PPCCOM,	PPCNONE,	{RT, RA0, RB}},

{"slw",		XRC(31,24,0),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB}},
{"slw.",	XRC(31,24,1),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB}},

{"cntlzw",	XRC(31,26,0),	XRB_MASK,    PPCCOM,	PPCNONE,	{RA, RS}},
{"cntlzw.",	XRC(31,26,1),	XRB_MASK,    PPCCOM,	PPCNONE,	{RA, RS}},

{"sld",		XRC(31,27,0),	X_MASK,      PPC64,	PPCNONE,	{RA, RS, RB}},
{"sld.",	XRC(31,27,1),	X_MASK,      PPC64,	PPCNONE,	{RA, RS, RB}},

{"and",		XRC(31,28,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"and.",	XRC(31,28,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"ldepx",	X(31,29),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"lwepx",	X(31,31),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"cmpl",	X(31,32),	XCMP_MASK,   PPC,	PPCNONE,	{BF, L, RA, RB}},

{"lvsr",	X(31,38),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},
{"lvehx",	X(31,39),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},

{"iselgt",	X(31,47),	X_MASK,      PPCISEL,	PPCNONE,	{RT, RA, RB}},

{"lvewx",	X(31,71),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},

{"addg6s",	XO(31,74,0,0),	XO_MASK,     POWER6,	PPCNONE,	{RT, RA, RB}},

{"iseleq",	X(31,79),	X_MASK,      PPCISEL,	PPCNONE,	{RT, RA, RB}},

{"isel",	XISEL(31,15), XISEL_MASK, PPCISEL|TITAN, PPCNONE,	{RT, RA, RB, CRB}},

{"subf",	XO(31,40,0,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"sub",		XO(31,40,0,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RB, RA}},
{"subf.",	XO(31,40,0,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"sub.",	XO(31,40,0,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RB, RA}},

{"eratilx",	X(31,51),	X_MASK,	     PPCA2,	PPCNONE,	{ERAT_T, RA, RB}},

{"lbarx",	X(31,52),	XEH_MASK,    POWER7,	PPCNONE,	{RT, RA0, RB, EH}},

{"ldux",	X(31,53),	X_MASK,      PPC64,	PPCNONE,	{RT, RAL, RB}},

{"dcbst",	X(31,54),	XRT_MASK,    PPC,	PPCNONE,	{RA, RB}},

{"lwzux",	X(31,55),	X_MASK,      PPCCOM,	PPCNONE,	{RT, RAL, RB}},

{"cntlzd",	XRC(31,58,0),	XRB_MASK,    PPC64,	PPCNONE,	{RA, RS}},
{"cntlzd.",	XRC(31,58,1),	XRB_MASK,    PPC64,	PPCNONE,	{RA, RS}},

{"andc",	XRC(31,60,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"andc.",	XRC(31,60,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"waitrsv",	X(31,62)|(1<<21), 0xffffffff, POWER7|E500MC|PPCA2, PPCNONE, {0}},
{"waitimpl",	X(31,62)|(2<<21), 0xffffffff, POWER7|E500MC|PPCA2, PPCNONE, {0}},
{"wait",	X(31,62),	  XWC_MASK,   POWER7|E500MC|PPCA2, PPCNONE, {WC}},

{"dcbstep",	XRT(31,63,0),	XRT_MASK, E500MC|PPCA2,	PPCNONE,	{RA, RB}},

{"td",		X(31,68),	X_MASK,      PPC64,	PPCNONE,	{TO, RA, RB}},

{"mulhd",	XO(31,73,0,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"mulhd.",	XO(31,73,0,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"mulhw",	XO(31,75,0,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"mulhw.",	XO(31,75,0,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},

{"mtsrd",	X(31,82),  XRB_MASK|(1<<20), PPC64,	PPCNONE,	{SR, RS}},

{"mfmsr",	X(31,83),	XRARB_MASK,  COM,	PPCNONE,	{RT}},

{"ldarx",	X(31,84),	XEH_MASK,    PPC64,	PPCNONE,	{RT, RA0, RB, EH}},

{"dcbfl",	XOPL(31,86,1),	XRT_MASK,    POWER5,	PPC476,		{RA, RB}},
{"dcbf",	X(31,86),	XLRT_MASK,   PPC,	PPCNONE,	{RA, RB, L}},

{"lbzx",	X(31,87),	X_MASK,      COM,	PPCNONE,	{RT, RA0, RB}},

{"lbepx",	X(31,95),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"dni",		XRC(31,97,1),	XRB_MASK,    E6500,	PPCNONE,	{DUI, DCTL}},

{"lvx",		X(31,103),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},

{"neg",		XO(31,104,0,0),	XORB_MASK,   COM,	PPCNONE,	{RT, RA}},
{"neg.",	XO(31,104,0,1),	XORB_MASK,   COM,	PPCNONE,	{RT, RA}},

{"mvidsplt",	X(31,110),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA, RB}},

{"mtsrdin",	X(31,114),	XRA_MASK,    PPC64,	PPCNONE,	{RS, RB}},

{"lharx",	X(31,116),	XEH_MASK,    POWER7,	PPCNONE,	{RT, RA0, RB, EH}},

{"lbzux",	X(31,119),	X_MASK,      COM,	PPCNONE,	{RT, RAL, RB}},

{"popcntb",	X(31,122),	XRB_MASK,    POWER5,	PPCNONE,	{RA, RS}},

{"nor",		XRC(31,124,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"nor.",	XRC(31,124,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"dcbfep",	XRT(31,127,0),	XRT_MASK, E500MC|PPCA2,	PPCNONE,	{RA, RB}},

{"wrtee",	X(31,131), XRARB_MASK, PPC403|BOOKE|PPCA2|PPC476, PPCNONE, {RS}},

{"dcbtstls",	X(31,134), X_MASK, PPCCHLK|PPC476|TITAN, PPCNONE,	{CT, RA, RB}},

{"stvebx",	X(31,135),	X_MASK,      PPCVEC,	PPCNONE,	{VS, RA, RB}},

{"subfe",	XO(31,136,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"subfe.",	XO(31,136,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"adde",	XO(31,138,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"adde.",	XO(31,138,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"mviwsplt",	X(31,142),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA, RB}},

{"dcbtstlse",	X(31,142),	X_MASK,      PPCCHLK,	E500MC,		{CT, RA, RB}},

{"mtcrf",	XFXM(31,144,0,0), XFXFXM_MASK, COM,	PPCNONE,	{FXM, RS}},
{"mtocrf",	XFXM(31,144,0,1), XFXFXM_MASK, COM,	PPCNONE,	{FXM, RS}},

{"mtmsr",	X(31,146),	XRLARB_MASK, COM,	PPCNONE,	{RS, A_L}},

{"eratsx",	XRC(31,147,0),	X_MASK,	     PPCA2,	PPCNONE,	{RT, RA0, RB}},
{"eratsx.",	XRC(31,147,1),	X_MASK,	     PPCA2,	PPCNONE,	{RT, RA0, RB}},

{"stdx",	X(31,149),	X_MASK,      PPC64,	PPCNONE,	{RS, RA0, RB}},

{"stwcx.",	XRC(31,150,1),	X_MASK,      PPC,	PPCNONE,	{RS, RA0, RB}},

{"stwx",	X(31,151),	X_MASK,      PPCCOM,	PPCNONE,	{RS, RA0, RB}},

{"prtyw",	X(31,154),	XRB_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{RA, RS}},

{"stdepx",	X(31,157),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RS, RA, RB}},

{"stwepx",	X(31,159),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RS, RA, RB}},

{"wrteei",	X(31,163), XE_MASK, PPC403|BOOKE|PPCA2|PPC476, PPCNONE,	{E}},

{"dcbtls",	X(31,166), X_MASK, PPCCHLK|PPC476|TITAN, PPCNONE,	{CT, RA, RB}},

{"stvehx",	X(31,167),	X_MASK,      PPCVEC,	PPCNONE,	{VS, RA, RB}},

{"dcbtlse",	X(31,174),	X_MASK,      PPCCHLK,	E500MC,		{CT, RA, RB}},

{"mtmsrd",	X(31,178),	XRLARB_MASK, PPC64,	PPCNONE,	{RS, A_L}},

{"eratre",	X(31,179),	X_MASK,	     PPCA2,	PPCNONE,	{RT, RA, WS}},

{"stdux",	X(31,181),	X_MASK,      PPC64,	PPCNONE,	{RS, RAS, RB}},

{"wchkall",	X(31,182),	X_MASK,      PPCA2,	PPCNONE,	{OBF}},

{"stwux",	X(31,183),	X_MASK,      PPCCOM,	PPCNONE,	{RS, RAS, RB}},

{"prtyd",	X(31,186),	XRB_MASK, POWER6|PPCA2,	PPCNONE,	{RA, RS}},

{"icblq.",	XRC(31,198,1),	X_MASK,      E6500,	PPCNONE,	{CT, RA0, RB}},

{"stvewx",	X(31,199),	X_MASK,      PPCVEC,	PPCNONE,	{VS, RA, RB}},

{"subfze",	XO(31,200,0,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"subfze.",	XO(31,200,0,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"addze",	XO(31,202,0,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"addze.",	XO(31,202,0,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"msgsnd",	XRTRA(31,206,0,0), XRTRA_MASK, E500MC|PPCA2, PPCNONE,	{RB}},

{"mtsr",	X(31,210), XRB_MASK|(1<<20), COM,	NON32,  	{SR, RS}},

{"eratwe",	X(31,211),	X_MASK,	     PPCA2,	PPCNONE,	{RS, RA, WS}},

{"ldawx.",	XRC(31,212,1),	X_MASK,	     PPCA2,	PPCNONE,	{RT, RA0, RB}},

{"stdcx.",	XRC(31,214,1),	X_MASK,      PPC64,	PPCNONE,	{RS, RA0, RB}},

{"stbx",	X(31,215),	X_MASK,      COM,	PPCNONE,	{RS, RA0, RB}},

{"stbepx",	X(31,223),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RS, RA, RB}},

{"icblc",	X(31,230), X_MASK, PPCCHLK|PPC476|TITAN, PPCNONE,	{CT, RA, RB}},

{"stvx",	X(31,231),	X_MASK,      PPCVEC,	PPCNONE,	{VS, RA, RB}},

{"subfme",	XO(31,232,0,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"subfme.",	XO(31,232,0,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"mulld",	XO(31,233,0,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"mulld.",	XO(31,233,0,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"addme",	XO(31,234,0,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"addme.",	XO(31,234,0,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"mullw",	XO(31,235,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"mullw.",	XO(31,235,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"icblce",	X(31,238),	X_MASK,      PPCCHLK,	E500MC|PPCA2,	{CT, RA, RB}},
{"msgclr",	XRTRA(31,238,0,0),XRTRA_MASK,E500MC|PPCA2, PPCNONE,	{RB}},
{"mtsrin",	X(31,242),	XRA_MASK,    PPC,	NON32,  	{RS, RB}},

{"dcbtstt",	XRT(31,246,0x10), XRT_MASK,  POWER7,	PPCNONE,	{RA, RB}},
{"dcbtst",	X(31,246),	X_MASK,      POWER4,	PPCNONE,	{RA, RB, CT}},
{"dcbtst",	X(31,246),	X_MASK,      PPC,	POWER4,		{CT, RA, RB}},

{"stbux",	X(31,247),	X_MASK,      COM,	PPCNONE,	{RS, RAS, RB}},

{"bpermd",	X(31,252),	X_MASK,   POWER7|PPCA2,	PPCNONE,	{RA, RS, RB}},

{"dcbtstep",	XRT(31,255,0),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"mfdcrx",	X(31,259),	X_MASK, BOOKE|PPCA2|PPC476, TITAN,	{RS, RA}},
{"mfdcrx.",	XRC(31,259,1),	X_MASK,      PPCA2,	PPCNONE,	{RS, RA}},

{"lvexbx",	X(31,261),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"lvepxl",	X(31,263),	X_MASK,      PPCVEC2,   PPCNONE,	{VD, RA0, RB}},

{"add",		XO(31,266,0,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"add.",	XO(31,266,0,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"ehpriv",	X(31,270),	0xffffffff, E500MC|PPCA2, PPCNONE,	{0}},

{"tlbiel",	X(31,274),	XRTLRA_MASK, POWER4,	PPC476,		{RB, L}},

{"mfapidi",	X(31,275),	X_MASK,      BOOKE,	TITAN,  	{RT, RA}},

{"dcbtt",	XRT(31,278,0x10), XRT_MASK,  POWER7,	PPCNONE,	{RA, RB}},
{"dcbt",	X(31,278),	X_MASK,      POWER4,	PPCNONE,	{RA, RB, CT}},
{"dcbt",	X(31,278),	X_MASK,      PPC,	POWER4,		{CT, RA, RB}},

{"lhzx",	X(31,279),	X_MASK,      COM,	PPCNONE,	{RT, RA0, RB}},

{"cdtbcd",	X(31,282),	XRB_MASK,    POWER6,	PPCNONE,	{RA, RS}},

{"eqv",		XRC(31,284,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"eqv.",	XRC(31,284,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"lhepx",	X(31,287),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"lvexhx",	X(31,293),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},
{"lvepx",	X(31,295),	X_MASK,      PPCVEC2,   PPCNONE,	{VD, RA0, RB}},

{"tlbie",	X(31,306),	XRTLRA_MASK, PPC,	TITAN,  	{RB, L}},

{"eciwx",	X(31,310),	X_MASK,      PPC,	TITAN,  	{RT, RA, RB}},

{"lhzux",	X(31,311),	X_MASK,      COM,	PPCNONE,	{RT, RAL, RB}},

{"cbcdtd",	X(31,314),	XRB_MASK,    POWER6,	PPCNONE,	{RA, RS}},

{"xor",		XRC(31,316,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"xor.",	XRC(31,316,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"dcbtep",	XRT(31,319,0),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"mfdcr",	X(31,323), X_MASK, PPC403|BOOKE|PPCA2|PPC476, TITAN,	{RT, SPR}},
{"mfdcr.",	XRC(31,323,1),	X_MASK,      PPCA2,	PPCNONE,	{RT, SPR}},

{"lvexwx",	X(31,325),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"dcread",	X(31,326),	X_MASK,  PPC476|TITAN,	PPCNONE,	{RT, RA, RB}},

{"mfpmr",	X(31,334),	X_MASK, PPCPMR|PPCE300,	PPCNONE,	{RT, PMR}},
{"mftmr",	X(31,366),	X_MASK,	PPCTMR|E6500,	PPCNONE,	{RT, TMR}},

{"mfspr",	X(31,339),	X_MASK,      COM,	PPCNONE,	{RT, SPR}},

{"lwax",	X(31,341),	X_MASK,      PPC64,	PPCNONE,	{RT, RA0, RB}},

{"dst",		XDSS(31,342,0),	XDSS_MASK,   PPCVEC,	PPCNONE,	{RA, RB, STRM}},

{"lhax",	X(31,343),	X_MASK,      COM,	PPCNONE,	{RT, RA0, RB}},

{"lvxl",	X(31,359),	X_MASK,      PPCVEC,	PPCNONE,	{VD, RA, RB}},

{"tlbia",	X(31,370),	0xffffffff,  PPC,	TITAN,  	{0}},

{"mftbl",	XSPR(31,371,268), XSPR_MASK, PPC,	NO371,		{RT}},
{"mftbu",	XSPR(31,371,269), XSPR_MASK, PPC,	NO371,		{RT}},
{"mftb",	X(31,371),	X_MASK,      PPC|PPCA2,	NO371|POWER7,	{RT, TBR}},

{"lwaux",	X(31,373),	X_MASK,      PPC64,	PPCNONE,	{RT, RAL, RB}},

{"dstst",	XDSS(31,374,0),	XDSS_MASK,   PPCVEC,	PPCNONE,	{RA, RB, STRM}},

{"lhaux",	X(31,375),	X_MASK,      COM,	PPCNONE,	{RT, RAL, RB}},

{"popcntw",	X(31,378),	XRB_MASK, POWER7|PPCA2,	PPCNONE,	{RA, RS}},

{"mtdcrx",	X(31,387),	X_MASK, BOOKE|PPCA2|PPC476, TITAN,	{RA, RS}},
{"mtdcrx.",	XRC(31,387,1),	X_MASK,      PPCA2,	PPCNONE,	{RA, RS}},

{"stvexbx",	X(31,389),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"dcblc",	X(31,390),	X_MASK, PPCCHLK|PPC476|TITAN, PPCNONE,	{CT, RA, RB}},

{"divdeu",	XO(31,393,0,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divdeu.",	XO(31,393,0,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divweu",	XO(31,395,0,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divweu.",	XO(31,395,0,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"dcblce",	X(31,398),	X_MASK,      PPCCHLK,	E500MC,		{CT, RA, RB}},

{"slbmte",	X(31,402),	XRA_MASK,    PPC64,	PPCNONE,	{RS, RB}},

{"icswx",	XRC(31,406,0),	X_MASK,   POWER7|PPCA2,	PPCNONE,	{RS, RA, RB}},
{"icswx.",	XRC(31,406,1),	X_MASK,   POWER7|PPCA2,	PPCNONE,	{RS, RA, RB}},

{"sthx",	X(31,407),	X_MASK,      COM,	PPCNONE,	{RS, RA0, RB}},

{"orc",		XRC(31,412,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"orc.",	XRC(31,412,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"sthepx",	X(31,415),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{RS, RA, RB}},

{"stvexhx",	X(31,421),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"dcblq.",	XRC(31,422,1),	X_MASK,      E6500,	PPCNONE,	{CT, RA0, RB}},

{"divde",	XO(31,425,0,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divde.",	XO(31,425,0,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divwe",	XO(31,427,0,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divwe.",	XO(31,427,0,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"slbie",	X(31,434),	XRTRA_MASK,  PPC64,	PPCNONE,	{RB}},

{"ecowx",	X(31,438),	X_MASK,      PPC,	TITAN,  	{RT, RA, RB}},

{"sthux",	X(31,439),	X_MASK,      COM,	PPCNONE,	{RS, RAS, RB}},

{"mdors",	0x7f9ce378,	0xffffffff,  E500MC,	PPCNONE,	{0}},

{"miso",	0x7f5ad378,	0xffffffff,  E6500,	PPCNONE,	{0}},

{"or",		XRC(31,444,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"or.",		XRC(31,444,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"mtdcr",	X(31,451), X_MASK, PPC403|BOOKE|PPCA2|PPC476, TITAN,	{SPR, RS}},
{"mtdcr.",	XRC(31,451,1), X_MASK,       PPCA2,	PPCNONE,	{SPR, RS}},

{"stvexwx",	X(31,453),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"dci",		X(31,454),	XRARB_MASK, PPCA2|PPC476, PPCNONE,	{CT}},

{"divdu",	XO(31,457,0,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"divdu.",	XO(31,457,0,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"divwu",	XO(31,459,0,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"divwu.",	XO(31,459,0,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},

{"mtpmr",	X(31,462),	X_MASK, PPCPMR|PPCE300,	PPCNONE,	{PMR, RS}},
{"mttmr",	X(31,494),	X_MASK,	PPCTMR|E6500,	PPCNONE,	{TMR, RS}},

{"mtspr",	X(31,467),	  X_MASK,    COM,	PPCNONE,	{SPR, RS}},

{"dcbi",	X(31,470),	XRT_MASK,    PPC,	PPCNONE,	{RA, RB}},

{"nand",	XRC(31,476,0),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},
{"nand.",	XRC(31,476,1),	X_MASK,      COM,	PPCNONE,	{RA, RS, RB}},

{"dsn", 	X(31,483),	XRT_MASK,    E500MC,	PPCNONE,	{RA, RB}},

{"icbtls",	X(31,486),	X_MASK, PPCCHLK|PPC476|TITAN, PPCNONE,	{CT, RA, RB}},

{"stvxl",	X(31,487),	X_MASK,      PPCVEC,	PPCNONE,	{VS, RA, RB}},

{"divd",	XO(31,489,0,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"divd.",	XO(31,489,0,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"divw",	XO(31,491,0,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"divw.",	XO(31,491,0,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},

{"icbtlse",	X(31,494),	X_MASK,      PPCCHLK,	E500MC,		{CT, RA, RB}},

{"slbia",	X(31,498),	0xffffffff,  PPC64,	PPCNONE,	{0}},

{"popcntd",	X(31,506),	XRB_MASK, POWER7|PPCA2,	PPCNONE,	{RA, RS}},

{"cmpb",	X(31,508),	X_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{RA, RS, RB}},

{"mcrxr",	X(31,512), XRARB_MASK|(3<<21), COM,	POWER7,		{BF}},

{"lbdx",	X(31,515),	X_MASK,      E500MC,	PPCNONE,	{RT, RA, RB}},

{"bblels",	X(31,518),	X_MASK,      PPCBRLK,	PPCNONE,	{0}},

{"lvlx",	X(31,519),	X_MASK,      CELL,	PPCNONE,	{VD, RA0, RB}},

{"subfco",	XO(31,8,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"subco",	XO(31,8,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RB, RA}},
{"subfco.",	XO(31,8,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"subco.",	XO(31,8,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RB, RA}},

{"addco",	XO(31,10,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"addco.",	XO(31,10,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"ldbrx",	X(31,532),	X_MASK, CELL|POWER7|PPCA2, PPCNONE,	{RT, RA0, RB}},

{"lswx",	X(31,533),	X_MASK,      PPCCOM,	E500|E500MC,	{RT, RAX, RBX}},

{"lwbrx",	X(31,534),	X_MASK,      PPCCOM,	PPCNONE,	{RT, RA0, RB}},

{"lfsx",	X(31,535),	X_MASK,      COM,	PPCEFS,		{FRT, RA0, RB}},

{"srw",		XRC(31,536,0),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB}},
{"srw.",	XRC(31,536,1),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB}},

{"srd",		XRC(31,539,0),	X_MASK,      PPC64,	PPCNONE,	{RA, RS, RB}},
{"srd.",	XRC(31,539,1),	X_MASK,      PPC64,	PPCNONE,	{RA, RS, RB}},

{"lhdx",	X(31,547),	X_MASK,      E500MC,	PPCNONE,	{RT, RA, RB}},

{"lvtrx",	X(31,549),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"bbelr",	X(31,550),	X_MASK,      PPCBRLK,	PPCNONE,	{0}},

{"lvrx",	X(31,551),	X_MASK,      CELL,	PPCNONE,	{VD, RA0, RB}},

{"subfo",	XO(31,40,1,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"subo",	XO(31,40,1,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RB, RA}},
{"subfo.",	XO(31,40,1,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"subo.",	XO(31,40,1,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RB, RA}},

{"tlbsync",	X(31,566),	0xffffffff,  PPC,	PPCNONE,	{0}},

{"lfsux",	X(31,567),	X_MASK,      COM,	PPCEFS,		{FRT, RAS, RB}},

{"lwdx",	X(31,579),	X_MASK,      E500MC,	PPCNONE,	{RT, RA, RB}},

{"lvtlx",	X(31,581),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"mfsr",	X(31,595), XRB_MASK|(1<<20), COM,	NON32,  	{RT, SR}},

{"lswi",	X(31,597),	X_MASK,      PPCCOM,	E500|E500MC,	{RT, RA0, NBI}},

{"lwsync",	XSYNC(31,598,1), 0xffffffff, PPC,	E500,		{0}},
{"ptesync",	XSYNC(31,598,2), 0xffffffff, PPC64,	PPCNONE,	{0}},
{"sync",	X(31,598),	XSYNCLE_MASK,E6500,	PPCNONE,	{LS, ESYNC}},
{"sync",	X(31,598),	XSYNC_MASK,  PPCCOM,	BOOKE|PPC476,	{LS}},
{"msync",	X(31,598),	0xffffffff, BOOKE|PPCA2|PPC476, PPCNONE, {0}},
{"sync",	X(31,598),	0xffffffff, BOOKE|PPC476, E6500,	{0}},
{"lwsync",	X(31,598),	0xffffffff, E500,	PPCNONE,	{0}},

{"lfdx",	X(31,599),	X_MASK,      COM,	PPCEFS,		{FRT, RA0, RB}},

{"mffgpr",	XRC(31,607,0),	XRA_MASK,    POWER6,	POWER7,		{FRT, RB}},
{"lfdepx",	X(31,607),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{FRT, RA, RB}},

{"lddx",	X(31,611),	X_MASK,      E500MC,	PPCNONE,	{RT, RA, RB}},

{"lvswx",	X(31,613),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"nego",	XO(31,104,1,0),	XORB_MASK,   COM,	PPCNONE,	{RT, RA}},
{"nego.",	XO(31,104,1,1),	XORB_MASK,   COM,	PPCNONE,	{RT, RA}},

{"lfdux",	X(31,631),	X_MASK,      COM,	PPCEFS,		{FRT, RAS, RB}},

{"stbdx",	X(31,643),	X_MASK,      E500MC,	PPCNONE,	{RS, RA, RB}},

{"stvlx",	X(31,647),	X_MASK,      CELL,	PPCNONE,	{VS, RA0, RB}},

{"subfeo",	XO(31,136,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"subfeo.",	XO(31,136,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"addeo",	XO(31,138,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"addeo.",	XO(31,138,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"mfsrin",	X(31,659),	XRA_MASK,    PPC,	NON32,  	{RT, RB}},

{"stdbrx",	X(31,660),	X_MASK, CELL|POWER7|PPCA2, PPCNONE,	{RS, RA0, RB}},

{"stswx",	X(31,661),	X_MASK,      PPCCOM,	E500|E500MC,	{RS, RA0, RB}},

{"stwbrx",	X(31,662),	X_MASK,      PPCCOM,	PPCNONE,	{RS, RA0, RB}},

{"stfsx",	X(31,663),	X_MASK,      COM,	PPCEFS,		{FRS, RA0, RB}},

{"sthdx",	X(31,675),	X_MASK,      E500MC,	PPCNONE,	{RS, RA, RB}},

{"stvfrx",	X(31,677),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"stvrx",	X(31,679),	X_MASK,      CELL,	PPCNONE,	{VS, RA0, RB}},

{"stbcx.",	XRC(31,694,1),	X_MASK,      POWER7,	PPCNONE,	{RS, RA0, RB}},

{"stfsux",	X(31,695),	X_MASK,      COM,	PPCEFS,		{FRS, RAS, RB}},

{"stwdx",	X(31,707),	X_MASK,      E500MC,	PPCNONE,	{RS, RA, RB}},

{"stvflx",	X(31,709),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"subfzeo",	XO(31,200,1,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"subfzeo.",	XO(31,200,1,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"addzeo",	XO(31,202,1,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"addzeo.",	XO(31,202,1,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"stswi",	X(31,725),	X_MASK,      PPCCOM,	E500|E500MC,	{RS, RA0, NB}},

{"sthcx.",	XRC(31,726,1),	X_MASK,      POWER7,	PPCNONE,	{RS, RA0, RB}},

{"stfdx",	X(31,727),	X_MASK,      COM,	PPCEFS,		{FRS, RA0, RB}},

{"mftgpr",	XRC(31,735,0),	XRA_MASK,    POWER6,	POWER7,		{RT, FRB}},
{"stfdepx",	X(31,735),	X_MASK,   E500MC|PPCA2,	PPCNONE,	{FRS, RA, RB}},

{"stddx",	X(31,739),	X_MASK,      E500MC,	PPCNONE,	{RS, RA, RB}},

{"stvswx",	X(31,741),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"subfmeo",	XO(31,232,1,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"subfmeo.",	XO(31,232,1,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"mulldo",	XO(31,233,1,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"mulldo.",	XO(31,233,1,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"addmeo",	XO(31,234,1,0),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},
{"addmeo.",	XO(31,234,1,1),	XORB_MASK,   PPCCOM,	PPCNONE,	{RT, RA}},

{"mullwo",	XO(31,235,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"mullwo.",	XO(31,235,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"dcba",	X(31,758), XRT_MASK, PPC405|PPC7450|BOOKE|PPCA2|PPC476, PPCNONE, {RA, RB}},
{"dcbal",	XOPL(31,758,1), XRT_MASK,    E500MC,	PPCNONE,	{RA, RB}},

{"stfdux",	X(31,759),	X_MASK,      COM,	PPCEFS,		{FRS, RAS, RB}},

{"lvsm",	X(31,773),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},
{"stvepxl",	X(31,775),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},
{"lvlxl",	X(31,775),	X_MASK,      CELL,	PPCNONE,	{VD, RA0, RB}},

{"addo",	XO(31,266,1,0),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},
{"addo.",	XO(31,266,1,1),	XO_MASK,     PPCCOM,	PPCNONE,	{RT, RA, RB}},

{"tlbivax",	X(31,786),	XRT_MASK,  BOOKE|PPCA2|PPC476, PPCNONE,	{RA, RB}},

{"lwzcix",	X(31,789),	X_MASK,      POWER6,	PPCNONE,	{RT, RA0, RB}},

{"lhbrx",	X(31,790),	X_MASK,      COM,	PPCNONE,	{RT, RA0, RB}},

{"lfdpx",	X(31,791),	X_MASK,      POWER6,	POWER7,		{FRTp, RA, RB}},

{"sraw",	XRC(31,792,0),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB}},
{"sraw.",	XRC(31,792,1),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, RB}},

{"srad",	XRC(31,794,0),	X_MASK,      PPC64,	PPCNONE,	{RA, RS, RB}},
{"srad.",	XRC(31,794,1),	X_MASK,      PPC64,	PPCNONE,	{RA, RS, RB}},

{"lfddx",	X(31,803),	X_MASK,      E500MC,	PPCNONE,	{FRT, RA, RB}},

{"lvtrxl",	X(31,805),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},
{"stvepx",	X(31,807),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},
{"lvrxl",	X(31,807),	X_MASK,      CELL,	PPCNONE,	{VD, RA0, RB}},

{"erativax",	X(31,819),	X_MASK,	     PPCA2,	PPCNONE,	{RS, RA0, RB}},

{"lhzcix",	X(31,821),	X_MASK,      POWER6,	PPCNONE,	{RT, RA0, RB}},

{"dss",		XDSS(31,822,0),	XDSS_MASK,   PPCVEC,	PPCNONE,	{STRM}},

{"srawi",	XRC(31,824,0),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, SH}},
{"srawi.",	XRC(31,824,1),	X_MASK,      PPCCOM,	PPCNONE,	{RA, RS, SH}},

{"sradi",	XS(31,413,0),	XS_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6}},
{"sradi.",	XS(31,413,1),	XS_MASK,     PPC64,	PPCNONE,	{RA, RS, SH6}},

{"lvtlxl",	X(31,837),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"tlbsrx.",	XRC(31,850,1),	XRT_MASK,    PPCA2,	PPCNONE,	{RA, RB}},

{"slbmfev",	X(31,851),	XRA_MASK,    PPC64,	PPCNONE,	{RT, RB}},

{"lbzcix",	X(31,853),	X_MASK,      POWER6,	PPCNONE,	{RT, RA0, RB}},

{"eieio",	X(31,854),	0xffffffff,  PPC,   BOOKE|PPCA2|PPC476,	{0}},
{"mbar",	X(31,854),	X_MASK, BOOKE|PPCA2|PPC476, PPCNONE,	{MO}},
{"eieio",	XMBAR(31,854,1),0xffffffff,  E500,   PPCNONE,	{0}},
{"eieio",	X(31,854),	0xffffffff, PPCA2|PPC476, PPCNONE,	{0}},

{"lfiwax",	X(31,855),	X_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{FRT, RA0, RB}},

{"lvswxl",	X(31,869),	X_MASK,      PPCVEC2,	PPCNONE,	{VD, RA0, RB}},

{"ldcix",	X(31,885),	X_MASK,      POWER6,	PPCNONE,	{RT, RA0, RB}},

{"lfiwzx",	X(31,887),	X_MASK,   POWER7|PPCA2,	PPCNONE,	{FRT, RA0, RB}},

{"stvlxl",	X(31,903),	X_MASK,      CELL,	PPCNONE,	{VS, RA0, RB}},

{"divdeuo",	XO(31,393,1,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divdeuo.",	XO(31,393,1,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divweuo",	XO(31,395,1,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divweuo.",	XO(31,395,1,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"tlbsx",	XRC(31,914,0),	X_MASK, PPC403|BOOKE|PPCA2|PPC476, PPCNONE, {RTO, RA, RB}},
{"tlbsx.",	XRC(31,914,1),	X_MASK, PPC403|BOOKE|PPCA2|PPC476, PPCNONE, {RTO, RA, RB}},

{"slbmfee",	X(31,915),	XRA_MASK,    PPC64,	PPCNONE,	{RT, RB}},

{"stwcix",	X(31,917),	X_MASK,      POWER6,	PPCNONE,	{RS, RA0, RB}},

{"sthbrx",	X(31,918),	X_MASK,      COM,	PPCNONE,	{RS, RA0, RB}},

{"stfdpx",	X(31,919),	X_MASK,      POWER6,	POWER7,		{FRSp, RA, RB}},

{"extsh",	XRC(31,922,0),	XRB_MASK,    PPCCOM,	PPCNONE,	{RA, RS}},
{"extsh.",	XRC(31,922,1),	XRB_MASK,    PPCCOM,	PPCNONE,	{RA, RS}},

{"stfddx",	X(31,931),	X_MASK,      E500MC,	PPCNONE,	{FRS, RA, RB}},

{"stvfrxl",	X(31,933),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"wclrone",	XOPL2(31,934,2),XRT_MASK,    PPCA2,	PPCNONE,	{RA0, RB}},
{"wclrall",	X(31,934),	XRARB_MASK,  PPCA2,	PPCNONE,	{L}},
{"wclr",	X(31,934),	X_MASK,	     PPCA2,	PPCNONE,	{L, RA0, RB}},

{"stvrxl",	X(31,935),	X_MASK,      CELL,	PPCNONE,	{VS, RA0, RB}},

{"divdeo",	XO(31,425,1,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divdeo.",	XO(31,425,1,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divweo",	XO(31,427,1,0),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},
{"divweo.",	XO(31,427,1,1),	XO_MASK,  POWER7|PPCA2,	PPCNONE,	{RT, RA, RB}},

{"tlbre",	X(31,946),	X_MASK, PPC403|BOOKE|PPCA2|PPC476, PPCNONE, {RSO, RAOPT, SHO}},

{"sthcix",	X(31,949),	X_MASK,      POWER6,	PPCNONE,	{RS, RA0, RB}},

{"icswepx",	XRC(31,950,0),	X_MASK,      PPCA2,	PPCNONE,	{RS, RA, RB}},
{"icswepx.",	XRC(31,950,1),	X_MASK,      PPCA2,	PPCNONE,	{RS, RA, RB}},

{"extsb",	XRC(31,954,0),	XRB_MASK,    PPC,	PPCNONE,	{RA, RS}},
{"extsb.",	XRC(31,954,1),	XRB_MASK,    PPC,	PPCNONE,	{RA, RS}},

{"stvflxl",	X(31,965),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"ici",		X(31,966),	XRARB_MASK,  PPCA2|PPC476, PPCNONE,	{CT}},

{"divduo",	XO(31,457,1,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"divduo.",	XO(31,457,1,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"divwuo",	XO(31,459,1,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"divwuo.",	XO(31,459,1,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},

{"tlbld",	X(31,978),	XRTRA_MASK,  PPC, PPC403|BOOKE|PPCA2|PPC476, {RB}},
{"tlbwe",	X(31,978),	X_MASK, PPC403|BOOKE|PPCA2|PPC476, PPCNONE, {RSO, RAOPT, SHO}},

{"stbcix",	X(31,981),	X_MASK,      POWER6,	PPCNONE,	{RS, RA0, RB}},

{"icbi",	X(31,982),	XRT_MASK,    PPC,	PPCNONE,	{RA, RB}},

{"stfiwx",	X(31,983),	X_MASK,      PPC,	PPCEFS,		{FRS, RA0, RB}},

{"extsw",	XRC(31,986,0),  XRB_MASK,    PPC64,	PPCNONE,	{RA, RS}},
{"extsw.",	XRC(31,986,1),	XRB_MASK,    PPC64,	PPCNONE,	{RA, RS}},

{"icbiep",	XRT(31,991,0),	XRT_MASK, E500MC|PPCA2,	PPCNONE,	{RA, RB}},

{"stvswxl",	X(31,997),	X_MASK,      PPCVEC2,	PPCNONE,	{VS, RA0, RB}},

{"divdo",	XO(31,489,1,0),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},
{"divdo.",	XO(31,489,1,1),	XO_MASK,     PPC64,	PPCNONE,	{RT, RA, RB}},

{"divwo",	XO(31,491,1,0),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},
{"divwo.",	XO(31,491,1,1),	XO_MASK,     PPC,	PPCNONE,	{RT, RA, RB}},

{"tlbli",	X(31,1010),	XRTRA_MASK,  PPC,	TITAN,  	{RB}},

{"stdcix",	X(31,1013),	X_MASK,      POWER6,	PPCNONE,	{RS, RA0, RB}},

{"dcbz",	X(31,1014),	XRT_MASK,    PPC,	PPCNONE,	{RA, RB}},
{"dclz",	X(31,1014),	XRT_MASK,    PPC,	PPCNONE,	{RA, RB}},

{"dcbzep",	XRT(31,1023,0),	XRT_MASK, E500MC|PPCA2,	PPCNONE,	{RA, RB}},

{"dcbzl",	XOPL(31,1014,1), XRT_MASK, POWER4|E500MC, PPC476,	{RA, RB}},

{"cctpl",	0x7c210b78,	0xffffffff,  CELL,	PPCNONE,	{0}},
{"cctpm",	0x7c421378,	0xffffffff,  CELL,	PPCNONE,	{0}},
{"cctph",	0x7c631b78,	0xffffffff,  CELL,	PPCNONE,	{0}},

{"dstt",	XDSS(31,342,1),	XDSS_MASK,   PPCVEC,	PPCNONE,	{RA, RB, STRM}},
{"dststt",	XDSS(31,374,1),	XDSS_MASK,   PPCVEC,	PPCNONE,	{RA, RB, STRM}},
{"dssall",	XDSS(31,822,1),	XDSS_MASK,   PPCVEC,	PPCNONE,	{0}},

{"db8cyc",	0x7f9ce378,	0xffffffff,  CELL,	PPCNONE,	{0}},
{"db10cyc",	0x7fbdeb78,	0xffffffff,  CELL,	PPCNONE,	{0}},
{"db12cyc",	0x7fdef378,	0xffffffff,  CELL,	PPCNONE,	{0}},
{"db16cyc",	0x7ffffb78,	0xffffffff,  CELL,	PPCNONE,	{0}},

{"lwz",		OP(32),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, D, RA0}},

{"lwzu",	OP(33),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, D, RAL}},

{"lbz",		OP(34),		OP_MASK,     COM,	PPCNONE,	{RT, D, RA0}},

{"lbzu",	OP(35),		OP_MASK,     COM,	PPCNONE,	{RT, D, RAL}},

{"stw",		OP(36),		OP_MASK,     PPCCOM,	PPCNONE,	{RS, D, RA0}},

{"stwu",	OP(37),		OP_MASK,     PPCCOM,	PPCNONE,	{RS, D, RAS}},

{"stb",		OP(38),		OP_MASK,     COM,	PPCNONE,	{RS, D, RA0}},

{"stbu",	OP(39),		OP_MASK,     COM,	PPCNONE,	{RS, D, RAS}},

{"lhz",		OP(40),		OP_MASK,     COM,	PPCNONE,	{RT, D, RA0}},

{"lhzu",	OP(41),		OP_MASK,     COM,	PPCNONE,	{RT, D, RAL}},

{"lha",		OP(42),		OP_MASK,     COM,	PPCNONE,	{RT, D, RA0}},

{"lhau",	OP(43),		OP_MASK,     COM,	PPCNONE,	{RT, D, RAL}},

{"sth",		OP(44),		OP_MASK,     COM,	PPCNONE,	{RS, D, RA0}},

{"sthu",	OP(45),		OP_MASK,     COM,	PPCNONE,	{RS, D, RAS}},

{"lmw",		OP(46),		OP_MASK,     PPCCOM,	PPCNONE,	{RT, D, RAM}},

{"stmw",	OP(47),		OP_MASK,     PPCCOM,	PPCNONE,	{RS, D, RA0}},

{"lfs",		OP(48),		OP_MASK,     COM,	PPCEFS,		{FRT, D, RA0}},

{"lfsu",	OP(49),		OP_MASK,     COM,	PPCEFS,		{FRT, D, RAS}},

{"lfd",		OP(50),		OP_MASK,     COM,	PPCEFS,		{FRT, D, RA0}},

{"lfdu",	OP(51),		OP_MASK,     COM,	PPCEFS,		{FRT, D, RAS}},

{"stfs",	OP(52),		OP_MASK,     COM,	PPCEFS,		{FRS, D, RA0}},

{"stfsu",	OP(53),		OP_MASK,     COM,	PPCEFS,		{FRS, D, RAS}},

{"stfd",	OP(54),		OP_MASK,     COM,	PPCEFS,		{FRS, D, RA0}},

{"stfdu",	OP(55),		OP_MASK,     COM,	PPCEFS,		{FRS, D, RAS}},

{"lq",		OP(56),		OP_MASK,     POWER4,	PPC476,		{RTQ, DQ, RAQ}},

{"lfdp",	OP(57),		OP_MASK,     POWER6,	POWER7,		{FRTp, D, RA0}},

{"ld",		DSO(58,0),	DS_MASK,     PPC64,	PPCNONE,	{RT, DS, RA0}},
{"ldu",		DSO(58,1),	DS_MASK,     PPC64,	PPCNONE,	{RT, DS, RAL}},
{"lwa",		DSO(58,2),	DS_MASK,     PPC64,	PPCNONE,	{RT, DS, RA0}},

{"dadd",	XRC(59,2,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},
{"dadd.",	XRC(59,2,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},

{"dqua",	ZRC(59,3,0),	Z2_MASK,     POWER6,	PPCNONE,	{FRT,FRA,FRB,RMC}},
{"dqua.",	ZRC(59,3,1),	Z2_MASK,     POWER6,	PPCNONE,	{FRT,FRA,FRB,RMC}},

{"fdivs",	A(59,18,0),	AFRC_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRB}},
{"fdivs.",	A(59,18,1),	AFRC_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRB}},

{"fsubs",	A(59,20,0),	AFRC_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRB}},
{"fsubs.",	A(59,20,1),	AFRC_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRB}},

{"fadds",	A(59,21,0),	AFRC_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRB}},
{"fadds.",	A(59,21,1),	AFRC_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRB}},

{"fsqrts",	A(59,22,0),    AFRAFRC_MASK, PPC,	TITAN,  	{FRT, FRB}},
{"fsqrts.",	A(59,22,1),    AFRAFRC_MASK, PPC,	TITAN,  	{FRT, FRB}},

{"fres",	A(59,24,0),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"fres",	A(59,24,0),   AFRALFRC_MASK, PPC,	POWER7,		{FRT, FRB, A_L}},
{"fres.",	A(59,24,1),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"fres.",	A(59,24,1),   AFRALFRC_MASK, PPC,	POWER7,		{FRT, FRB, A_L}},

{"fmuls",	A(59,25,0),	AFRB_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRC}},
{"fmuls.",	A(59,25,1),	AFRB_MASK,   PPC,	PPCEFS,		{FRT, FRA, FRC}},

{"frsqrtes",	A(59,26,0),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"frsqrtes",	A(59,26,0),   AFRALFRC_MASK, POWER5,	POWER7,		{FRT, FRB, A_L}},
{"frsqrtes.",	A(59,26,1),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"frsqrtes.",	A(59,26,1),   AFRALFRC_MASK, POWER5,	POWER7,		{FRT, FRB, A_L}},

{"fmsubs",	A(59,28,0),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fmsubs.",	A(59,28,1),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fmadds",	A(59,29,0),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fmadds.",	A(59,29,1),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fnmsubs",	A(59,30,0),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fnmsubs.",	A(59,30,1),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fnmadds",	A(59,31,0),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fnmadds.",	A(59,31,1),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"dmul",	XRC(59,34,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},
{"dmul.",	XRC(59,34,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},

{"drrnd",	ZRC(59,35,0),	Z2_MASK,     POWER6,	PPCNONE,	{FRT, FRA, FRB, RMC}},
{"drrnd.",	ZRC(59,35,1),	Z2_MASK,     POWER6,	PPCNONE,	{FRT, FRA, FRB, RMC}},

{"dscli",	ZRC(59,66,0),	Z_MASK,      POWER6,	PPCNONE,	{FRT, FRA, SH16}},
{"dscli.",	ZRC(59,66,1),	Z_MASK,      POWER6,	PPCNONE,	{FRT, FRA, SH16}},

{"dquai",	ZRC(59,67,0),	Z2_MASK,     POWER6,	PPCNONE,	{TE, FRT,FRB,RMC}},
{"dquai.",	ZRC(59,67,1),	Z2_MASK,     POWER6,	PPCNONE,	{TE, FRT,FRB,RMC}},

{"dscri",	ZRC(59,98,0),	Z_MASK,      POWER6,	PPCNONE,	{FRT, FRA, SH16}},
{"dscri.",	ZRC(59,98,1),	Z_MASK,      POWER6,	PPCNONE,	{FRT, FRA, SH16}},

{"drintx",	ZRC(59,99,0),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRT, FRB, RMC}},
{"drintx.",	ZRC(59,99,1),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRT, FRB, RMC}},

{"dcmpo",	X(59,130),	X_MASK,      POWER6,	PPCNONE,	{BF,  FRA, FRB}},

{"dtstex",	X(59,162),	X_MASK,      POWER6,	PPCNONE,	{BF,  FRA, FRB}},
{"dtstdc",	Z(59,194),	Z_MASK,      POWER6,	PPCNONE,	{BF,  FRA, DCM}},
{"dtstdg",	Z(59,226),	Z_MASK,      POWER6,	PPCNONE,	{BF,  FRA, DGM}},

{"drintn",	ZRC(59,227,0),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRT, FRB, RMC}},
{"drintn.",	ZRC(59,227,1),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRT, FRB, RMC}},

{"dctdp",	XRC(59,258,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},
{"dctdp.",	XRC(59,258,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},

{"dctfix",	XRC(59,290,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},
{"dctfix.",	XRC(59,290,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},

{"ddedpd",	XRC(59,322,0),	X_MASK,      POWER6,	PPCNONE,	{SP, FRT, FRB}}, 
{"ddedpd.",	XRC(59,322,1),	X_MASK,      POWER6,	PPCNONE,	{SP, FRT, FRB}}, 

{"dxex",	XRC(59,354,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},
{"dxex.",	XRC(59,354,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},

{"dsub",	XRC(59,514,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},
{"dsub.",	XRC(59,514,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},

{"ddiv",	XRC(59,546,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},
{"ddiv.",	XRC(59,546,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},

{"dcmpu",	X(59,642),	X_MASK,      POWER6,	PPCNONE,	{BF,  FRA, FRB}},

{"dtstsf",	X(59,674),	X_MASK,      POWER6,	PPCNONE,	{BF,  FRA, FRB}},

{"drsp",	XRC(59,770,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},
{"drsp.",	XRC(59,770,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRB}},

{"dcffix",	XRC(59,802,0), X_MASK|FRA_MASK, POWER7,	PPCNONE,	{FRT, FRB}},
{"dcffix.",	XRC(59,802,1), X_MASK|FRA_MASK, POWER7,	PPCNONE,	{FRT, FRB}},

{"denbcd",	XRC(59,834,0),	X_MASK,      POWER6,	PPCNONE,	{S, FRT, FRB}},
{"denbcd.",	XRC(59,834,1),	X_MASK,      POWER6,	PPCNONE,	{S, FRT, FRB}},

{"fcfids",	XRC(59,846,0),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},
{"fcfids.",	XRC(59,846,1),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},

{"diex",	XRC(59,866,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},
{"diex.",	XRC(59,866,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRA, FRB}},

{"fcfidus",	XRC(59,974,0),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},
{"fcfidus.",	XRC(59,974,1),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},

{"stfdp",	OP(61),		OP_MASK,     POWER6,	POWER7,		{FRSp, D, RA0}},

{"std",		DSO(62,0),	DS_MASK,     PPC64,	PPCNONE,	{RS, DS, RA0}},
{"stdu",	DSO(62,1),	DS_MASK,     PPC64,	PPCNONE,	{RS, DS, RAS}},
{"stq",		DSO(62,2),	DS_MASK,     POWER4,	PPC476,		{RSQ, DS, RA0}},

{"fcmpu",	X(63,0),     X_MASK|(3<<21), COM,	PPCEFS,		{BF, FRA, FRB}},

{"daddq",	XRC(63,2,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},
{"daddq.",	XRC(63,2,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},

{"dquaq",	ZRC(63,3,0),	Z2_MASK,     POWER6,	PPCNONE,	{FRTp, FRAp, FRBp, RMC}},
{"dquaq.",	ZRC(63,3,1),	Z2_MASK,     POWER6,	PPCNONE,	{FRTp, FRAp, FRBp, RMC}},

{"fcpsgn",	XRC(63,8,0),	X_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{FRT, FRA, FRB}},
{"fcpsgn.",	XRC(63,8,1),	X_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{FRT, FRA, FRB}},

{"frsp",	XRC(63,12,0),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},
{"frsp.",	XRC(63,12,1),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},

{"fctiw",	XRC(63,14,0),	XRA_MASK,    PPCCOM,	PPCEFS,		{FRT, FRB}},
{"fctiw.",	XRC(63,14,1),	XRA_MASK,    PPCCOM,	PPCEFS,		{FRT, FRB}},

{"fctiwz",	XRC(63,15,0),	XRA_MASK,    PPCCOM,	PPCEFS,		{FRT, FRB}},
{"fctiwz.",	XRC(63,15,1),	XRA_MASK,    PPCCOM,	PPCEFS,		{FRT, FRB}},

{"fdiv",	A(63,18,0),	AFRC_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRB}},
{"fdiv.",	A(63,18,1),	AFRC_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRB}},

{"fsub",	A(63,20,0),	AFRC_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRB}},
{"fsub.",	A(63,20,1),	AFRC_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRB}},

{"fadd",	A(63,21,0),	AFRC_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRB}},
{"fadd.",	A(63,21,1),	AFRC_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRB}},

{"fsel",	A(63,23,0),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fsel.",	A(63,23,1),	A_MASK,      PPC,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fre",		A(63,24,0),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"fre",		A(63,24,0),   AFRALFRC_MASK, POWER5,	POWER7,		{FRT, FRB, A_L}},
{"fre.",	A(63,24,1),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"fre.",	A(63,24,1),   AFRALFRC_MASK, POWER5,	POWER7,		{FRT, FRB, A_L}},

{"fmul",	A(63,25,0),	AFRB_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRC}},
{"fmul.",	A(63,25,1),	AFRB_MASK,   PPCCOM,	PPCEFS,		{FRT, FRA, FRC}},

{"frsqrte",	A(63,26,0),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"frsqrte",	A(63,26,0),   AFRALFRC_MASK, PPC,	POWER7,		{FRT, FRB, A_L}},
{"frsqrte.",	A(63,26,1),   AFRAFRC_MASK,  POWER7,	PPCNONE,	{FRT, FRB}},
{"frsqrte.",	A(63,26,1),   AFRALFRC_MASK, PPC,	POWER7,		{FRT, FRB, A_L}},

{"fmsub",	A(63,28,0),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fmsub.",	A(63,28,1),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fmadd",	A(63,29,0),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fmadd.",	A(63,29,1),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fnmsub",	A(63,30,0),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fnmsub.",	A(63,30,1),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fnmadd",	A(63,31,0),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},
{"fnmadd.",	A(63,31,1),	A_MASK,      PPCCOM,	PPCEFS,		{FRT, FRA, FRC, FRB}},

{"fcmpo",	X(63,32),    X_MASK|(3<<21), COM,	PPCEFS,		{BF, FRA, FRB}},

{"dmulq",	XRC(63,34,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},
{"dmulq.",	XRC(63,34,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},

{"drrndq",	ZRC(63,35,0),	Z2_MASK,     POWER6,	PPCNONE,	{FRTp, FRA, FRBp, RMC}},
{"drrndq.",	ZRC(63,35,1),	Z2_MASK,     POWER6,	PPCNONE,	{FRTp, FRA, FRBp, RMC}},

{"mtfsb1",	XRC(63,38,0),	XRARB_MASK,  COM,	PPCNONE,	{BT}},
{"mtfsb1.",	XRC(63,38,1),	XRARB_MASK,  COM,	PPCNONE,	{BT}},

{"fneg",	XRC(63,40,0),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},
{"fneg.",	XRC(63,40,1),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},

{"mcrfs",      X(63,64), XRB_MASK|(3<<21)|(3<<16), COM,	PPCNONE,	{BF, BFA}},

{"dscliq",	ZRC(63,66,0),	Z_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, SH16}},
{"dscliq.",	ZRC(63,66,1),	Z_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, SH16}},

{"dquaiq",	ZRC(63,67,0),	Z2_MASK,     POWER6,	PPCNONE,	{TE, FRTp, FRBp, RMC}},
{"dquaiq.",	ZRC(63,67,1),	Z2_MASK,     POWER6,	PPCNONE,	{TE, FRTp, FRBp, RMC}},

{"mtfsb0",	XRC(63,70,0),	XRARB_MASK,  COM,	PPCNONE,	{BT}},
{"mtfsb0.",	XRC(63,70,1),	XRARB_MASK,  COM,	PPCNONE,	{BT}},

{"fmr",		XRC(63,72,0),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},
{"fmr.",	XRC(63,72,1),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},

{"dscriq",	ZRC(63,98,0),	Z_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, SH16}},
{"dscriq.",	ZRC(63,98,1),	Z_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, SH16}},

{"drintxq",	ZRC(63,99,0),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRTp, FRBp, RMC}},
{"drintxq.",	ZRC(63,99,1),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRTp, FRBp, RMC}},

{"ftdiv",	X(63,128),   X_MASK|(3<<21), POWER7,	PPCNONE,	{BF, FRA, FRB}},

{"dcmpoq",	X(63,130),	X_MASK,      POWER6,	PPCNONE,	{BF, FRAp, FRBp}},

{"mtfsfi",  XRC(63,134,0), XWRA_MASK|(3<<21)|(1<<11), POWER6|PPCA2|PPC476, PPCNONE, {BFF, U, W}},
{"mtfsfi",  XRC(63,134,0), XRA_MASK|(3<<21)|(1<<11), COM, POWER6|PPCA2|PPC476, {BFF, U}},
{"mtfsfi.", XRC(63,134,1), XWRA_MASK|(3<<21)|(1<<11), POWER6|PPCA2|PPC476, PPCNONE, {BFF, U, W}},
{"mtfsfi.", XRC(63,134,1), XRA_MASK|(3<<21)|(1<<11), COM, POWER6|PPCA2|PPC476, {BFF, U}},

{"fnabs",	XRC(63,136,0),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},
{"fnabs.",	XRC(63,136,1),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},

{"fctiwu",	XRC(63,142,0),	XRA_MASK,    POWER7,	PPCNONE,	{FRT, FRB}},
{"fctiwu.",	XRC(63,142,1),	XRA_MASK,    POWER7,	PPCNONE,	{FRT, FRB}},
{"fctiwuz",	XRC(63,143,0),	XRA_MASK,    POWER7,	PPCNONE,	{FRT, FRB}},
{"fctiwuz.",	XRC(63,143,1),	XRA_MASK,    POWER7,	PPCNONE,	{FRT, FRB}},

{"ftsqrt",	X(63,160), X_MASK|(3<<21|FRA_MASK), POWER7, PPCNONE,	{BF, FRB}},

{"dtstexq",	X(63,162),	X_MASK,      POWER6,	PPCNONE,	{BF, FRAp, FRBp}},
{"dtstdcq",	Z(63,194),	Z_MASK,      POWER6,	PPCNONE,	{BF, FRAp, DCM}},
{"dtstdgq",	Z(63,226),	Z_MASK,      POWER6,	PPCNONE,	{BF, FRAp, DGM}},

{"drintnq",	ZRC(63,227,0),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRTp, FRBp, RMC}},
{"drintnq.",	ZRC(63,227,1),	Z2_MASK,     POWER6,	PPCNONE,	{R, FRTp, FRBp, RMC}},

{"dctqpq",	XRC(63,258,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRB}},
{"dctqpq.",	XRC(63,258,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRB}},

{"fabs",	XRC(63,264,0),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},
{"fabs.",	XRC(63,264,1),	XRA_MASK,    COM,	PPCEFS,		{FRT, FRB}},

{"dctfixq",	XRC(63,290,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRBp}},
{"dctfixq.",	XRC(63,290,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRBp}},

{"ddedpdq",	XRC(63,322,0),	X_MASK,      POWER6,	PPCNONE,	{SP, FRTp, FRBp}},
{"ddedpdq.",	XRC(63,322,1),	X_MASK,      POWER6,	PPCNONE,	{SP, FRTp, FRBp}},

{"dxexq",	XRC(63,354,0),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRBp}},
{"dxexq.",	XRC(63,354,1),	X_MASK,      POWER6,	PPCNONE,	{FRT, FRBp}},

{"frin",	XRC(63,392,0),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"frin.",	XRC(63,392,1),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"friz",	XRC(63,424,0),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"friz.",	XRC(63,424,1),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"frip",	XRC(63,456,0),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"frip.",	XRC(63,456,1),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"frim",	XRC(63,488,0),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},
{"frim.",	XRC(63,488,1),	XRA_MASK,    POWER5,	PPCNONE,	{FRT, FRB}},

{"dsubq",	XRC(63,514,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},
{"dsubq.",	XRC(63,514,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},

{"ddivq",	XRC(63,546,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},
{"ddivq.",	XRC(63,546,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRAp, FRBp}},

{"mffs",	XRC(63,583,0),	XRARB_MASK,  COM,	PPCEFS,		{FRT}},
{"mffs.",	XRC(63,583,1),	XRARB_MASK,  COM,	PPCEFS,		{FRT}},

{"dcmpuq",	X(63,642),	X_MASK,      POWER6,	PPCNONE,	{BF, FRAp, FRBp}},

{"dtstsfq",	X(63,674),	X_MASK,      POWER6,	PPCNONE,	{BF, FRA, FRBp}},

{"mtfsf",	XFL(63,711,0),	XFL_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{FLM, FRB, XFL_L, W}},
{"mtfsf",	XFL(63,711,0),	XFL_MASK,    COM, POWER6|PPCA2|PPC476|PPCEFS,	{FLM, FRB}},
{"mtfsf.",	XFL(63,711,1),	XFL_MASK, POWER6|PPCA2|PPC476, PPCNONE,	{FLM, FRB, XFL_L, W}},
{"mtfsf.",	XFL(63,711,1),	XFL_MASK,    COM, POWER6|PPCA2|PPC476|PPCEFS,	{FLM, FRB}},

{"drdpq",	XRC(63,770,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRBp}},
{"drdpq.",	XRC(63,770,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRBp}},

{"dcffixq",	XRC(63,802,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRB}},
{"dcffixq.",	XRC(63,802,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRB}},

{"fctid",	XRC(63,814,0),	XRA_MASK,    PPC64,	PPCNONE,	{FRT, FRB}},
{"fctid",	XRC(63,814,0),	XRA_MASK,    PPC476,	PPCNONE,	{FRT, FRB}},
{"fctid.",	XRC(63,814,1),	XRA_MASK,    PPC64,	PPCNONE,	{FRT, FRB}},
{"fctid.",	XRC(63,814,1),	XRA_MASK,    PPC476,	PPCNONE,	{FRT, FRB}},

{"fctidz",	XRC(63,815,0),	XRA_MASK,    PPC64,	PPCNONE,	{FRT, FRB}},
{"fctidz",	XRC(63,815,0),	XRA_MASK,    PPC476,	PPCNONE,	{FRT, FRB}},
{"fctidz.",	XRC(63,815,1),	XRA_MASK,    PPC64,	PPCNONE,	{FRT, FRB}},
{"fctidz.",	XRC(63,815,1),	XRA_MASK,    PPC476,	PPCNONE,	{FRT, FRB}},

{"denbcdq",	XRC(63,834,0),	X_MASK,      POWER6,	PPCNONE,	{S, FRTp, FRBp}},
{"denbcdq.",	XRC(63,834,1),	X_MASK,      POWER6,	PPCNONE,	{S, FRTp, FRBp}},

{"fcfid",	XRC(63,846,0),	XRA_MASK,    PPC64,	PPCNONE,	{FRT, FRB}},
{"fcfid",	XRC(63,846,0),	XRA_MASK,    PPC476,	PPCNONE,	{FRT, FRB}},
{"fcfid.",	XRC(63,846,1),	XRA_MASK,    PPC64,	PPCNONE,	{FRT, FRB}},
{"fcfid.",	XRC(63,846,1),	XRA_MASK,    PPC476,	PPCNONE,	{FRT, FRB}},

{"diexq",	XRC(63,866,0),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRA, FRBp}},
{"diexq.",	XRC(63,866,1),	X_MASK,      POWER6,	PPCNONE,	{FRTp, FRA, FRBp}},

{"fctidu",	XRC(63,942,0),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},
{"fctidu.",	XRC(63,942,1),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},

{"fctiduz",	XRC(63,943,0),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},
{"fctiduz.",	XRC(63,943,1),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},

{"fcfidu",	XRC(63,974,0),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},
{"fcfidu.",	XRC(63,974,1),	XRA_MASK, POWER7|PPCA2,	PPCNONE,	{FRT, FRB}},
};

static const int powerpc_num_opcodes =
  sizeof (powerpc_opcodes) / sizeof (powerpc_opcodes[0]);

#endif
