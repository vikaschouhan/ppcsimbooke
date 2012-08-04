#include "processor.h"

#define CLEAR_GPR(n) xor r##n, r##n, r##n 

#define CLEAR_ALL_GPRS  \
    CLEAR_GPR(0);       \
    CLEAR_GPR(1);       \
    CLEAR_GPR(2);       \
    CLEAR_GPR(3);       \
    CLEAR_GPR(4);       \
    CLEAR_GPR(5);       \
    CLEAR_GPR(6);       \
    CLEAR_GPR(7);       \
    CLEAR_GPR(8);       \
    CLEAR_GPR(9);       \
    CLEAR_GPR(10);      \
    CLEAR_GPR(11);      \
    CLEAR_GPR(12);      \
    CLEAR_GPR(13);      \
    CLEAR_GPR(14);      \
    CLEAR_GPR(15);      \
    CLEAR_GPR(16);      \
    CLEAR_GPR(17);      \
    CLEAR_GPR(18);      \
    CLEAR_GPR(19);      \
    CLEAR_GPR(20);      \
    CLEAR_GPR(21);      \
    CLEAR_GPR(22);      \
    CLEAR_GPR(23);      \
    CLEAR_GPR(24);      \
    CLEAR_GPR(25);      \
    CLEAR_GPR(26);      \
    CLEAR_GPR(27);      \
    CLEAR_GPR(28);      \
    CLEAR_GPR(29);      \
    CLEAR_GPR(30);      \
    CLEAR_GPR(31);

.text
.align 12
cpu_continue:
    /* Clear GPRs 0 and 31 */
    CLEAR_GPR(0);
    CLEAR_GPR(31);

    /* Read PVR into r31 */
    mfspr r31, SPRN_PVR;  // Get PVR

    /* Invalidate d-Cache */
    mfspr r0, SPRN_L1CSR0;
    ori r0, r0, 0x2;
    msync;
    isync;
    mtspr SPRN_L1CSR0, r0;
    isync

    /* disable d-cache */
    li r0, 0;
    msync;
    isync;
    mtspr SPRN_L1CSR0, r0;
    isync;

    /* Invalidate i-cache */
    mfspr r0, SPRN_L1CSR1;
    ori r0, r0, 0x2;
    mtspr SPRN_L1CSR1, r0;
    isync

    /* disable i-cache */
    li r0, 0;
    mtspr SPRN_L1CSR1, r0;
    isync;

    /* clear some of the registers */
    li r0,0;
    mtspr SPRN_SRR0, r0;
    mtspr SPRN_SRR1, r0
    mtspr SPRN_CSRR0, r0;
    mtspr SPRN_CSRR1, r0;
    mtspr SPRN_MCSRR0, r0;
    mtspr SPRN_MCSRR1, r0;

    b cpu_continue;

.org ((.+ 0xfff) & ~0xfff) - 4
_start : .globl _start
   b cpu_continue; 
