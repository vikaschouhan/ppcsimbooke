#ifndef _PPC_EXCEPTION_HPP_
#define _PPC_EXCEPTION_HPP_

/*  Exceptions:  */
/* IVOR number + 1 for booke */
#define    PPC_EXCEPTION_CR     0x1    /* Critical */
#define    PPC_EXCEPTION_MC     0x2    /* Machine Check */
#define    PPC_EXCEPTION_DSI    0x3    /*  Data Storage Interrupt  */
#define    PPC_EXCEPTION_ISI    0x4    /*  Instruction Storage Interrupt  */
#define    PPC_EXCEPTION_EI     0x5    /*  External interrupt  */
#define    PPC_EXCEPTION_ALIGN  0x6    /* Alignment */
#define    PPC_EXCEPTION_PRG    0x7    /* Program Exception */
#define    PPC_EXCEPTION_FPU    0x8    /*  Floating-Point unavailable  */

#if defined(CONFIG_BOOKE) || defined(CONFIG_8xx)
#define    PPC_EXCEPTION_SC     0x9    /* Syscall */
#define    PPC_EXCEPTION_DEC    0xb
#else
#define    PPC_EXCEPTION_DEC    0x9    /*  Decrementer  */
#define    PPC_EXCEPTION_SC     0xc    /*  Syscall  */
#endif

#define    PPC_EXCEPTION_FIT     0xc    /* Fixed Interval Timer */
#define    PPC_EXCEPTION_WTD     0xd    /* Watch Dog */
#define    PPC_EXCEPTION_DTLB    0xe    /* Data Tlb  */
#define    PPC_EXCEPTION_ITLB    0xf    /* Instruction Tlb */
#define    PPC_EXCEPTION_DBG     0x10   /* Debug */

#define    PPC_EXCEPTION_SPE_UA  0x21   /* SPE / Embedded FP unavailable */
#define    PPC_EXCEPTION_EM_FP_D 0x22   /* Embedded FP data */
#define    PPC_EXCEPTION_EM_FP_R 0x23   /* Embedded FP round */
#define    PPC_EXCEPTION_PMM     0x24   /* Performance Monitor */
#define    PPC_EXCEPTION_DB      0x25   /* Door Bell */
#define    PPC_EXCEPTION_DB_CR   0x26   /* Door Bell Critical */


/*  XER bits:  */
#define    PPC_XER_SO    (1UL << 31)    /*  Summary Overflow  */
#define    PPC_XER_OV    (1 << 30)    /*  Overflow  */
#define    PPC_XER_CA    (1 << 29)    /*  Carry  */

struct  ppc_cpu_exception_map {
    uint8_t  exception_type;
    uint32_t ivor_no;
};

struct ppc_cpu_exception_map cpu_exception_map[] = 
{
    { PPC_EXCEPTION_CR,    SPRN_IVOR0 },
    { PPC_EXCEPTION_MC,    SPRN_IVOR1 },
    { PPC_EXCEPTION_DSI,   SPRN_IVOR2 },
    { PPC_EXCEPTION_ISI,   SPRN_IVOR3 },
    { PPC_EXCEPTION_EI,    SPRN_IVOR4 },
    { PPC_EXCEPTION_ALIGN, SPRN_IVOR5 },
    { PPC_EXCEPTION_PRG,   SPRN_IVOR6 },
    { PPC_EXCEPTION_FPU,   SPRN_IVOR7 },
    { PPC_EXCEPTION_SC,    SPRN_IVOR8 },
    { PPC_EXCEPTION_DEC,   SPRN_IVOR10 },
    { PPC_EXCEPTION_FIT,   SPRN_IVOR11 },
    { PPC_EXCEPTION_WTD,   SPRN_IVOR12 },
    { PPC_EXCEPTION_DTLB,  SPRN_IVOR13 },
    { PPC_EXCEPTION_ITLB,  SPRN_IVOR14 },
    { PPC_EXCEPTION_DBG,   SPRN_IVOR15 },
    { PPC_EXCEPTION_PMM,   SPRN_IVOR35 },
    { PPC_EXCEPTION_DB,    SPRN_IVOR36 },
    { PPC_EXCEPTION_DB_CR, SPRN_IVOR37 },
    /* Guest and hypervirsor exceptions not supported at this time */
};

#endif
