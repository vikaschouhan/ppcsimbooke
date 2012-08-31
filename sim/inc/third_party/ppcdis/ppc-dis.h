#ifndef _PPC_DIS_H
#define _PPC_DIS_H

// Some structs for various ppc dialects
// Copied from ppc-dis.c in ppc-dis project
//
// NOTE:
// Changes for ppcsimbooke :-
//   1. Changed type of ppc_opts from "struct" to "static const struct"
//   2. Moved "dis_private" instance to the bottom level header with a new name.
// 
// Changes :-
//   (31 Aug, 2010) : Removed all processor families except Freescale cores

struct dis_private
{
  /* Stash the result of parsing disassembler_options here.  */
  ppc_cpu_t dialect;
};

struct ppc_mopt {
  const char *opt;
  ppc_cpu_t cpu;
  ppc_cpu_t sticky;
};

static const struct ppc_mopt ppc_opts[] = {
  { "e300",    (PPC_OPCODE_PPC | PPC_OPCODE_E300),
    0 },
  { "e500",    (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_SPE
		| PPC_OPCODE_ISEL | PPC_OPCODE_EFS | PPC_OPCODE_BRLOCK
		| PPC_OPCODE_PMR | PPC_OPCODE_CACHELCK | PPC_OPCODE_RFMCI
		| PPC_OPCODE_E500),
    0 },
  { "e500mc",  (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_ISEL
		| PPC_OPCODE_PMR | PPC_OPCODE_CACHELCK | PPC_OPCODE_RFMCI
		| PPC_OPCODE_E500MC),
    0 },
  { "e500mc64",  (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_ISEL
		| PPC_OPCODE_PMR | PPC_OPCODE_CACHELCK | PPC_OPCODE_RFMCI
		| PPC_OPCODE_E500MC | PPC_OPCODE_64 | PPC_OPCODE_POWER5
		| PPC_OPCODE_POWER6 | PPC_OPCODE_POWER7),
    0 },
  { "e5500",    (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_ISEL
		| PPC_OPCODE_PMR | PPC_OPCODE_CACHELCK | PPC_OPCODE_RFMCI
		| PPC_OPCODE_E500MC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5 | PPC_OPCODE_POWER6
		| PPC_OPCODE_POWER7),
    0 },
  { "e6500",   (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_ISEL
		| PPC_OPCODE_PMR | PPC_OPCODE_CACHELCK | PPC_OPCODE_RFMCI
		| PPC_OPCODE_E500MC | PPC_OPCODE_64 | PPC_OPCODE_ALTIVEC
		| PPC_OPCODE_ALTIVEC2 | PPC_OPCODE_E6500 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5 | PPC_OPCODE_POWER6 | PPC_OPCODE_POWER7),
    0 },
  { "e500x2",  (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_SPE
		| PPC_OPCODE_ISEL | PPC_OPCODE_EFS | PPC_OPCODE_BRLOCK
		| PPC_OPCODE_PMR | PPC_OPCODE_CACHELCK | PPC_OPCODE_RFMCI
		| PPC_OPCODE_E500),
    0 },
};


#endif
