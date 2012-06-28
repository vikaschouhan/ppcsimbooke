#ifndef _MISC_HPP
#define _MISC_HPP

// endianness consts
static const int  EMUL_UNDEFINED_ENDIAN  =  0;
static const int  EMUL_LITTLE_ENDIAN     =  1;
static const int  EMUL_BIG_ENDIAN        =  2;

// instruction call frame
#define N_IC_ARGS 6
struct instr_call {
    std::string opcode;
    std::string fmt;
    int nargs;
    size_t arg[N_IC_ARGS];
    void *fptr;               // Function pointer

    // Dump state
    void dump_state(){
        std::cout << "opcode : " << opcode << std::endl;
        std::cout << "args   : ";
        for(int i=0; i<N_IC_ARGS; i++){
            std::cout << std::hex << std::showbase << arg[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "nargs  : " << nargs << std::endl;
    }
    // print instruction
    void print_instr(){
        for(int i=nargs; i<N_IC_ARGS; i++){
            fmt += "%s";
        }
        fmt += "\n";
        // We use printf for printing , since we have format of arguments in a string.
        if(opcode == "")
            printf(".long 0x%lx\n", (arg[0] & 0xffffffff));
        else
            printf(fmt.c_str(), opcode.c_str(), arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
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
