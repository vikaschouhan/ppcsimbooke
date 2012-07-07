#ifndef _MISC_HPP
#define _MISC_HPP

#include "utils.h"

// instruction call frame
#define N_IC_ARGS 6           // Max arguments supported
struct instr_call {
    std::string   opcode;               // Opcode
    std::string   fmt;                  // Display format
    int           nargs;                // Number of arguments
    size_t        arg[N_IC_ARGS];       // Arguments themselves
    int           targ[N_IC_ARGS];      // arguments suitable for our purpose
    void          *fptr;                // Function pointer

    instr_call(){
        opcode = "";
        fmt    = "";
        nargs  = 0;
        for(int i=0; i<N_IC_ARGS; i++){ arg[i] = 0; }
    }
    // Dump state
    void dump_state(){
        std::cout << "opcode : " << opcode << std::endl;
        std::cout << "args   : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "targs  : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << targ[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "fmt    : " << fmt << std::endl;
        std::cout << "nargs  : " << nargs << std::endl << std::endl;
    }
    // print instruction
    void print_instr(){
        std::string lfmt = fmt;
        for(int i=nargs; i<N_IC_ARGS; i++){
            lfmt += "%c";
        }
        fmt += "\n";
        // We use printf for printing , since we have format of arguments in a string.
        if(opcode == "")
            printf(".long 0x%lx\n", (arg[0] & 0xffffffff));
        else
            printf(lfmt.c_str(), opcode.c_str(), arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
    }

    // Get/Set functions for boost::python. We don't really need these in our c++ library
    template <int ARG_NUM> void setarg(size_t val){
        arg[ARG_NUM] = val;
    }
    template <int ARG_NUM> size_t getarg(){
        return arg[ARG_NUM];
    }
};

// Some constants
#define    PPC_NGPRS        32
#define    PPC_NFPRS        32
#define    PPC_NVRS         32
#define    PPC_NSPRS        1024
#define    PPC_NPMRS        1024
#define    PPC_NTGPRS       4

// PPC register ( 64 bit only )
struct ppc_reg64 {
    uint64_t value;     // value
    uint64_t attr;      // attribute ( permissions etc. )

    // Constructors
    ppc_reg64(){
        value = 0;
    }
    ppc_reg64(uint64_t val) : value(val) {}

    // Getter/Setter functions for bit fields. ( Mainly for boost::python )
    uint64_t get_bf(uint64_t mask){
        return ((value & mask) >> rshift(mask));
    }
    void set_bf(uint64_t bf, uint64_t mask){
        value &= ~mask;
        value |= ((bf << rshift(mask)) & mask);
    }
};

// PPC register file ( this is the file we are gonna use in our cpu )
struct ppc_regs {
    typedef std::vector<ppc_reg64>  ppc_reg64_vector;
    ppc_reg64         cr;
    ppc_reg64         fpscr;
    ppc_reg64         msr;
    ppc_reg64         gpr[PPC_NGPRS];
    ppc_reg64         fpr[PPC_NFPRS];
    ppc_reg64         spr[PPC_NSPRS];
    ppc_reg64         pmr[PPC_NPMRS];

    // Functions for boost::python usage only
    template<int gprno> ppc_reg64& get_gpr(){
        return gpr[gprno];
    }
    template<int fprno> ppc_reg64& get_fpr(){
        return fpr[fprno];
    }
    template<int sprno> ppc_reg64& get_spr(){
        return spr[sprno];
    }
    template<int pmrno> ppc_reg64& get_pmr(){
        return pmr[pmrno];
    }
};

#endif
