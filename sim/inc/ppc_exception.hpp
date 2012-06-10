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
// At present only 64 exception sub-types can be used for each major exception type
// Multiple types can be flagged simultaneously by simply ORing them.
//
// Each type will be paired with the exception number from above #defines.
//
#define    PPC_EXCEPT_CR                    0x0000000000000001ULL

#define    PPC_EXCEPT_MC_MCPBAR             0x0000000000000001ULL           // Negative edge on MCP signal
#define    PPC_EXCEPT_MC_DCPERR             0x0000000000000002ULL           // Data Cache Parity Error
#define    PPC_EXCEPT_MC_ICPERR             0x0000000000000004ULL           // Instr. Cache PE
#define    PPC_EXCEPT_MC_DCP_PERR           0x0000000000000008ULL           // Data Cache Push Parity Error
#define    PPC_EXCEPT_MC_BUS_IAERR          0x0000000000000010ULL           // Bus Instr. Address Error
#define    PPC_EXCEPT_MC_BUS_RAERR          0x0000000000000020ULL           // Bus Read Address Error
#define    PPC_EXCEPT_MC_BUS_WAERR          0x0000000000000040ULL           // Bus Write Address Error
#define    PPC_EXCEPT_MC_BUS_IBERR          0x0000000000000080ULL           // Bus Instr Data Bus Error
#define    PPC_EXCEPT_MC_BUS_RBERR          0x0000000000000100ULL           // Read Data Bus Error
#define    PPC_EXCEPT_MC_BUS_WBERR          0x0000000000000200ULL           // Write Bus Error
#define    PPC_EXCEPT_MC_BUS_IPERR          0x0000000000000400ULL           // Instr. Parity Error
#define    PPC_EXCEPT_MC_BUS_RPERR          0x0000000000000800ULL           // Read Parity Error
#define    PPC_EXCEPT_MC_BUS_FAULT          0x0000000000001000ULL           // Bus Fault

#define    PPC_EXCEPT_DSI_ACS_R             0x0000000000000001ULL           // Read Access Control Exception
#define    PPC_EXCEPT_DSI_ACS_W             0x0000000000000002ULL           // Write Access Control Exception
#define    PPC_EXCEPT_DSI_RSV_WT            0x0000000000000004ULL           // Load/Store Resv through WT
#define    PPC_EXCEPT_DSI_RSV_CI            0x0000000000000008ULL           // Load/Store Resv through CI
#define    PPC_EXCEPT_DSI_CL                0x0000000000000010ULL           // Cache locking
#define    PPC_EXCEPT_DSI_BO                0x0000000000000020ULL           // Byte ordering

#define    PPC_EXCEPT_ISI_ACS               0x0000000000000001ULL           // Acccess
#define    PPC_EXCEPT_ISI_BO                0x0000000000000002ULL           // BO

#define    PPC_EXCEPT_EI                    0x0000000000000001ULL

#define    PPC_EXCEPT_ALIGN_DCBZ            0x0000000000000001ULL           // dcbz on page marked as WT ( write through ) or CI ( cache inhibited )
#define    PPC_EXCEPT_ALIGN_LD              0x0000000000000002ULL           // load not aligned on natural boundary
#define    PPC_EXCEPT_ALIGN_ST              0x0000000000000004ULL           // store not aligned on natural boundary
#define    PPC_EXCEPT_ALIGN_SPE             0x0000000000000008ULL           // when SPE/SPFP instrs access address not aligned on their natural boundary

#define    PPC_EXCEPT_PRG_ILG               0x0000000000000001ULL           // Illegal
#define    PPC_EXCEPT_PRG_PRV               0x0000000000000002ULL           // Previledged
#define    PPC_EXCEPT_PRG_TRAP              0x0000000000000004ULL           // Trap
#define    PPC_EXCEPT_PRG_UNIMPL            0x0000000000000008ULL           // Unimplemented operation

#define    PPC_EXCEPT_SC                    0x0000000000000001ULL
#define    PPC_EXCEPT_DEC                   0x0000000000000002ULL
#define    PPC_EXCEPT_FIT                   0x0000000000000004ULL
#define    PPC_EXCEPT_WTD                   0x0000000000000008ULL

#define    PPC_EXCEPT_DTLB_MISS_LD          0x0000000000000001ULL           // Data TLB miss on load access
#define    PPC_EXCEPT_DTLB_MISS_ST          0x0000000000000002ULL           // Data TLB miss on store access

#define    PPC_EXCEPT_ITLB_MISS             0x0000000000000001ULL

#define    PPC_EXCEPT_DBG_TRAP              0x0000000000000001ULL            // Trap
#define    PPC_EXCEPT_DBG_IAC1              0x0000000000000002ULL            // Instr Address Compare
#define    PPC_EXCEPT_DBG_IAC2              0x0000000000000004ULL            // Instr Address Compare
#define    PPC_EXCEPT_DBG_DAC1R             0x0000000000000008ULL            // Data Address compare
#define    PPC_EXCEPT_DBG_DAC1W             0x0000000000000010ULL            // Data Address compare
#define    PPC_EXCEPT_DBG_DAC2R             0x0000000000000020ULL            // Data Address compare
#define    PPC_EXCEPT_DBG_DAC2W             0x0000000000000040ULL            // Data Address compare
#define    PPC_EXCEPT_DBG_ICMP              0x0000000000000080ULL            // Instruction complete
#define    PPC_EXCEPT_DBG_BRT               0x0000000000000100ULL            // Branch taken
#define    PPC_EXCEPT_DBG_RET               0x0000000000000200ULL            // Return from Intr
#define    PPC_EXCEPT_DBG_IRPT              0x0000000000000400ULL            // Interrupt taken
#define    PPC_EXCEPT_DBG_UDE               0x0000000000000800ULL            // Unconditional Debug Event

#define    PPC_EXCEPT_SPE_UA                0x0000000000000001ULL            // SPE unavail.
#define    PPC_EXCEPT_EM_FP_D               0x0000000000000001ULL
#define    PPC_EXCEPT_EM_FP_R               0x0000000000000001ULL

#endif
