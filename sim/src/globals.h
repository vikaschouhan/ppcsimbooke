#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "config.h"
#include "cpu_ppc_regs.h"            // PPC register table
#include "utils.h"

//------------------------------------ instruction call frame ---------------------------------------------

// instruction call frame
#define N_IC_ARGS 6           // Max arguments supported
struct instr_call {
    struct instr_arg {
        size_t  v;         // Actual argument
        size_t  p;         // Indirect Pointer to the m_ireghash entry
        bool    t;         // Argument type ( 1=register, 0=value )
    };
    std::string   opcname;              // Opcode name
    std::string   fmt;                  // Display format
    uint64_t      opc;                  // opcode ( First 32 bit is opcode and next 32 bit is the entry no in opcode table )
    int           nargs;                // Number of arguments
    instr_arg     arg[N_IC_ARGS];       // Argument array
    void          *fptr;                // Function pointer

    instr_call();
    // Dump state
    void dump_state();
    // print instruction
    void print_instr();
    // Get instruction representation in string form ( Used for DEBUG logs )
    char* get_instr_str();
    // Get function for boost::python. We don't really need these in our c++ library
    template <int ARG_NUM> instr_arg& getarg(){
        return arg[ARG_NUM];
    }
};

inline std::ostream& operator<<(std::ostream& ostr,  instr_call &i){
    ostr << i.get_instr_str() << " ";
    return ostr;
}

//------------------ register file definitions --------------------------------------------
//
// powerPC register attribute flags
// if attr=0, this means the register is illegal
#define REG_READ_SUP        0x00000001UL                         // Register has read access in supervisor mode
#define REG_WRITE_SUP       0x00000002UL                         // Register has write access in supervisor mode
#define REG_READ_USR        0x00000004UL                         // Register has read access in user mode
#define REG_WRITE_USR       0x00000008UL                         // Register has write access in user mode
#define REG_CLEAR_W1C       0x00000010UL                         // Register can be cleared by writing 1 to the bits
#define REG_REQ_SYNC        0x00000020UL                         // Register write requires synchronization


// Some constants
#define    REG_TYPE_INV     0x0
#define    REG_TYPE_MSR     0x1
#define    REG_TYPE_CR      0x2
#define    REG_TYPE_ACC     0x3
#define    REG_TYPE_GPR     0x4
#define    REG_TYPE_SPR     0x5
#define    REG_TYPE_PMR     0x6

#define    PPC_NGPRS        32
#define    PPC_NSPRS        1024
#define    PPC_NPMRS        1024

// PPC register (64 bit only)
struct ppc_reg64 {
    union u_reg64 {
        int32_t          s32v[2];     // pair of signed 32 bit values
        uint32_t         u32v[2];     // pair of unsigned 32 bit values
        int64_t          s64v;        // singned 64bit value
        uint64_t         u64v;        // unsigned 64bit value

        u_reg64(uint64_t v): u64v(v) {}
    } value;

    const uint64_t   fvalue;    // fvalue is used in case register is read only
    const uint64_t   attr;      // attribute ( permissions etc. )
    const int        indx;      // register index no
    const int        type;      // register type

    // Constructors
    ppc_reg64(uint64_t val, uint64_t attr_, int regno, int type_):
        value(val),
        fvalue(val),
        attr(attr_),
        indx(regno),
        type(type_)
    {}

    // value.u64v's getter/setter for boost::python
    uint64_t __get_v()       { return value.u64v; }
    void __set_v(uint64_t v) { value.u64v = v;    }

    // Getter/Setter functions for bit fields. (Mainly for boost::python)
    uint64_t get_bf(uint64_t mask){
        return ((value.u64v & mask) >> rshift<uint64_t>(mask));
    }
    void set_bf(uint64_t bf, uint64_t mask){
        value.u64v &= ~mask;
        value.u64v |= ((bf << rshift<uint64_t>(mask)) & mask);
    }

    // refreshes value to fixed value
    void refresh_fval(){ value.u64v = fvalue; }

    // This data type can't be copied
    ppc_reg64& operator=(const ppc_reg64& rreg){ return *this; }

    // Allow integers to be directly assigned
    ppc_reg64& operator=(uint64_t v){ value.u64v = v; return *this; }

    // Overload some of operators
    ppc_reg64& operator|=(uint64_t v)          { value.u64v |= v; return *this; }
    ppc_reg64& operator&=(uint64_t v)          { value.u64v &= v; return *this; }
    ppc_reg64& operator>>=(uint64_t v)         { value.u64v >>= v; return *this; }
    ppc_reg64& operator<<=(uint64_t v)         { value.u64v <<= v; return *this; }
    uint64_t   operator>>(uint64_t v)          { return value.u64v >> v; }
    uint64_t   operator<<(uint64_t v)          { return value.u64v << v; }
    uint64_t   operator|(uint64_t v)           { return value.u64v | v; }
    uint64_t   operator&(uint64_t v)           { return value.u64v & v; }
};


// BookE (e500v2 compliant) PPC register file (this is the file we are gonna use in our cpu)
struct ppc_regs {
    // mode
    bool              cm;

    ppc_reg64         msr;
    ppc_reg64         cr;
    ppc_reg64         acc;

    // GPRs
    ppc_reg64         gpr[32];

    // SPRs
    ppc_reg64         atbl;
    ppc_reg64         atbu;
    ppc_reg64         csrr0;
    ppc_reg64         csrr1;
    ppc_reg64         ctr;
    ppc_reg64         dac1;
    ppc_reg64         dac2;
    ppc_reg64         dbcr0;
    ppc_reg64         dbcr1;
    ppc_reg64         dbcr2;
    ppc_reg64         dbsr;
    ppc_reg64         dear;
    ppc_reg64         dec;
    ppc_reg64         decar;
    ppc_reg64         esr;
    ppc_reg64         iac1;
    ppc_reg64         iac2;
    ppc_reg64         ivor0;
    ppc_reg64         ivor1;
    ppc_reg64         ivor2;
    ppc_reg64         ivor3;
    ppc_reg64         ivor4;
    ppc_reg64         ivor5;
    ppc_reg64         ivor6;
    ppc_reg64         ivor7;
    ppc_reg64         ivor8;
    ppc_reg64         ivor9;
    ppc_reg64         ivor10;
    ppc_reg64         ivor11;
    ppc_reg64         ivor12;
    ppc_reg64         ivor13;
    ppc_reg64         ivor14;
    ppc_reg64         ivor15;
    ppc_reg64         ivpr;
    ppc_reg64         lr;
    ppc_reg64         pid0;
    ppc_reg64         pid1;
    ppc_reg64         pid2;
    ppc_reg64         pir;
    ppc_reg64         pvr;
    ppc_reg64         sprg0;
    ppc_reg64         sprg1;
    ppc_reg64         sprg2;
    ppc_reg64         sprg3;
    ppc_reg64         sprg4;
    ppc_reg64         sprg5;
    ppc_reg64         sprg6;
    ppc_reg64         sprg7;
    ppc_reg64         srr0;
    ppc_reg64         srr1;
    ppc_reg64         tbl;
    ppc_reg64         tbu;
    ppc_reg64         tcr;
    ppc_reg64         tsr;
    ppc_reg64         usprg0;
    ppc_reg64         xer;

    ppc_reg64         bbear;
    ppc_reg64         bbtar;
    ppc_reg64         bucsr;
    ppc_reg64         hid0;
    ppc_reg64         hid1;
    ppc_reg64         ivor32;
    ppc_reg64         ivor33;
    ppc_reg64         ivor34;
    ppc_reg64         ivor35;
    ppc_reg64         l1cfg0; 
    ppc_reg64         l1cfg1;
    ppc_reg64         l1csr0;
    ppc_reg64         l1csr1;
    ppc_reg64         mas0;
    ppc_reg64         mas1;
    ppc_reg64         mas2;
    ppc_reg64         mas3;
    ppc_reg64         mas4;
    ppc_reg64         mas5;
    ppc_reg64         mas6;
    ppc_reg64         mas7;
    ppc_reg64         mcar;
    ppc_reg64         mcsr;
    ppc_reg64         mcsrr0;
    ppc_reg64         mcsrr1;
    ppc_reg64         mmucfg;
    ppc_reg64         mmucsr0;
    ppc_reg64         spefscr;
    ppc_reg64         svr;
    ppc_reg64         tlb0cfg;
    ppc_reg64         tlb1cfg;

    // PMRs
    ppc_reg64         pmgc0;
    ppc_reg64         pmlca0;
    ppc_reg64         pmlca1;
    ppc_reg64         pmlca2;
    ppc_reg64         pmlca3;
    ppc_reg64         pmlcb0;
    ppc_reg64         pmlcb1;
    ppc_reg64         pmlcb2;
    ppc_reg64         pmlcb3;
    ppc_reg64         pmc0;
    ppc_reg64         pmc1;
    ppc_reg64         pmc2;
    ppc_reg64         pmc3;

    ppc_reg64         upmgc0;
    ppc_reg64         upmlca0;
    ppc_reg64         upmlca1;
    ppc_reg64         upmlca2;
    ppc_reg64         upmlca3;
    ppc_reg64         upmlcb0;
    ppc_reg64         upmlcb1;
    ppc_reg64         upmlcb2;
    ppc_reg64         upmlcb3;
    ppc_reg64         upmc0;
    ppc_reg64         upmc1;
    ppc_reg64         upmc2;
    ppc_reg64         upmc3;

    // register ptrs
    std::map<std::string, ppc_reg64*>  m_reg;
    std::map<int,         ppc_reg64*>  m_ireg;

    // Constructors
    ppc_regs(bool c_m=0);
    
    // functions operating on CR, XER
    void       update_cr0(uint64_t value=0);                     // Update CR0
    void       update_cr0_host(x86_flags &hf);                   // Update CR0 using host flags

    // Used for Condition register operations
    void       update_crF(unsigned bf, unsigned val);            // Updates CR[bf] with val i.e CR[bf] <- (val & 0xf)
    unsigned   get_crF(unsigned bf);                             // Get crF  (F -> [0:7])
    void       update_crf(unsigned field, bool value);           // Update CR by exact field value [0:31]
    bool       get_crf(unsigned field);                          // Get CR bit at exact field

    // XER operations
    void       update_xerF(unsigned bf, unsigned val);           // Updates XER[bf] with val i.e XER[bf] <- (val & 0xf)
    unsigned   get_xerF(unsigned bf);                            // Get xerF (F -> [0:7])
    void       update_xerf(unsigned field, bool value);          // Update XER by exact field value [0:31]
    bool       get_xerf(unsigned field);                         // Get XER bit at exact field

    void       update_xer_so_ov(uint8_t so_ov);                  // Update SO & OV
    void       update_xer_so_ov_host(x86_flags &hf);             // Update SO & OV using host flags
    void       update_xer_ca(bool value=0);                      // Update XER[CA]
    void       update_xer_ca_host(x86_flags &hf);                // Update XER[CA] using host flags

    bool       get_xer_so();                                     // Get XER[SO]
    bool       get_xer_ca();                                     // Get XER[CA]
    bool       get_xer_ov();                                     // Get XER[OV]

    // functions operating on SEPFSCR
    void       update_spefscr(x86_mxcsr& hf, bool high);

    // for boost::python
    template<int x>
    ppc_reg64& get_gpr(){
        return gpr[x];
    }
 
};

#endif
