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

// flags for exception types ( they denote the exact exception )
//
#define    PPC_EXCEPT_CR                    0x10000

#define    PPC_EXCEPT_MC_MCPBAR             0x20000           // Negative edge on MCP signal
#define    PPC_EXCEPT_MC_DCPE               0x20001           // Data Cache Parity Error
#define    PPC_EXCEPT_MC_ICPE               0x20002           // Instr. Cache PE
#define    PPC_EXCEPT_MC_BIAE               0x20003           // Bus Instr. Address Error
#define    PPC_EXCEPT_MC_BRAE               0x20004           // Bus Read Address Error
#define    PPC_EXCEPT_MC_BWAE               0x20005           // Bus Write Address Error
#define    PPC_EXCEPT_MC_BIDBE              0x20006           // Bus Instr Data Bus Error
#define    PPC_EXCEPT_MC_RDBE               0x20007           // Read Data Bus Error
#define    PPC_EXCEPT_MC_WBE                0x20008           // Write Bus Error
#define    PPC_EXCEPT_MC_IPE                0x20009           // Instr. Parity Error
#define    PPC_EXCEPT_MC_RPE                0x2000a           // Read Parity Error
#define    PPPC_EXCEPT_MC_BF                0x2000b           // Bus Fault

#define    PPC_EXCEPT_DSI_ACS_R             0x30000           // Read Access Control Exception
#define    PPC_EXCEPT_DSI_ACS_W             0x30001           // Write Access Control Exception
#define    PPC_EXCEPT_DSI_RSV_WT            0x30002           // Load/Store Resv through WT
#define    PPC_EXCEPT_DSI_RSV_CI            0x30003           // Load/Store Resv through CI
#define    PPC_EXCEPT_DSI_CL                0x30004           // Cache locking
#define    PPC_EXCEPT_DSI_BO                0x30005           // Byte ordering

#define    PPC_EXCEPT_ISI_ACS               0x40000           // Acccess
#define    PPC_EXCEPT_ISI_BO                0x40001           // BO

#define    PPC_EXCEPT_EI                    0x50000
#define    PPC_EXCEPT_ALIGN                 0x60000

#define    PPC_EXCEPT_PRG_ILG               0x70000           // Illegal
#define    PPC_EXCEPT_PRG_PRV               0x70001           // Previledged
#define    PPC_EXCEPT_PRG_TRAP              0x70002           // Trap
#define    PPC_EXCEPT_PRG_UNIMPL            0x70003           // Unimplemented operation

#define    PPC_EXCEPT_SC                    0x80000
#define    PPC_EXCEPT_DEC                   0x90000
#define    PPC_EXCEPT_FIT                   0xa0000
#define    PPC_EXCEPT_WTD                   0xb0000

#define    PPC_EXCEPT_DTLB_MISS             0xc0000
#define    PPC_EXCEPT_ITLB_MISS             0xd0000

#define    PPC_EXCEPT_DBG_TRAP              0xe0000            // Trap
#define    PPC_EXCEPT_DBG_IAC               0xe0001            // Instr Address Compare
#define    PPC_EXCEPT_DBG_DAC               0xe0002            // Data Address compare
#define    PPC_EXCEPT_DBG_IC                0xe0003            // Instruction complete
#define    PPC_EXCEPT_DBG_BT                0xe0004            // Branch taken
#define    PPC_EXCEPT_DBG_RFI               0xe0005            // Return from Intr
#define    PPC_EXCEPT_DBG_IT                0xe0006            // Interrupt taken
#define    PPC_EXCEPT_DBG_UDE               0xe0007            // Unconditional Debug Event

#define    PPC_EXCEPT_SPE_UA               0x210000            // SPE unavail.
#define    PPC_EXCEPT_EM_FP_D              0x220000
#define    PPC_EXCEPT_EM_FP_R              0x230000

#endif
