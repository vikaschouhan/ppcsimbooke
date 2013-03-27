#ifndef _CPU_PPC_REGS_H
#define _CPU_PPC_REGS_H

// Register IDs

// GPRs
#define REG_GPR0     (1              )
#define REG_GPR1     (REG_GPR0    + 1)
#define REG_GPR2     (REG_GPR1    + 1)
#define REG_GPR3     (REG_GPR2    + 1)
#define REG_GPR4     (REG_GPR3    + 1)
#define REG_GPR5     (REG_GPR4    + 1)
#define REG_GPR6     (REG_GPR5    + 1)
#define REG_GPR7     (REG_GPR6    + 1)
#define REG_GPR8     (REG_GPR7    + 1)
#define REG_GPR9     (REG_GPR8    + 1)
#define REG_GPR10    (REG_GPR9    + 1)
#define REG_GPR11    (REG_GPR10   + 1)
#define REG_GPR12    (REG_GPR11   + 1)
#define REG_GPR13    (REG_GPR12   + 1)
#define REG_GPR14    (REG_GPR13   + 1)
#define REG_GPR15    (REG_GPR14   + 1)
#define REG_GPR16    (REG_GPR15   + 1)
#define REG_GPR17    (REG_GPR16   + 1)
#define REG_GPR18    (REG_GPR17   + 1)
#define REG_GPR19    (REG_GPR18   + 1)
#define REG_GPR20    (REG_GPR19   + 1)
#define REG_GPR21    (REG_GPR20   + 1)
#define REG_GPR22    (REG_GPR21   + 1)
#define REG_GPR23    (REG_GPR22   + 1)
#define REG_GPR24    (REG_GPR23   + 1)
#define REG_GPR25    (REG_GPR24   + 1)
#define REG_GPR26    (REG_GPR25   + 1)
#define REG_GPR27    (REG_GPR26   + 1)
#define REG_GPR28    (REG_GPR27   + 1)
#define REG_GPR29    (REG_GPR28   + 1)
#define REG_GPR30    (REG_GPR29   + 1)
#define REG_GPR31    (REG_GPR30   + 1)

// Hidden
#define REG_PC       (100            )
#define REG_MSR      (REG_PC      + 1)
#define REG_CR       (REG_MSR     + 1)
#define REG_ACC      (REG_CR      + 1)

// SPRs
#define REG_SPR0     (1000                       )
#define REG_ATBL     (REG_SPR0    + SPRN_ATBL    )
#define REG_ATBU     (REG_SPR0    + SPRN_ATBU    ) 
#define REG_CSRR0    (REG_SPR0    + SPRN_CSRR0   )
#define REG_CSRR1    (REG_SPR0    + SPRN_CSRR1   )
#define REG_CTR      (REG_SPR0    + SPRN_CTR     )
#define REG_DAC1     (REG_SPR0    + SPRN_DAC1    )
#define REG_DAC2     (REG_SPR0    + SPRN_DAC2    )
#define REG_DBCR0    (REG_SPR0    + SPRN_DBCR0   )
#define REG_DBCR1    (REG_SPR0    + SPRN_DBCR1   )
#define REG_DBCR2    (REG_SPR0    + SPRN_DBCR2   )
#define REG_DBSR     (REG_SPR0    + SPRN_DBSR    )
#define REG_DEAR     (REG_SPR0    + SPRN_DEAR    )
#define REG_DEC      (REG_SPR0    + SPRN_DEC     )
#define REG_DECAR    (REG_SPR0    + SPRN_DECAR   )
#define REG_ESR      (REG_SPR0    + SPRN_ESR     )
#define REG_IAC1     (REG_SPR0    + SPRN_IAC1    )
#define REG_IAC2     (REG_SPR0    + SPRN_IAC2    )
#define REG_IVOR0    (REG_SPR0    + SPRN_IVOR0   )
#define REG_IVOR1    (REG_SPR0    + SPRN_IVOR1   )
#define REG_IVOR2    (REG_SPR0    + SPRN_IVOR2   )
#define REG_IVOR3    (REG_SPR0    + SPRN_IVOR3   )
#define REG_IVOR4    (REG_SPR0    + SPRN_IVOR4   )
#define REG_IVOR5    (REG_SPR0    + SPRN_IVOR5   )
#define REG_IVOR6    (REG_SPR0    + SPRN_IVOR6   )
#define REG_IVOR7    (REG_SPR0    + SPRN_IVOR7   )
#define REG_IVOR8    (REG_SPR0    + SPRN_IVOR8   )
#define REG_IVOR9    (REG_SPR0    + SPRN_IVOR9   )
#define REG_IVOR10   (REG_SPR0    + SPRN_IVOR10  )
#define REG_IVOR11   (REG_SPR0    + SPRN_IVOR11  )
#define REG_IVOR12   (REG_SPR0    + SPRN_IVOR12  )
#define REG_IVOR13   (REG_SPR0    + SPRN_IVOR13  )
#define REG_IVOR14   (REG_SPR0    + SPRN_IVOR14  )
#define REG_IVOR15   (REG_SPR0    + SPRN_IVOR15  )
#define REG_IVPR     (REG_SPR0    + SPRN_IVPR    )
#define REG_LR       (REG_SPR0    + SPRN_LR      )
#define REG_PID      (REG_SPR0    + SPRN_PID     )
#define REG_PIR      (REG_SPR0    + SPRN_PIR     )
#define REG_PVR      (REG_SPR0    + SPRN_PVR     )
#define REG_SPRG0    (REG_SPR0    + SPRN_SPRG0   )
#define REG_SPRG1    (REG_SPR0    + SPRN_SPRG1   )
#define REG_SPRG2    (REG_SPR0    + SPRN_SPRG2   )
#define REG_SPRG3R   (REG_SPR0    + SPRN_SPRG3R  )
#define REG_SPRG3    (REG_SPR0    + SPRN_SPRG3   )
#define REG_SPRG4R   (REG_SPR0    + SPRN_SPRG4R  )
#define REG_SPRG4    (REG_SPR0    + SPRN_SPRG4   )
#define REG_SPRG5R   (REG_SPR0    + SPRN_SPRG5R  )
#define REG_SPRG5    (REG_SPR0    + SPRN_SPRG5   )
#define REG_SPRG6R   (REG_SPR0    + SPRN_SPRG6R  )
#define REG_SPRG6    (REG_SPR0    + SPRN_SPRG6   )
#define REG_SPRG7R   (REG_SPR0    + SPRN_SPRG7R  )
#define REG_SPRG7    (REG_SPR0    + SPRN_SPRG7   )
#define REG_SRR0     (REG_SPR0    + SPRN_SRR0    )
#define REG_SRR1     (REG_SPR0    + SPRN_SRR1    )
#define REG_TBRL     (REG_SPR0    + SPRN_TBRL    )
#define REG_TBWL     (REG_SPR0    + SPRN_TBWL    )
#define REG_TBRU     (REG_SPR0    + SPRN_TBRU    )
#define REG_TBWU     (REG_SPR0    + SPRN_TBWU    )
#define REG_TCR      (REG_SPR0    + SPRN_TCR     )
#define REG_TSR      (REG_SPR0    + SPRN_TSR     )
#define REG_USPRG0   (REG_SPR0    + SPRN_USPRG0  )
#define REG_XER      (REG_SPR0    + SPRN_XER     )

#define REG_BBEAR    (REG_SPR0    + SPRN_BBEAR   )
#define REG_BBTAR    (REG_SPR0    + SPRN_BBTAR   )
#define REG_BUCSR    (REG_SPR0    + SPRN_BUCSR   )
#define REG_HID0     (REG_SPR0    + SPRN_HID0    )
#define REG_HID1     (REG_SPR0    + SPRN_HID1    )
#define REG_IVOR32   (REG_SPR0    + SPRN_IVOR32  )
#define REG_IVOR33   (REG_SPR0    + SPRN_IVOR33  )
#define REG_IVOR34   (REG_SPR0    + SPRN_IVOR34  )
#define REG_IVOR35   (REG_SPR0    + SPRN_IVOR35  )
#define REG_L1CFG0   (REG_SPR0    + SPRN_L1CFG0  )
#define REG_L1CFG1   (REG_SPR0    + SPRN_L1CFG1  )
#define REG_L1CSR0   (REG_SPR0    + SPRN_L1CSR0  )
#define REG_L1CSR1   (REG_SPR0    + SPRN_L1CSR1  )
#define REG_MAS0     (REG_SPR0    + SPRN_MAS0    )
#define REG_MAS1     (REG_SPR0    + SPRN_MAS1    )
#define REG_MAS2     (REG_SPR0    + SPRN_MAS2    )
#define REG_MAS3     (REG_SPR0    + SPRN_MAS3    )
#define REG_MAS4     (REG_SPR0    + SPRN_MAS4    )
#define REG_MAS5     (REG_SPR0    + SPRN_MAS5    )
#define REG_MAS6     (REG_SPR0    + SPRN_MAS6    )
#define REG_MAS7     (REG_SPR0    + SPRN_MAS7    )
#define REG_MCAR     (REG_SPR0    + SPRN_MCAR    )
#define REG_MCSR     (REG_SPR0    + SPRN_MCSR    )
#define REG_MCSRR0   (REG_SPR0    + SPRN_MCSRR0  )
#define REG_MCSRR1   (REG_SPR0    + SPRN_MCSRR1  )
#define REG_MMUCFG   (REG_SPR0    + SPRN_MMUCFG  )
#define REG_MMUCSR0  (REG_SPR0    + SPRN_MMUCSR0 )
#define REG_PID0     (REG_SPR0    + SPRN_PID0    )
#define REG_PID1     (REG_SPR0    + SPRN_PID1    )
#define REG_PID2     (REG_SPR0    + SPRN_PID2    )
#define REG_SPEFSCR  (REG_SPR0    + SPRN_SPEFSCR )
#define REG_SVR      (REG_SPR0    + SPRN_SVR     )
#define REG_TLB0CFG  (REG_SPR0    + SPRN_TLB0CFG )
#define REG_TLB1CFG  (REG_SPR0    + SPRN_TLB1CFG ) 

// PMRS
#define REG_PMR0     (3000                       )
#define REG_PMC0     (REG_PMR0    + PMRN_PMC0    )
#define REG_PMC1     (REG_PMR0    + PMRN_PMC1    )
#define REG_PMC2     (REG_PMR0    + PMRN_PMC2    )
#define REG_PMC3     (REG_PMR0    + PMRN_PMC3    )
#define REG_PMLCA0   (REG_PMR0    + PMRN_PMLCA0  )
#define REG_PMLCA1   (REG_PMR0    + PMRN_PMLCA1  )
#define REG_PMLCA2   (REG_PMR0    + PMRN_PMLCA2  )
#define REG_PMLCA3   (REG_PMR0    + PMRN_PMLCA3  )
#define REG_PMLCB0   (REG_PMR0    + PMRN_PMLCB0  )
#define REG_PMLCB1   (REG_PMR0    + PMRN_PMLCB1  )
#define REG_PMLCB2   (REG_PMR0    + PMRN_PMLCB2  )
#define REG_PMLCB3   (REG_PMR0    + PMRN_PMLCB3  )
#define REG_PMGC0    (REG_PMR0    + PMRN_PMGC0   )
#define REG_UPMC0    (REG_PMR0    + PMRN_UPMC0   )
#define REG_UPMC1    (REG_PMR0    + PMRN_UPMC1   )
#define REG_UPMC2    (REG_PMR0    + PMRN_UPMC2   )
#define REG_UPMC3    (REG_PMR0    + PMRN_UPMC3   )
#define REG_UPMLCA0  (REG_PMR0    + PMRN_UPMLCA0 )
#define REG_UPMLCA1  (REG_PMR0    + PMRN_UPMLCA1 )
#define REG_UPMLCA2  (REG_PMR0    + PMRN_UPMLCA2 )
#define REG_UPMLCA3  (REG_PMR0    + PMRN_UPMLCA3 )
#define REG_UPMLCB0  (REG_PMR0    + PMRN_UPMLCB0 )
#define REG_UPMLCB1  (REG_PMR0    + PMRN_UPMLCB1 )
#define REG_UPMLCB2  (REG_PMR0    + PMRN_UPMLCB2 )
#define REG_UPMLCB3  (REG_PMR0    + PMRN_UPMLCB3 )
#define REG_UPMGC0   (REG_PMR0    + PMRN_UPMGC0  )


#endif