#ifndef _MISC_HPP
#define _MISC_HPP

// Types of arguments
#define  PPC_ARG_GPR     1
#define  PPC_ARG_FPR     2
#define  PPC_ARG_VAL     3
#define  PPC_ARG_VR      4    // Most probably these are not valid
#define  PPC_ARG_VSR     5    //
#define  PPC_ARG_FSL     6    //
#define  PPC_ARG_FCR     7    //
#define  PPC_ARG_CR      8    //

// instruction call frame
#define N_IC_ARGS 6           // Max arguments supported
struct instr_call {
    std::string   opcode;               // Opcode
    std::string   fmt;                  // Display format
    int           nargs;                // Number of arguments
    size_t        arg[N_IC_ARGS];       // Arguments themselves
    int           targ[N_IC_ARGS];      // type of args
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

#endif
