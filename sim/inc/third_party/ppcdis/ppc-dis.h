#ifndef _PPC_DIS_H
#define _PPC_DIS_H

// Some structs for various ppc dialects
// Copied from ppc-dis.c in ppc-dis project
//
// NOTE:
// Changes for ppcsimbooke :-
//   1. Changed type of ppc_opts from "struct" to "static const struct"
//   2. Changed name of "dis_private" instance from "private" to "private_data" ( since "private"
//      is a reserved keyword in c++ ).
//   3. Changed type of private_data to static.

struct dis_private
{
  /* Stash the result of parsing disassembler_options here.  */
  ppc_cpu_t dialect;
};
static dis_private private_data;

#define POWERPC_DIALECT(INFO) \
  (((struct dis_private *) ((INFO)->private_data))->dialect)

struct ppc_mopt {
  const char *opt;
  ppc_cpu_t cpu;
  ppc_cpu_t sticky;
};

static const struct ppc_mopt ppc_opts[] = {
  { "403",     (PPC_OPCODE_PPC | PPC_OPCODE_403),
    0 },
  { "405",     (PPC_OPCODE_PPC | PPC_OPCODE_403 | PPC_OPCODE_405),
    0 },
  { "440",     (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_440
		| PPC_OPCODE_ISEL | PPC_OPCODE_RFMCI),
    0 },
  { "464",     (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_440
		| PPC_OPCODE_ISEL | PPC_OPCODE_RFMCI),
    0 },
  { "476",     (PPC_OPCODE_PPC | PPC_OPCODE_ISEL | PPC_OPCODE_440
		| PPC_OPCODE_476 | PPC_OPCODE_POWER4 | PPC_OPCODE_POWER5),
    0 },
  { "601",     (PPC_OPCODE_PPC | PPC_OPCODE_601),
    0 },
  { "603",     (PPC_OPCODE_PPC),
    0 },
  { "604",     (PPC_OPCODE_PPC),
    0 },
  { "620",     (PPC_OPCODE_PPC | PPC_OPCODE_64),
    0 },
  { "7400",    (PPC_OPCODE_PPC | PPC_OPCODE_ALTIVEC),
    0 },
  { "7410",    (PPC_OPCODE_PPC | PPC_OPCODE_ALTIVEC),
    0 },
  { "7450",    (PPC_OPCODE_PPC | PPC_OPCODE_ALTIVEC),
    0 },
  { "7455",    (PPC_OPCODE_PPC | PPC_OPCODE_ALTIVEC),
    0 },
  { "750cl",   (PPC_OPCODE_PPC | PPC_OPCODE_PPCPS)
    , 0 },
  { "a2",      (PPC_OPCODE_PPC | PPC_OPCODE_ISEL | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5 | PPC_OPCODE_CACHELCK | PPC_OPCODE_64
		| PPC_OPCODE_A2),
    0 },
  { "altivec", (PPC_OPCODE_PPC),
    PPC_OPCODE_ALTIVEC },
  { "any",     0,
    PPC_OPCODE_ANY },
  { "booke",   (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE),
    0 },
  { "booke32", (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE),
    0 },
  { "cell",    (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_CELL | PPC_OPCODE_ALTIVEC),
    0 },
  { "com",     (PPC_OPCODE_COMMON),
    0 },
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
  { "efs",     (PPC_OPCODE_PPC | PPC_OPCODE_EFS),
    0 },
  { "power4",  (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4),
    0 },
  { "power5",  (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5),
    0 },
  { "power6",  (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5 | PPC_OPCODE_POWER6 | PPC_OPCODE_ALTIVEC),
    0 },
  { "power7",  (PPC_OPCODE_PPC | PPC_OPCODE_ISEL | PPC_OPCODE_64
		| PPC_OPCODE_POWER4 | PPC_OPCODE_POWER5 | PPC_OPCODE_POWER6
		| PPC_OPCODE_POWER7 | PPC_OPCODE_ALTIVEC | PPC_OPCODE_VSX),
    0 },
  { "ppc",     (PPC_OPCODE_PPC),
    0 },
  { "ppc32",   (PPC_OPCODE_PPC),
    0 },
  { "ppc64",   (PPC_OPCODE_PPC | PPC_OPCODE_64),
    0 },
  { "ppc64bridge", (PPC_OPCODE_PPC | PPC_OPCODE_64_BRIDGE),
    0 },
  { "ppcps",   (PPC_OPCODE_PPC | PPC_OPCODE_PPCPS),
    0 },
  { "pwr",     (PPC_OPCODE_POWER),
    0 },
  { "pwr2",    (PPC_OPCODE_POWER | PPC_OPCODE_POWER2),
    0 },
  { "pwr4",    (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4),
    0 },
  { "pwr5",    (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5),
    0 },
  { "pwr5x",   (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5),
    0 },
  { "pwr6",    (PPC_OPCODE_PPC | PPC_OPCODE_64 | PPC_OPCODE_POWER4
		| PPC_OPCODE_POWER5 | PPC_OPCODE_POWER6 | PPC_OPCODE_ALTIVEC),
    0 },
  { "pwr7",    (PPC_OPCODE_PPC | PPC_OPCODE_ISEL | PPC_OPCODE_64
		| PPC_OPCODE_POWER4 | PPC_OPCODE_POWER5 | PPC_OPCODE_POWER6
		| PPC_OPCODE_POWER7 | PPC_OPCODE_ALTIVEC | PPC_OPCODE_VSX),
    0 },
  { "pwrx",    (PPC_OPCODE_POWER | PPC_OPCODE_POWER2),
    0 },
  { "spe",     (PPC_OPCODE_PPC | PPC_OPCODE_EFS),
    PPC_OPCODE_SPE },
  { "titan",   (PPC_OPCODE_PPC | PPC_OPCODE_BOOKE | PPC_OPCODE_PMR
		| PPC_OPCODE_RFMCI | PPC_OPCODE_TITAN),
    0 },
  { "vsx",     (PPC_OPCODE_PPC),
    PPC_OPCODE_VSX },
};


#endif
