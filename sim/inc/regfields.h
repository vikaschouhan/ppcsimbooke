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
#define DBSR_UDE           0x40000000UL

#endif
