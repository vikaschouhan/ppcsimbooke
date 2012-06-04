#ifndef _REGFIELDS_H_
#define _REGFIELDS_H_

// BP -> Bit position ( from MSB to LSB )
// SHIFT -> shift from right ( REG_SIZE - BP - 1)

/* XER */
#define XER_SO_BP    (0)
#define XER_OV_BP    (1)
#define XER_CA_BP    (2)
#define XER_CA       (1)
#define XER_OV       (1)
#define XER_SO       (1)
#define XER_CA_SHIFT (29)
#define XER_OV_SHIFT (30)
#define XER_SO_SHIFT (31)

/* MSR bit positions (counted from MSB=0 in 32 bits)*/
/*
 * MSR_UCLE, MSR_SPE etc are already defined by regs.h in thirdpary files from Linux.
 */
#define MSR_GS_BP               (3)
#define MSR_GS_SHIFT            (31 - MSR_GS_BP)

#define MSR_UCLE_BP             (5)
#define MSR_UCLE_SHIFT          (31 - MSR_UCLE_BP)

#define MSR_SPE_BP              (6)
#define MSR_SPE_SHIFT           (31 - MSR_SPE_BP)

#define MSR_WE_BP               (13)
#define MSR_WE_SHIFT            (31 - MSR_WE_BP)

#define MSR_CE_BP               (14)
#define MSR_CE_SHIFT            (31 - MSR_CE_BP)

#define MSR_EE_BP               (16)
#define MSR_EE_SHIFT            (31 - MSR_EE_BP)

#define MSR_PR_BP               (17)
#define MSR_PR_SHIFT            (31 - MSR_PR_BP)

#define MSR_FP_BP               (18)
#define MSR_FP_SHIFT            (31 - MSR_FP_BP)

#define MSR_ME_BP               (19)
#define MSR_ME_SHIFT            (31 - MSR_ME_BP)

#define MSR_FE0_BP              (20)
#define MSR_FE0_SHIFT           (31 - MSR_FE0_BP)

#define MSR_UBLE_BP             (21)
#define MSR_UBLE_SHIFT          (31 - MSR_UBLE_BP)

#define MSR_DE_BP               (22)
#define MSR_DE_SHIFT            (31 - MSR_DE_BP)

#define MSR_FE1_BP              (23)
#define MSR_FE1_SHIFT           (31 - MSR_FE1_BP)

#define MSR_IS_BP               (26)
#define MSR_IS_SHIFT            (31 - MSR_IS_BP)

#define MSR_DS_BP               (27)
#define MSR_DS_SHIFT            (31 - MSR_DS_BP)

#define MSR_PMM_BP              (29)
#define MSR_PMM_SHIFT           (31 - MSR_PMM_SHIFT)

#define MSR_RI_BP               (30)
#define MSR_RI_SHIFT            (31 - MSR_RI_SHIFT)

#endif
