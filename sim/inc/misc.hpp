#ifndef _MISC_HPP
#define _MISC_HPP

// Misc macros ( specific to simulator ) library
#define    EMUL_UNDEFINED_ENDIAN    0
#define    EMUL_LITTLE_ENDIAN       1
#define    EMUL_BIG_ENDIAN          2

// instruction call frame
#define N_IC_ARGS 6
struct instr_call {
    std::string opcode;
    size_t arg[N_IC_ARGS];

    void dump_state(){
        std::cout << "opcode : " << opcode << std::endl;
        std::cout << "args   : ";
        for(int i=0; i<N_IC_ARGS; i++){
            std::cout << std::hex << std::showbase << arg[i] << " ";
        }
        std::cout << std::endl;
    }
    // Get/Set functions for boost::python. We don't really need these in our c++ library
    // We are creating seperate sets of funtions for each arg cnt, until we find a better way to do things
#define INSTR_CALL_SETARG(ARG_NUM)    \
    void setarg##ARG_NUM(size_t val)  \
    {                                 \
        arg[ARG_NUM] = val;           \
    }
#define INSTR_CALL_GETARG(ARG_NUM)    \
    size_t getarg##ARG_NUM()          \
    {                                 \
        return arg[ARG_NUM];          \
    }

    // Define functions
    INSTR_CALL_SETARG(0)  INSTR_CALL_GETARG(0)
    INSTR_CALL_SETARG(1)  INSTR_CALL_GETARG(1)
    INSTR_CALL_SETARG(2)  INSTR_CALL_GETARG(2)
    INSTR_CALL_SETARG(3)  INSTR_CALL_GETARG(3)
    INSTR_CALL_SETARG(4)  INSTR_CALL_GETARG(4)
    INSTR_CALL_SETARG(5)  INSTR_CALL_GETARG(5)

};

#endif
