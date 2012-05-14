#ifndef _CPU_HOST_H_
#define _CPU_HOST_H_

/*
 * This contains bare minimum
 * of host cpu's state
 */
/*
 * Cpu state for x86 cpu
 */
struct x86_cpu_state {
    uint8_t bits;
    uint32_t flags;
    uint32_t dummy;

    public:
    x86_cpu_state():bits(32), flags(0){}
    ~x86_cpu_state(){}
};

/*
 * Cpu state for x86_64
 */
struct x64_cpu_state {
    uint8_t bits;
    uint64_t flags;
    uint64_t dummy;

    public:
    x64_cpu_state():bits(64), flags(0){}
    ~x64_cpu_state(){}
};

/*
 * x86/x86_64 flags bitfields
 */
#define X86_FLAGS_CF     (1 << 0)    // Carry flag
#define X86_FLAGS_AF     (1 << 4)    // Auxiliary flag
#define X86_FLAGS_ZF     (1 << 6)    // zero flag
#define X86_FLAGS_SF     (1 << 7)    // sign flag
#define X86_FLAGS_OF     (1 << 11)   // overflow flag

#endif
