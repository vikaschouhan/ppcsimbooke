#include "processor.h"

.global .bootpage
.section .bootpage, "ax"
.align 12
cpu_continue:
    # MMU initialization
    # [0x0000_0000:0x3fff_ffff] -> [0x0000_0000:0x3fff_ffff]
    lis r0,    0x1002;   # MAS0 <- 0x1002_0000
    ori r0,r0, 0x0000;
    lis r1,    0xc000;   # MAS1 <- 0xc000_0a00
    ori r1,r1, 0x0a00;
    lis r2,    0x0000;   # MAS2 <- 0x0000_0004
    ori r2,r2, 0x0004;
    lis r3,    0x0000;   # MAS3 <- 0x0000_0015
    ori r3,r3, 0x0015;
    mtspr SPRN_MAS0, r0;
    mtspr SPRN_MAS1, r1;
    mtspr SPRN_MAS2, r2;
    mtspr SPRN_MAS3, r3;
    tlbwe;
    msync;
    isync;
   
    # [0x4000_0000:0x7fff_fff] -> [0x4000_0000:0x7fff_ffff]
    lis r0,    0x1003;   # MAS0 <- 0x1003_0000
    ori r0,r0, 0x0000;
    lis r1,    0xc000;   # MAS1 <- 0xc000_0a00
    ori r1,r1, 0x0a00;
    lis r2,    0x4000;   # MAS2 <- 0x4000_0004
    ori r2,r2, 0x0004;
    lis r3,    0x4000;   # MAS3 <- 0x4000_0015
    ori r3,r3, 0x0015;
    mtspr SPRN_MAS0, r0;
    mtspr SPRN_MAS1, r1;
    mtspr SPRN_MAS2, r2;
    mtspr SPRN_MAS3, r3;
    tlbwe;
    msync;
    isync;

    # TODO: We haven't yet implemented any CCSR, this is for future
    # [0xff70_0000:0xff7f_ffff] -> [0xff70_0000:0xff7f_ffff]
    lis r0,    0x1004;   # MAS0 <- 0x1004_0000
    ori r0,r0, 0x0000;
    lis r1,    0xc000;   # MAS1 <- 0xc000_0500
    ori r1,r1, 0x0500;
    lis r2,    0xff70;   # MAS2 <- 0xff70_0008
    ori r2,r2, 0x0008;
    lis r3,    0xff70;   # MAS3 <- 0xff70_000f
    ori r3,r3, 0x000f;
    mtspr SPRN_MAS0, r0;
    mtspr SPRN_MAS1, r1;
    mtspr SPRN_MAS2, r2;
    mtspr SPRN_MAS3, r3;
    tlbwe;
    msync;
    isync;

    # TODO: Set up LAW windows. Do misc configuration.
    # TODO: Initialize all caches

    # Assign stack pointer
    lis r1,   __stack_start@h;
    ori r1,r1,__stack_start@l;

    # jump to nomain
    lis r0,    main@h;
    ori r0,r0, main@l;
    mtspr SPRN_LR, r0;
    blrl;
0:  b 0b;   

.org ((.+ 0xfff) & ~0xfff) - 4
__start : .global __start
   b cpu_continue; 
