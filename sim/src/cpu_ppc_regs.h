#ifndef _CPU_PPC_REGS_H
#define _CPU_PPC_REGS_H

// Register IDs

namespace ppcsimbooke {

    // GPRs
    static const int REG_GPR0   =  (1              );
    static const int REG_GPR1   =  (REG_GPR0    + 1);
    static const int REG_GPR2   =  (REG_GPR1    + 1);
    static const int REG_GPR3   =  (REG_GPR2    + 1);
    static const int REG_GPR4   =  (REG_GPR3    + 1);
    static const int REG_GPR5   =  (REG_GPR4    + 1);
    static const int REG_GPR6   =  (REG_GPR5    + 1);
    static const int REG_GPR7   =  (REG_GPR6    + 1);
    static const int REG_GPR8   =  (REG_GPR7    + 1);
    static const int REG_GPR9   =  (REG_GPR8    + 1);
    static const int REG_GPR10  =  (REG_GPR9    + 1);
    static const int REG_GPR11  =  (REG_GPR10   + 1);
    static const int REG_GPR12  =  (REG_GPR11   + 1);
    static const int REG_GPR13  =  (REG_GPR12   + 1);
    static const int REG_GPR14  =  (REG_GPR13   + 1);
    static const int REG_GPR15  =  (REG_GPR14   + 1);
    static const int REG_GPR16  =  (REG_GPR15   + 1);
    static const int REG_GPR17  =  (REG_GPR16   + 1);
    static const int REG_GPR18  =  (REG_GPR17   + 1);
    static const int REG_GPR19  =  (REG_GPR18   + 1);
    static const int REG_GPR20  =  (REG_GPR19   + 1);
    static const int REG_GPR21  =  (REG_GPR20   + 1);
    static const int REG_GPR22  =  (REG_GPR21   + 1);
    static const int REG_GPR23  =  (REG_GPR22   + 1);
    static const int REG_GPR24  =  (REG_GPR23   + 1);
    static const int REG_GPR25  =  (REG_GPR24   + 1);
    static const int REG_GPR26  =  (REG_GPR25   + 1);
    static const int REG_GPR27  =  (REG_GPR26   + 1);
    static const int REG_GPR28  =  (REG_GPR27   + 1);
    static const int REG_GPR29  =  (REG_GPR28   + 1);
    static const int REG_GPR30  =  (REG_GPR29   + 1);
    static const int REG_GPR31  =  (REG_GPR30   + 1);

    // Hidden
    static const int REG_PC     =  (100            );
    static const int REG_NIP    =  (REG_PC      + 1);
    static const int REG_MSR    =  (REG_NIP     + 1);
    static const int REG_CR     =  (REG_MSR     + 1);
    static const int REG_ACC    =  (REG_CR      + 1);

    // SPRs
    static const int REG_SPR0   =  (1000                       );
    static const int REG_ATBL   =  (REG_SPR0    + SPRN_ATBL    );
    static const int REG_ATBU   =  (REG_SPR0    + SPRN_ATBU    ); 
    static const int REG_CSRR0  =  (REG_SPR0    + SPRN_CSRR0   );
    static const int REG_CSRR1  =  (REG_SPR0    + SPRN_CSRR1   );
    static const int REG_CTR    =  (REG_SPR0    + SPRN_CTR     );
    static const int REG_DAC1   =  (REG_SPR0    + SPRN_DAC1    );
    static const int REG_DAC2   =  (REG_SPR0    + SPRN_DAC2    );
    static const int REG_DBCR0  =  (REG_SPR0    + SPRN_DBCR0   );
    static const int REG_DBCR1  =  (REG_SPR0    + SPRN_DBCR1   );
    static const int REG_DBCR2  =  (REG_SPR0    + SPRN_DBCR2   );
    static const int REG_DBSR   =  (REG_SPR0    + SPRN_DBSR    );
    static const int REG_DEAR   =  (REG_SPR0    + SPRN_DEAR    );
    static const int REG_DEC    =  (REG_SPR0    + SPRN_DEC     );
    static const int REG_DECAR  =  (REG_SPR0    + SPRN_DECAR   );
    static const int REG_ESR    =  (REG_SPR0    + SPRN_ESR     );
    static const int REG_IAC1   =  (REG_SPR0    + SPRN_IAC1    );
    static const int REG_IAC2   =  (REG_SPR0    + SPRN_IAC2    );
    static const int REG_IVOR0  =  (REG_SPR0    + SPRN_IVOR0   );
    static const int REG_IVOR1  =  (REG_SPR0    + SPRN_IVOR1   );
    static const int REG_IVOR2  =  (REG_SPR0    + SPRN_IVOR2   );
    static const int REG_IVOR3  =  (REG_SPR0    + SPRN_IVOR3   );
    static const int REG_IVOR4  =  (REG_SPR0    + SPRN_IVOR4   );
    static const int REG_IVOR5  =  (REG_SPR0    + SPRN_IVOR5   );
    static const int REG_IVOR6  =  (REG_SPR0    + SPRN_IVOR6   );
    static const int REG_IVOR7  =  (REG_SPR0    + SPRN_IVOR7   );
    static const int REG_IVOR8  =  (REG_SPR0    + SPRN_IVOR8   );
    static const int REG_IVOR9  =  (REG_SPR0    + SPRN_IVOR9   );
    static const int REG_IVOR10 =  (REG_SPR0    + SPRN_IVOR10  );
    static const int REG_IVOR11 =  (REG_SPR0    + SPRN_IVOR11  );
    static const int REG_IVOR12 =  (REG_SPR0    + SPRN_IVOR12  );
    static const int REG_IVOR13 =  (REG_SPR0    + SPRN_IVOR13  );
    static const int REG_IVOR14 =  (REG_SPR0    + SPRN_IVOR14  );
    static const int REG_IVOR15 =  (REG_SPR0    + SPRN_IVOR15  );
    static const int REG_IVPR   =  (REG_SPR0    + SPRN_IVPR    );
    static const int REG_LR     =  (REG_SPR0    + SPRN_LR      );
    static const int REG_PID    =  (REG_SPR0    + SPRN_PID     );
    static const int REG_PIR    =  (REG_SPR0    + SPRN_PIR     );
    static const int REG_PVR    =  (REG_SPR0    + SPRN_PVR     );
    static const int REG_SPRG0  =  (REG_SPR0    + SPRN_SPRG0   );
    static const int REG_SPRG1  =  (REG_SPR0    + SPRN_SPRG1   );
    static const int REG_SPRG2  =  (REG_SPR0    + SPRN_SPRG2   );
    static const int REG_SPRG3R =  (REG_SPR0    + SPRN_SPRG3R  );
    static const int REG_SPRG3  =  (REG_SPR0    + SPRN_SPRG3   );
    static const int REG_SPRG4R =  (REG_SPR0    + SPRN_SPRG4R  );
    static const int REG_SPRG4  =  (REG_SPR0    + SPRN_SPRG4   );
    static const int REG_SPRG5R =  (REG_SPR0    + SPRN_SPRG5R  );
    static const int REG_SPRG5  =  (REG_SPR0    + SPRN_SPRG5   );
    static const int REG_SPRG6R =  (REG_SPR0    + SPRN_SPRG6R  );
    static const int REG_SPRG6  =  (REG_SPR0    + SPRN_SPRG6   );
    static const int REG_SPRG7R =  (REG_SPR0    + SPRN_SPRG7R  );
    static const int REG_SPRG7  =  (REG_SPR0    + SPRN_SPRG7   );
    static const int REG_SRR0   =  (REG_SPR0    + SPRN_SRR0    );
    static const int REG_SRR1   =  (REG_SPR0    + SPRN_SRR1    );
    static const int REG_TBRL   =  (REG_SPR0    + SPRN_TBRL    );
    static const int REG_TBWL   =  (REG_SPR0    + SPRN_TBWL    );
    static const int REG_TBRU   =  (REG_SPR0    + SPRN_TBRU    );
    static const int REG_TBWU   =  (REG_SPR0    + SPRN_TBWU    );
    static const int REG_TCR    =  (REG_SPR0    + SPRN_TCR     );
    static const int REG_TSR    =  (REG_SPR0    + SPRN_TSR     );
    static const int REG_USPRG0 =  (REG_SPR0    + SPRN_USPRG0  );
    static const int REG_XER    =  (REG_SPR0    + SPRN_XER     );

    static const int REG_BBEAR   = (REG_SPR0    + SPRN_BBEAR   );
    static const int REG_BBTAR   = (REG_SPR0    + SPRN_BBTAR   );
    static const int REG_BUCSR   = (REG_SPR0    + SPRN_BUCSR   );
    static const int REG_HID0    = (REG_SPR0    + SPRN_HID0    );
    static const int REG_HID1    = (REG_SPR0    + SPRN_HID1    );
    static const int REG_IVOR32  = (REG_SPR0    + SPRN_IVOR32  );
    static const int REG_IVOR33  = (REG_SPR0    + SPRN_IVOR33  );
    static const int REG_IVOR34  = (REG_SPR0    + SPRN_IVOR34  );
    static const int REG_IVOR35  = (REG_SPR0    + SPRN_IVOR35  );
    static const int REG_L1CFG0  = (REG_SPR0    + SPRN_L1CFG0  );
    static const int REG_L1CFG1  = (REG_SPR0    + SPRN_L1CFG1  );
    static const int REG_L1CSR0  = (REG_SPR0    + SPRN_L1CSR0  );
    static const int REG_L1CSR1  = (REG_SPR0    + SPRN_L1CSR1  );
    static const int REG_MAS0    = (REG_SPR0    + SPRN_MAS0    );
    static const int REG_MAS1    = (REG_SPR0    + SPRN_MAS1    );
    static const int REG_MAS2    = (REG_SPR0    + SPRN_MAS2    );
    static const int REG_MAS3    = (REG_SPR0    + SPRN_MAS3    );
    static const int REG_MAS4    = (REG_SPR0    + SPRN_MAS4    );
    static const int REG_MAS5    = (REG_SPR0    + SPRN_MAS5    );
    static const int REG_MAS6    = (REG_SPR0    + SPRN_MAS6    );
    static const int REG_MAS7    = (REG_SPR0    + SPRN_MAS7    );
    static const int REG_MCAR    = (REG_SPR0    + SPRN_MCAR    );
    static const int REG_MCSR    = (REG_SPR0    + SPRN_MCSR    );
    static const int REG_MCSRR0  = (REG_SPR0    + SPRN_MCSRR0  );
    static const int REG_MCSRR1  = (REG_SPR0    + SPRN_MCSRR1  );
    static const int REG_MMUCFG  = (REG_SPR0    + SPRN_MMUCFG  );
    static const int REG_MMUCSR0 = (REG_SPR0    + SPRN_MMUCSR0 );
    static const int REG_PID0    = (REG_SPR0    + SPRN_PID0    );
    static const int REG_PID1    = (REG_SPR0    + SPRN_PID1    );
    static const int REG_PID2    = (REG_SPR0    + SPRN_PID2    );
    static const int REG_SPEFSCR = (REG_SPR0    + SPRN_SPEFSCR );
    static const int REG_SVR     = (REG_SPR0    + SPRN_SVR     );
    static const int REG_TLB0CFG = (REG_SPR0    + SPRN_TLB0CFG );
    static const int REG_TLB1CFG = (REG_SPR0    + SPRN_TLB1CFG ); 

    // PMRS
    static const int REG_PMR0     = (3000                       );
    static const int REG_PMC0     = (REG_PMR0    + PMRN_PMC0    );
    static const int REG_PMC1     = (REG_PMR0    + PMRN_PMC1    );
    static const int REG_PMC2     = (REG_PMR0    + PMRN_PMC2    );
    static const int REG_PMC3     = (REG_PMR0    + PMRN_PMC3    );
    static const int REG_PMLCA0   = (REG_PMR0    + PMRN_PMLCA0  );
    static const int REG_PMLCA1   = (REG_PMR0    + PMRN_PMLCA1  );
    static const int REG_PMLCA2   = (REG_PMR0    + PMRN_PMLCA2  );
    static const int REG_PMLCA3   = (REG_PMR0    + PMRN_PMLCA3  );
    static const int REG_PMLCB0   = (REG_PMR0    + PMRN_PMLCB0  );
    static const int REG_PMLCB1   = (REG_PMR0    + PMRN_PMLCB1  );
    static const int REG_PMLCB2   = (REG_PMR0    + PMRN_PMLCB2  );
    static const int REG_PMLCB3   = (REG_PMR0    + PMRN_PMLCB3  );
    static const int REG_PMGC0    = (REG_PMR0    + PMRN_PMGC0   );
    static const int REG_UPMC0    = (REG_PMR0    + PMRN_UPMC0   );
    static const int REG_UPMC1    = (REG_PMR0    + PMRN_UPMC1   );
    static const int REG_UPMC2    = (REG_PMR0    + PMRN_UPMC2   );
    static const int REG_UPMC3    = (REG_PMR0    + PMRN_UPMC3   );
    static const int REG_UPMLCA0  = (REG_PMR0    + PMRN_UPMLCA0 );
    static const int REG_UPMLCA1  = (REG_PMR0    + PMRN_UPMLCA1 );
    static const int REG_UPMLCA2  = (REG_PMR0    + PMRN_UPMLCA2 );
    static const int REG_UPMLCA3  = (REG_PMR0    + PMRN_UPMLCA3 );
    static const int REG_UPMLCB0  = (REG_PMR0    + PMRN_UPMLCB0 );
    static const int REG_UPMLCB1  = (REG_PMR0    + PMRN_UPMLCB1 );
    static const int REG_UPMLCB2  = (REG_PMR0    + PMRN_UPMLCB2 );
    static const int REG_UPMLCB3  = (REG_PMR0    + PMRN_UPMLCB3 );
    static const int REG_UPMGC0   = (REG_PMR0    + PMRN_UPMGC0  );

}

#endif
