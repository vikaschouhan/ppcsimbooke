#ifndef    _CPU_HPP_
#define    _CPU_HPP_

#include "config.h"

using std::cerr;
using std::string;
using std::ostringstream;

/*
 *  The generic CPU class:
 *  NOTE: This is an abstract class i.e objects can't be defined for this class type.
 *        You want to derive it first.
 */

class cpu {

    string            name;                          /* Cpu specific name */
    string            path;                          /*  Full "path" to the CPU, e.g. "machine[0].cpu[0]":  */
    struct timeval    starttime;
    uint8_t           byte_order;
    uint8_t           is_32bit;                      /*  0 for emulated 64-bit CPUs, 1 for 32-bit.  */
    
    uint8_t           running;                       /*  1 while running, 0 when paused/stopped.  */
    /* book keeping */
    uint64_t          cpu_id;                        /*  0-based CPU id, in an emulated SMP system.  */
    uint8_t           cpu_no;                        /*  numerical cpu no */
    static size_t     ncpus;                         /*  Current number of cpus */

    protected:
    uint64_t          pc;                            /*  The program counter. (For 32-bit modes, not all bits are used.)  */
    uint64_t          nip;                           /*  Next Instruction pointer */ 
    uint8_t           bits;                          /*  32 -> 32 bits, 64 -> 64 bits */
    size_t            ninstrs;                       /*  Nr of instructions executed, etc.:  */
    bool              emul_full;                     /*  1 if full emulation, 0 if partial */

    public:
    virtual void      run() = 0;                           // run
    virtual void      step(size_t instr_cnt=1) = 0;        // step
    virtual void      run_instr(std::string instr) = 0;    // run a specific instr

    public:
    /* Constructor */
    cpu(){}
    cpu(uint64_t cpuid, string name, uint64_t pc=0) {
        init_cpu(cpuid, name, pc); 
    }
    /* Destructor */
    virtual ~cpu() {
        this->ncpus--;
    }

    void init_cpu(uint64_t cpuid, std::string name, uint64_t pc=0){
        
        if (name.empty()) {
             cerr << "cpu(): cpu name = NULL?" << std::endl;
             exit(1);
        }

        this->name = name;
        this->path = static_cast<ostringstream *>(&(ostringstream() << "cpu[" << cpuid << "]"))->str();

        this->cpu_id     = cpuid;
        this->cpu_no = this->ncpus++;
        this->pc = pc;
        this->ninstrs = 0;
        /* we are keeing this as don't care for time being.
         * will be changed in future
         */
        //this->byte_order = EMUL_UNDEFINED_ENDIAN;
        this->byte_order = 1;
        this->running    = 0; 
    }

    // Get number of instrs
    size_t get_ninstrs(){
        return ninstrs;
    }
    uint64_t get_pc(){
        return pc;
    }

    public:
    void cpu_show_cycles(int forced){ }
    

};

/* Total number of cpus */
size_t cpu::ncpus = 0;

#define    JUST_MARK_AS_NON_WRITABLE    1
#define    INVALIDATE_ALL            2
#define    INVALIDATE_PADDR        4
#define    INVALIDATE_VADDR        8
#define    INVALIDATE_VADDR_UPPER4        16    /*  useful for PPC emulation  */


#endif    /*  CPU_H  */
