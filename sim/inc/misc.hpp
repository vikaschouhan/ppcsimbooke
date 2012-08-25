#ifndef _MISC_HPP
#define _MISC_HPP

#include "utils.h"

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

    instr_call(){
        opcname = "";
        fmt     = "";
        opc     = 0;
        nargs   = 0;
        for(int i=0; i<N_IC_ARGS; i++){
            arg[i].v = arg[i].p = arg[i].t = 0;
        }
        fptr = NULL;
    }
    // Dump state
    void dump_state(){
        std::cout << "name   : " << opcname << std::endl;
        std::cout << "opcode : " << std::hex << std::showbase << opc << std::endl;
        std::cout << "args   : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i].v << " ";
        }
        std::cout << std::endl;
        std::cout << "pargs  : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i].p << " ";
        }
        std::cout << std::endl;
        std::cout << "targs  : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i].t << " ";
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
        if(opcname == "")
            printf(".long 0x%lx\n", (arg[0].v & 0xffffffff));
        else
            printf(lfmt.c_str(), opcname.c_str(), arg[0].v, arg[1].v, arg[2].v, arg[3].v, arg[4].v, arg[5].v);
    }
    // Get instruction representation in string form ( Used for DEBUG logs )
    char* get_instr_str(){
        static char instr_str[100];
        std::string lfmt = fmt;
        for(int i=nargs; i<N_IC_ARGS; i++){
            lfmt += "%c";
        }
        fmt += " ";
        // We use printf for printing , since we have format of arguments in a string.
        if(opcname == "")
            sprintf(instr_str, ".long 0x%lx ", (arg[0].v & 0xffffffff));
        else
            sprintf(instr_str, lfmt.c_str(), opcname.c_str(), arg[0].v, arg[1].v, arg[2].v, arg[3].v, arg[4].v, arg[5].v);
        return instr_str;
    }

    // Get function for boost::python. We don't really need these in our c++ library
    template <int ARG_NUM> instr_arg& getarg(){
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

// python signal callback to respond to any python errors( we can attach a callback with signature int xxx() {} )
struct py_signal_callback {
    typedef int (*callback_ptr)();   // Return type is "int" to return any status indicator
    static callback_ptr callback;
};
py_signal_callback::callback_ptr py_signal_callback::callback = NULL;

#endif
