#ifndef _REGFIELDS_H_
#define _REGFIELDS_H_

/* XER */
#define XER_CA             0x20000000
#define XER_OV             0x40000000
#define XER_SO             0x80000000

/* Left out MCSR bit fields not defined by Linux's header files */
#define MCSR_MCP           0x80000000UL
#define MCSR_ICPERR        0x40000000UL
#define MCSR_DCP_PERR      0x20000000UL
#define MCSR_DCPERR        0x10000000UL

/* Left out DBSR fields */
#define DBSR_IDE           0x80000000UL
#define DBSR_UDE           0x40000000UL
#define DBSR_MRR           0x30000000UL
#define DBSR_ICMP          DBSR_IC              //  These point to the names in linux header files
#define DBSR_BRT           DBSR_BT              //
#define DBSR_TRAP          DBSR_TIE             //

/* MAS0 */
#define MAS0_TLBSEL        0x10000000UL
#define MAS0_ESEL          0x000f0000UL
#define MAS0_NV            0x00000001UL

/* MAS1 */
#define MAS1_V             0x80000000UL
#define MAS1_IPROT         0x40000000UL
#define MAS1_TID           0x00ff0000UL
#define MAS1_TS            0x00001000UL
#define MAS1_TSIZE         0x00000f00UL

/* MAS2 */
#define MAS2_EPN           0xfffff000UL
#define MAS2_X0            0x00000040UL
#define MAS2_X1            0x00000020UL
#define MAS2_W             0x00000010UL
#define MAS2_I             0x00000008UL
#define MAS2_M             0x00000004UL
#define MAS2_G             0x00000002UL
#define MAS2_E             0x00000001UL
#define MAS2_X01           0x00000060UL
#define MAS2_WIMGE         0x0000001fUL

/* MAS3 */
#define MAS3_RPN           0xfffff000UL
#define MAS3_U03           0x000003c0UL
#define MAS3_PERMIS        0x0000003fUL
#define MAS3_UX            0x00000020UL
#define MAS3_SX            0x00000010UL
#define MAS3_UW            0x00000008UL
#define MAS3_SW            0x00000004UL
#define MAS3_UR            0x00000002UL
#define MAS3_SR            0x00000001UL

/* MAS4 */
#define MAS4_TLBSELD       0x30000000UL
#define MAS4_TIDSELD       0x00030000UL
#define MAS4_TSIZED        0x00000f00UL
#define MAS4_X0D           0x00000040UL
#define MAS4_X1D           0x00000020UL
#define MAS4_WD            0x00000010UL
#define MAS4_ID            0x00000008UL
#define MAS4_MD            0x00000004UL
#define MAS4_GD            0x00000002UL
#define MAS4_ED            0x00000001UL
#define MAS4_X01D          0x00000060UL
#define MAS4_WIMGED        0x0000001fUL

/* MAS6 */
#define MAS6_SPID0         0x00ff0000UL
#define MAS6_SAS           0x00000001UL

/* MAS7 */
#define MAS7_RPN           0x0000000fUL

/* PVR */
#undef  PVR_VER
#undef  PVR_REV
#define PVR_VER            0xffff0000UL
#define PVR_REV            0x0000ffffUL

/* HID0 */

#undef HID0_EMCP
#undef HID0_DOZE
#undef HID0_NAP
#undef HID0_SLEEP
#undef HID0_TBEN
#undef HID0_TBCLK
#undef HID0_EN_MAS7_UPDATE
#undef HID0_DCFA
#undef HID0_NOPTI

#define HID0_EMCP          0x80000000UL
#define HID0_DOZE          0x00800000UL
#define HID0_NAP           0x00400000UL
#define HID0_SLEEP         0x00200000UL
#define HID0_TBEN          0x00004000UL
#define HID0_TBCLK         0x00002000UL
#define HID0_EN_MAS7_UPDATE      0x80UL
#define HID0_DCFA          0x00000040UL
#define HID0_NOPTI         0x00000001UL


#endif
