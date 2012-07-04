#ifndef    _CPU_PPC_HPP_
#define    _CPU_PPC_HPP_

#include "config.h"
#include "tlb_booke.hpp"
#include "ppc_exception.hpp"
#include "cpu.hpp"
#include "cpu_ppc_regs.h"
#include "cpu_host.h"
#include "cpu_ppc_quirks.hpp"
#include "memory.hpp"
#include "third_party/ppc/reg.h"
#include "regfields.h"

/* 64 bit MSRs were used in older powerPC designs */
/* All BookE cores have 32 bit MSRs only */

/*
 * booke cpu
 * NOTE: All registers are 64 bits.
 *       ( even if some of them are 32 bits in actual hardware. Upper 32 bits are just ignored in those cases )
 */
class cpu_ppc_booke : public cpu {

    public:

    /* 
     * @func : constructor for this cpu
     * @args : cpuid and name
     *
     * @notes : Default program counter at beginning ( reset vector ) is 0xfffffffc
     *
     * @brief : inittializes a new cpu instance
     */
    // Default constructor
    cpu_ppc_booke(){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->cpu_no = this->ncpus++;
        init_reghash();
        gen_ppc_opc_func_hash(this);
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    cpu_ppc_booke(uint64_t cpuid, std::string name): cpu(cpuid, name, 0xfffffffc){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->cpu_no = this->ncpus++;
        init_reghash();
        gen_ppc_opc_func_hash(this);
        /* Add other things */
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    /*
     * @func : destructor
     */
    ~cpu_ppc_booke(){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->ncpus--;
        LOG("DEBUG4") << MSG_FUNC_END;
    } 

    // Initalize CPU ( for objects  being created by default constructor )
    void init_cpu_ppc_booke(uint64_t cpuid, std::string name){
        init_cpu(cpuid, name, 0xfffffffc);
    }

    // All virtual functions
    int run_instr(instr_call *ic);
    int xlate_v2p(uint64_t vaddr, uint64_t *return_paddr, int flags);
    // Overloaded run
    int run_instr(std::string opcode, std::string arg0="", std::string arg1="", std::string arg2="",
            std::string arg3="", std::string arg4="", std::string arg5="");

    // Get GPR value
    uint64_t get_gpr(int gprno) throw(sim_exception);
    // Get spr value
    uint64_t get_spr(int sprno) throw(sim_exception);
    // Get pmr value
    uint64_t get_pmr(int pmrno) throw (sim_exception);
    // Get fpr value
    uint64_t get_fpr(int fprno) throw(sim_exception);
    // Get msr
    uint64_t get_msr() throw();
    // Get cr
    uint64_t get_cr() throw();
    // Get fpscr
    uint64_t get_fpscr() throw();
    // Get Register by name
    uint64_t get_reg(std::string name) throw(sim_exception);
    // Dump CPU state
    void dump_state(int columns=0, std::ostream &ostr=std::cout, int dump_all_sprs=0);

    //
    // for boost::python
    // Set Register by name ( templatized version for boost::python export )
    template <int reg_num> void ___set_reg(uint64_t val){
        *m_ireghash[reg_num] = val;
    }
    // Get Register by name ( templatized version for boost::python export )
    template <int reg_num> uint64_t ___get_reg(){
        return *m_ireghash[reg_num];
    }
    bool operator==(cpu_ppc_booke const &x) const {
        return cpu_no == x.cpu_no;
    }
    bool operator!=(cpu_ppc_booke const &x) const {
        return cpu_no != x.cpu_no;
    }

    protected:
    // Update CR0
    void update_cr0(bool use_host, uint64_t value=0);
    // Updates CR[bf] with val i.e CR[bf] <- (val & 0xf)
    void update_crF(unsigned bf, uint64_t val);
    // Get crF  ( F -> [0:7] )
    unsigned get_crF(unsigned bf);
    // Update CR by exact field value [0:31]
    void update_crf(unsigned field, unsigned value);
    // Get CR bit at exact field
    unsigned get_crf(unsigned field);
     // set cr
    void set_cr(uint32_t value) throw();
    // Update XER
    void update_xer(bool use_host, uint64_t value=0);
    void update_xerF(unsigned bf, unsigned val);
    void update_xerf(unsigned field, unsigned value);
    unsigned get_xerF(unsigned bf);
    unsigned get_xerf(unsigned field);
    // Get XER[SO]
    unsigned get_xer_so();
    unsigned get_xer_ca();

    private:
    void init_reghash();
    void init_spr_attrs();
    void ppc_exception(int exception_nr, uint64_t subtype, uint64_t ea=0xffffffffffffffffULL);
    
    private: 
    // This function defines nested functions ( using struct encapsulation technique )
    // Each function implements the pseudocode for one ( exactly one ) ppc opcode.
    //
    // For eg. there is one function for "add", another for "or" and likewise
    //
    // NOTE : It's the responsibility of this function to populate ppc_func_hash and
    //        it has to be called once in the constructor.
    friend void gen_ppc_opc_func_hash(cpu_ppc_booke *cpu);

    uint64_t    of_emul_addr;

    int         mode;              /*  MODE_PPC or MODE_POWER  */
    int         bits;              /*  32 or 64  */

    int         irq_asserted;      /*  External Interrupt flag  */
    int         dec_intr_pending;  /* Decrementer interrupt pending  */
    uint64_t    zero;              /*  A zero register  */

    uint64_t    cr;                /*  Condition Register  */
    uint64_t    fpscr;             /*  FP Status and Control Register  */
    uint64_t    gpr[PPC_NGPRS];    /*  General Purpose Registers  */
    uint64_t    fpr[PPC_NFPRS];    /*  Floating-Point Registers  */

    uint64_t    vr_hi[PPC_NVRS];   /*  128-bit Vector registers  */
    uint64_t    vr_lo[PPC_NVRS];   /*  (Hi and lo 64-bit parts)  */

    uint64_t    msr;

    uint64_t    tgpr[PPC_NTGPRS]; /*Temporary gpr 0..3  */

    uint64_t    spr[PPC_NSPRS];
    uint64_t    pmr[PPC_NPMRS];

#define SPR_ACS_READ    0x00000001UL       // SPR has read access
#define SPR_ACS_WRITE   0x00000002UL       // SPR has write access
#define SPR_CLEAR_W1C   0x00000010UL       // SPR can be cleared by writing 1 to the bits
#define SPR_REQ_SYNC    0x00000020UL       // SPR write requires synchronization
#define SPR_PRIV        0x00000100UL       // SPR is priviledged
#define SPR_ILLEGAL     0xffffffffUL       // SPR is illegal
    uint32_t    spr_attr[PPC_NSPRS];

    // Book keeping
    uint8_t                               cpu_no;            /* Numerical cpu no */
    static size_t                         ncpus;

    // Pointers to generic registers/stuff hashed by name and numerical identifiers
    std::map<std::string, uint64_t*>      m_reghash;
    std::map<int, uint64_t*>              m_ireghash;

    /*
     *  Instruction translation cache and Virtual->Physical->Host
     *  address translation:
     */
    ppc_tlb_booke     l2tlb;            /* Only L2 tlb is implemented */

    // memory module
    std::shared_ptr<memory> mem_ptr;

    /* Host specific stuff */
#if HOST_ARCH == x86_64
    struct x64_cpu_state host_state;
#elif HOST_ARCH == i686
    struct x86_cpu_state host_state;
#endif

    // opcode to function pointer map
    typedef void (*ppc_opc_fun_ptr)(cpu_ppc_booke *, struct instr_call *);
    std::map<std::string, ppc_opc_fun_ptr>  ppc_func_hash;

    // Implementaion specific fixes
    static cpu_ppc_booke_quirks  quirks;

};

// --------------------------- STATIC DATA ---------------------------------------------------

// This is list of pointers to all alive cpu objects , so that we could refer them using it's class type only.
size_t                         cpu_ppc_booke::ncpus = 0; /* Current number of powerpc cpus */
// Cpu fixes for booke
cpu_ppc_booke_quirks           cpu_ppc_booke::quirks;

// --------------------------- Include external files ----------------------------------------

// Include this file ( this is supposed to be dynamically generated by some external utility )
#include  "cpu_ppc_instr.h"


// -----------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//
// --------------------------- Member function definitions -----------------------------------
//
// All virtual functions
int cpu_ppc_booke::run_instr(instr_call *ic){
    LOG("DEBUG4") << MSG_FUNC_START;
    ppc_func_hash[ic->opcode](this, ic);
    LOG("DEBUG4") << MSG_FUNC_END;
    return 0;
}

int cpu_ppc_booke::xlate_v2p(uint64_t vaddr, uint64_t *return_paddr, int flags){
    return 0;
}

// Second form of run_instr
// Seems like c++ doesn't have an very effective way of handling non POD objects
//  with variadic arg funcs
int cpu_ppc_booke::run_instr(std::string opcode, std::string arg0, std::string arg1, std::string arg2,
        std::string arg3, std::string arg4, std::string arg5){
    LOG("DEBUG4") << MSG_FUNC_START;
    instr_call call_this;
    call_this.opcode = opcode;

    // We don't have support for Vector registers 
    // NOTE: arg is of std::string type only, argno is of type int
    #define PARSE_INSTR_ARG(opc, arg_this, argno)                                                             \
        do{                                                                                                   \
            if((arg_this) == "") break;                                                                       \
            arg_this = quirks.arg_fix_zero_quirk((opc), (arg_this), (argno));                                 \
            if(((arg_this).at(0) >= 'A' && (arg_this).at(0) <= 'Z') ||                                        \
                    ((arg_this).at(0) >= 'a' && (arg_this).at(0) <= 'z')){                                    \
                /* It starts with a character, so assume it's some register only */                           \
                boost::algorithm::to_lower(arg_this);                                                         \
                assert_and_throw(m_reghash.find(arg_this) != m_reghash.end(),                                 \
                        sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal ppc register"));                        \
                call_this.arg[argno] = reinterpret_cast<size_t>(m_reghash[arg_this]);                         \
            }else{                                                                                            \
                /* Assuming it's an immediate value */                                                        \
                call_this.arg[argno] = static_cast<size_t>(str_to_int(arg_this));                             \
            }                                                                                                 \
        }while(0)
    PARSE_INSTR_ARG(opcode, arg0, 0);
    PARSE_INSTR_ARG(opcode, arg1, 1);
    PARSE_INSTR_ARG(opcode, arg2, 2);
    PARSE_INSTR_ARG(opcode, arg3, 3);
    PARSE_INSTR_ARG(opcode, arg4, 4);
    PARSE_INSTR_ARG(opcode, arg5, 5);

    // Call instr_func
    ppc_func_hash[opcode](this, &call_this);
    LOG("DEBUG4") << MSG_FUNC_END;
    return 0;
}

// Initialize SPR attributes
void cpu_ppc_booke::init_spr_attrs(){
    // spr_no[5] denotes priviledge level of SPR
    // If 1 -> the SPR is supervisor only,
    // If 0 -> it's accessible from both User and supervisor mode
#define GET_SPR_PRIV(spr_no) (((i >> 4) & 1) ? SPR_PRIV : 0) 
    int i;
    for(i=0; i<PPC_NSPRS; i++){
        spr_attr[i] = SPR_ILLEGAL;
    }

    // Set permissions for individual sprs
    spr_attr[SPRN_ATBL]     = SPR_ACS_READ;
    spr_attr[SPRN_ATBU]     = SPR_ACS_READ;
    spr_attr[SPRN_CSRR0]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_CSRR1]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_CTR]      = SPR_ACS_READ  | SPR_ACS_WRITE;
    spr_attr[SPRN_DAC1]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_DAC2]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_DBCR0]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_DBCR1]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC; 
    spr_attr[SPRN_DBCR2]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_DBSR]     = SPR_ACS_READ  | SPR_CLEAR_W1C  | SPR_PRIV;
    spr_attr[SPRN_DEAR]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_DEC]      = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_DECAR]    = SPR_ACS_WRITE | SPR_PRIV;
    spr_attr[SPRN_ESR]      = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IAC1]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IAC2]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR0]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR1]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR2]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR3]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR4]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR5]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR6]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR8]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR10]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR11]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR12]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR13]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR14]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR15]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVPR]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_LR]       = SPR_ACS_READ  | SPR_ACS_WRITE;
    spr_attr[SPRN_PID]      = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_PIR]      = SPR_ACS_READ  | SPR_PRIV;
    spr_attr[SPRN_PVR]      = SPR_ACS_READ  | SPR_PRIV;
    spr_attr[SPRN_SPRG0]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG1]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG2]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG3R]   = SPR_ACS_READ;
    spr_attr[SPRN_SPRG3]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG4R]   = SPR_ACS_READ;
    spr_attr[SPRN_SPRG4]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG5R]   = SPR_ACS_READ;
    spr_attr[SPRN_SPRG5]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG6R]   = SPR_ACS_READ;
    spr_attr[SPRN_SPRG6]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SPRG7R]   = SPR_ACS_READ;
    spr_attr[SPRN_SPRG7]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SRR0]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_SRR1]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_TBRL]     = SPR_ACS_READ;
    spr_attr[SPRN_TBWL]     = SPR_ACS_WRITE | SPR_PRIV;
    spr_attr[SPRN_TBRU]     = SPR_ACS_READ;
    spr_attr[SPRN_TBWU]     = SPR_ACS_WRITE | SPR_PRIV;
    spr_attr[SPRN_TCR]      = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_TSR]      = SPR_ACS_READ  | SPR_CLEAR_W1C  | SPR_PRIV;
    spr_attr[SPRN_USPRG0]   = SPR_ACS_READ  | SPR_ACS_WRITE;
    spr_attr[SPRN_XER]      = SPR_ACS_READ  | SPR_ACS_WRITE;

    spr_attr[SPRN_BBEAR]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_REQ_SYNC;
    spr_attr[SPRN_BBTAR]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_REQ_SYNC;
    spr_attr[SPRN_BUCSR]    = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_HID0]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_HID1]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_IVOR32]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR33]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR34]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_IVOR35]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_L1CFG0]   = SPR_ACS_READ;
    spr_attr[SPRN_L1CFG1]   = SPR_ACS_READ;
    spr_attr[SPRN_L1CSR0]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_L1CSR1]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS0]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS1]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS2]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS3]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS4]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS5]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;    // Needs to be verified
    spr_attr[SPRN_MAS6]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MAS7]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_MCAR]     = SPR_ACS_READ  | SPR_PRIV;
    spr_attr[SPRN_MCSR]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_MCSRR0]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_MCSRR1]   = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV;
    spr_attr[SPRN_MMUCFG]   = SPR_ACS_READ  | SPR_PRIV;
    spr_attr[SPRN_MMUCSR0]  = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_PID0]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_PID1]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_PID2]     = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_PRIV      | SPR_REQ_SYNC;
    spr_attr[SPRN_SPEFSCR]  = SPR_ACS_READ  | SPR_ACS_WRITE  | SPR_REQ_SYNC;
    spr_attr[SPRN_SVR]      = SPR_ACS_READ  | SPR_PRIV;
    spr_attr[SPRN_TLB0CFG]  = SPR_ACS_READ  | SPR_PRIV;
    spr_attr[SPRN_TLB1CFG]  = SPR_ACS_READ  | SPR_PRIV;


}                              

// Interrupt handling routines ( they handle exceptions at hardware level only and redirect control
//                               to appropriate ISR. ).
// @args :
//     exception_nr -> exception number
//     subtype -> a flag denoting the event which caused the exception
//     ea -> effective address at the time fault occured ( used in case of DSI faults etc )
void cpu_ppc_booke::ppc_exception(int exception_nr, uint64_t subtype=0, uint64_t ea)
{
    LOG("DEBUG4") << MSG_FUNC_START;

    uint64_t *srr0 = NULL;
    uint64_t *srr1 = NULL;

#define GET_PC_FROM_IVOR_NUM(ivor_num)                          \
    ((spr[SPRN_IVPR] & 0xffff) << 16) | ((spr[SPRN_IVOR##ivor_num] & 0xfff) << 4)
#define CLR_DEFAULT_MSR_BITS()                                  \
    msr &= ~MSR_SPE & ~MSR_WE & ~MSR_EE & ~MSR_PR & ~MSR_FP & ~MSR_FE0 & ~MSR_FE1 & ~MSR_IS & ~MSR_DS

    switch(exception_nr){
        case  PPC_EXCEPTION_CR:
            if((msr & MSR_CE) == 0){ RETURNVOID(DEBUG4); }

            spr[SPRN_CSRR0] = pc;
            spr[SPRN_CSRR1] = msr;
           
            // Clear default MSR bits. Also clear CE bit 
            CLR_DEFAULT_MSR_BITS();
            msr &= ~MSR_CE;
            pc = GET_PC_FROM_IVOR_NUM(0);
            break;
        case  PPC_EXCEPTION_MC:
            if((msr & MSR_ME) == 0){
                std::cout << "Received Machine Check and MSR[ME]=0. Going into checkstop." << std::endl;
                exit(1);
            }
            
            spr[SPRN_MCSRR0] = pc;
            spr[SPRN_MCSRR1] = msr;
            spr[SPRN_MCAR]   = ea;

            // If no cause specified, skip straight away
            if(subtype != 0ULL){

                // Check for sub types
                if((subtype & PPC_EXCEPT_MC_DCPERR) == PPC_EXCEPT_MC_DCPERR){
                    spr[SPRN_MCSR] |= MCSR_DCPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RAERR) == PPC_EXCEPT_MC_BUS_RAERR){
                    spr[SPRN_MCSR] |= MCSR_BUS_RAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RBERR) == PPC_EXCEPT_MC_BUS_RBERR){
                    spr[SPRN_MCSR] |= MCSR_BUS_RBERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RPERR) == PPC_EXCEPT_MC_BUS_RPERR){
                    spr[SPRN_MCSR] |= MCSR_BUS_RPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_WAERR) == PPC_EXCEPT_MC_BUS_WAERR){
                    spr[SPRN_MCSR] |= MCSR_BUS_WAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_WBERR) == PPC_EXCEPT_MC_BUS_WBERR){
                    spr[SPRN_MCSR] |= MCSR_BUS_WBERR;
                }
                if((subtype & PPC_EXCEPT_MC_DCP_PERR) == PPC_EXCEPT_MC_DCP_PERR){
                    spr[SPRN_MCSR] |= MCSR_DCP_PERR;
                }
                if((subtype & PPC_EXCEPT_MC_ICPERR) == PPC_EXCEPT_MC_ICPERR){
                    spr[SPRN_MCSR] |= MCSR_ICPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IAERR) == PPC_EXCEPT_MC_BUS_IAERR){
                    spr[SPRN_MCSR] |= MCSR_BUS_IAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IBERR) == PPC_EXCEPT_MC_BUS_IBERR){
                     spr[SPRN_MCSR] |= MCSR_BUS_IBERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IPERR) == PPC_EXCEPT_MC_BUS_IPERR){
                        spr[SPRN_MCSR] |= MCSR_BUS_IPERR;
                }
            }

            // Clear default MSR bits.
            CLR_DEFAULT_MSR_BITS();
            msr &= ~MSR_ME;
            pc = GET_PC_FROM_IVOR_NUM(1);
            break;
        case  PPC_EXCEPTION_DSI:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;
            spr[SPRN_DEAR] = ea;
            spr[SPRN_ESR] = 0;   // Clear ESR first

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DSI_ACS_W) == PPC_EXCEPT_DSI_ACS_W){
                    spr[SPRN_ESR] |= ESR_ST;
                }
                if((subtype & PPC_EXCEPT_DSI_CL) == PPC_EXCEPT_DSI_CL){
                    spr[SPRN_ESR] |= ESR_DLK;
                }
                if((subtype & PPC_EXCEPT_DSI_BO) == PPC_EXCEPT_DSI_BO){
                    spr[SPRN_ESR] |= ESR_BO;
                }
            }

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(2); 
            break;
        case  PPC_EXCEPTION_ISI:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;
            spr[SPRN_ESR] = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_ISI_BO) == PPC_EXCEPT_ISI_BO){
                    spr[SPRN_ESR] |= ESR_BO;
                }
                if((subtype & PPC_EXCEPT_ISI_ACS) == PPC_EXCEPT_ISI_ACS){
                    // No bit is said to be affected for this in e500 core RM
                }
            }

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(3);
            break;
        case  PPC_EXCEPTION_EI:
            if((msr & MSR_EE) == 0){ RETURNVOID(DEBUG4); }
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            CLR_DEFAULT_MSR_BITS();  // MSR[EE] is also cleared by this.
            pc = GET_PC_FROM_IVOR_NUM(4);
            break;
        case  PPC_EXCEPTION_ALIGN:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;
            spr[SPRN_DEAR] = ea;
            spr[SPRN_ESR]  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_ALIGN_SPE) == PPC_EXCEPT_ALIGN_SPE){
                    spr[SPRN_ESR] |= ESR_SPV;
                }
                if((subtype & PPC_EXCEPT_ALIGN_ST) == PPC_EXCEPT_ALIGN_ST){
                    spr[SPRN_ESR] |= ESR_ST;
                }
            }

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(5);
            break;
        case  PPC_EXCEPTION_PRG:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;
            spr[SPRN_ESR]  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_PRG_ILG) == PPC_EXCEPT_PRG_ILG){
                    spr[SPRN_ESR] |= ESR_PIL;
                }
                if((subtype & PPC_EXCEPT_PRG_PRV) == PPC_EXCEPT_PRG_PRV){
                    spr[SPRN_ESR] |= ESR_PPR;
                }
                if((subtype & PPC_EXCEPT_PRG_TRAP) == PPC_EXCEPT_PRG_TRAP){
                    spr[SPRN_ESR] |= ESR_PTR;
                }
            }
      
            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(6);
            break;
        case  PPC_EXCEPTION_FPU:
            // Is FPU there in e500v2 ??
            break;
        case  PPC_EXCEPTION_SC:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(8);
            break;
        case  PPC_EXCEPTION_DEC:
            // A decrementer intr. occurs when no higher priority exception exists ,
            // a decrementer exception exists (TSR[DIS] = 1), and the interrupt is
            // enabled (TCR[DIE] =1 and MSR[EE] = 1)
            if((spr[SPRN_TSR] & TSR_DIS) && (spr[SPRN_TCR] & TCR_DIE) && (msr & MSR_EE)){
                // Do nothing
            }else{
                RETURNVOID(DEBUG4);
            }
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            CLR_DEFAULT_MSR_BITS();
            spr[SPRN_TSR] |= TSR_DIS; // Why the hell, am I doing it ?? TSR[DIS] is already set. Isn't it ?
            pc = GET_PC_FROM_IVOR_NUM(10);
            break;
        case  PPC_EXCEPTION_FIT:
            // A fixed interval timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[FIS] = 1) and the interrupt is enabled
            // (TCR[FIE] = 1 and MSR[EE] = 1)
            if((spr[SPRN_TSR] & TSR_FIS) && (spr[SPRN_TCR] & TCR_FIE) && (msr & MSR_EE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            CLR_DEFAULT_MSR_BITS();
            spr[SPRN_TSR] |= TSR_FIS;
            pc = GET_PC_FROM_IVOR_NUM(11);
            break;
        case  PPC_EXCEPTION_WTD:
            // A watchdog timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[WIS] = 1) and the interrupt is enabled
            // (TCR[WIE] = 1 and MSR[CE] = 1)
            if((spr[SPRN_TSR] & TSR_WIS) && (spr[SPRN_TCR] & TCR_WIE) && (msr & MSR_CE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            spr[SPRN_CSRR0] = pc;
            spr[SPRN_CSRR1] = msr;

            CLR_DEFAULT_MSR_BITS();
            msr &= ~MSR_CE;               // Clear CE bit, since WDT is critical type
            spr[SPRN_TSR] |= TSR_WIS;
            pc = GET_PC_FROM_IVOR_NUM(12);
            break;
        case  PPC_EXCEPTION_DTLB:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;
            spr[SPRN_DEAR] = ea;
            spr[SPRN_ESR]  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DTLB_MISS_ST) == PPC_EXCEPT_DTLB_MISS_ST){
                    spr[SPRN_ESR] |= ESR_ST;
                }
            }

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(13);
            break;
        case  PPC_EXCEPTION_ITLB:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(14);
            break;
        case  PPC_EXCEPTION_DBG:
            // First check if DBCR0[IDM] is set then check if MSR[DE] is set
            if((spr[SPRN_DBCR0] & DBCR0_IDM) == 0){ RETURNVOID(DEBUG4); }
            if((msr & MSR_DE) == 0){ RETURNVOID(DEBUG4); }

            // Debug exceptions are of "CRITICAL" type in e500v2, but "DEBUG" type in e500mc and later
#ifdef CONFIG_E500
            srr0 = &spr[SPRN_CSRR0];
            srr1 = &spr[SPRN_CSRR1];
#else
            srr0 = &spr[SPRN_DSRR0];
            srr1 = &spr[SPRN_DSRR0];
#endif
            // FIXME:
            // Some references are ambiguous ( for eg. the e500v2 RM says that,
            // for return from interrupt (RET) debug exceptions, CSRR0 is set
            // to the address of the instruction that would have executed after
            // the rfi, rfci, or rfmci that caused the debug interrupt.
            // Not "that" instr is pc+4 or the return address ( which rfxi was supposed
            // to jump onto ), is unclear. This needs to be fixed. )

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DBG_TRAP) == PPC_EXCEPT_DBG_TRAP){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_TIE;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC1) == PPC_EXCEPT_DBG_IAC1){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_IAC1;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC2) == PPC_EXCEPT_DBG_IAC2){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_IAC2;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1R) == PPC_EXCEPT_DBG_DAC1R){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_DAC1R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1W) == PPC_EXCEPT_DBG_DAC1W){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_DAC1W;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2R) == PPC_EXCEPT_DBG_DAC2R){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_DAC2R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2W) == PPC_EXCEPT_DBG_DAC2W){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_DAC2W;
                }
                if((subtype & PPC_EXCEPT_DBG_ICMP) == PPC_EXCEPT_DBG_ICMP){
                    *srr0 = pc + 4;
                    spr[SPRN_DBSR] |= DBSR_IC;
                }
                if((subtype & PPC_EXCEPT_DBG_BRT) == PPC_EXCEPT_DBG_BRT){
                    *srr0 = pc;
                    spr[SPRN_DBSR] |= DBSR_BT;
                }
                if((subtype & PPC_EXCEPT_DBG_RET) == PPC_EXCEPT_DBG_RET){
                    *srr0 = pc + 4;
                    spr[SPRN_DBSR] |= DBSR_RET;
                }
                if((subtype & PPC_EXCEPT_DBG_IRPT) == PPC_EXCEPT_DBG_IRPT){
                    //srr0 = intr vector
                    spr[SPRN_DBSR] |= DBSR_IRPT;
                }
                if((subtype & PPC_EXCEPT_DBG_UDE) == PPC_EXCEPT_DBG_UDE){
                    *srr0 = pc + 4;
                    spr[SPRN_DBSR] |= DBSR_UDE;
                }
            }
            
            *srr1 = msr;

            CLR_DEFAULT_MSR_BITS();
            msr &= ~MSR_DE;               // Clear DE bit
            pc = GET_PC_FROM_IVOR_NUM(15);
            break;
        case  PPC_EXCEPTION_SPE_UA:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            spr[SPRN_ESR]  = ESR_SPV;              // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(32);
            break;
        case  PPC_EXCEPTION_EM_FP_D:
            // Check conditions
            if(((spr[SPRN_SPEFSCR] & SPEFSCR_FINVE) != 0) && (((spr[SPRN_SPEFSCR] & SPEFSCR_FINVH) != 0) ||
                        ((spr[SPRN_SPEFSCR] & SPEFSCR_FINV) != 0))){
                goto skip_0;
            }
            if(((spr[SPRN_SPEFSCR] & SPEFSCR_FDBZE) != 0) && (((spr[SPRN_SPEFSCR] & SPEFSCR_FDBZH) != 0) ||
                        ((spr[SPRN_SPEFSCR] & SPEFSCR_FDBZ) != 0))){
                goto skip_0;
            }
            if(((spr[SPRN_SPEFSCR] & SPEFSCR_FUNFE) != 0) && (((spr[SPRN_SPEFSCR] & SPEFSCR_FUNFH) != 0) ||
                        ((spr[SPRN_SPEFSCR] & SPEFSCR_FUNF) != 0))){
                goto skip_0;
            }
            if(((spr[SPRN_SPEFSCR] & SPEFSCR_FOVFE) != 0) && (((spr[SPRN_SPEFSCR] & SPEFSCR_FOVFH) != 0) ||
                        ((spr[SPRN_SPEFSCR] & SPEFSCR_FOVF) != 0))){
                goto skip_0;
            }
            RETURNVOID(DEBUG4);

            skip_0:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            spr[SPRN_ESR]  = ESR_SPV;  // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(33);
            break;
        case  PPC_EXCEPTION_EM_FP_R:
            // Check conditions
            if(((spr[SPRN_SPEFSCR] & SPEFSCR_FINXE) != 0) &&
                   (((spr[SPRN_SPEFSCR] & SPEFSCR_FGH) != 0) ||
                   ((spr[SPRN_SPEFSCR] & SPEFSCR_FXH) != 0)  ||
                   ((spr[SPRN_SPEFSCR] & SPEFSCR_FG) != 0)  ||
                   ((spr[SPRN_SPEFSCR] & SPEFSCR_FX) != 0))){
                goto skip_1;
            }
            if(((spr[SPRN_SPEFSCR] & SPEFSCR_FRMC) == 0x2) || ((spr[SPRN_SPEFSCR] & SPEFSCR_FRMC) == 0x3)){
                goto skip_1;
            }
            RETURNVOID(DEBUG4);

            skip_1:
            spr[SPRN_SRR0] = pc;
            spr[SPRN_SRR1] = msr;

            spr[SPRN_ESR] = ESR_SPV;   // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(34);
            break;
        case  PPC_EXCEPTION_PMM:
            // TODO: perf mon intr not supported for time being.
            break;
        case  PPC_EXCEPTION_DB:
            // Door bell. Nothing to do
            break;
        case  PPC_EXCEPTION_DB_CR:
            // Door bell. Nothing to do
            break;
        default:
            break;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : init_reghash
 * @args : none
 *
 * @brief : Initialize register pointers' hash for easy accessibility
 */
void cpu_ppc_booke::init_reghash(){
     LOG("DEBUG4") << MSG_FUNC_START;

    m_reghash["pc"]         = &(pc);                    m_ireghash[REG_PC]          = m_reghash["pc"];
    m_reghash["msr"]        = &(msr);                   m_ireghash[REG_MSR]         = m_reghash["msr"];
    m_reghash["cr"]         = &(cr);                    m_ireghash[REG_CR]          = m_reghash["cr"];
    m_reghash["fpscr"]      = &(fpscr);                 m_ireghash[REG_FPSCR]       = m_reghash["fpscr"];

    m_reghash["atbl"]       = &(spr[SPRN_ATBL]);        m_ireghash[REG_ATBL]        = m_reghash["atbl"];
    m_reghash["atbu"]       = &(spr[SPRN_ATBU]);        m_ireghash[REG_ATBU]        = m_reghash["atbu"];
    m_reghash["csrr0"]      = &(spr[SPRN_CSRR0]);       m_ireghash[REG_CSRR0]       = m_reghash["csrr0"];
    m_reghash["csrr1"]      = &(spr[SPRN_CSRR1]);       m_ireghash[REG_CSRR1]       = m_reghash["csrr1"];
    m_reghash["ctr"]        = &(spr[SPRN_CTR]);         m_ireghash[REG_CTR]         = m_reghash["ctr"];
    m_reghash["dac1"]       = &(spr[SPRN_DAC1]);        m_ireghash[REG_DAC1]        = m_reghash["dac1"];
    m_reghash["dac2"]       = &(spr[SPRN_DAC2]);        m_ireghash[REG_DAC2]        = m_reghash["dac2"];
    m_reghash["dbcr0"]      = &(spr[SPRN_DBCR0]);       m_ireghash[REG_DBCR0]       = m_reghash["dbcr0"];
    m_reghash["dbcr1"]      = &(spr[SPRN_DBCR1]);       m_ireghash[REG_DBCR1]       = m_reghash["dbcr1"];
    m_reghash["dbcr2"]      = &(spr[SPRN_DBCR2]);       m_ireghash[REG_DBCR2]       = m_reghash["dbcr2"];
    m_reghash["dbsr"]       = &(spr[SPRN_DBSR]);        m_ireghash[REG_DBSR]        = m_reghash["dbsr"];
    m_reghash["dear"]       = &(spr[SPRN_DEAR]);        m_ireghash[REG_DEAR]        = m_reghash["dear"];
    m_reghash["dec"]        = &(spr[SPRN_DEC]);         m_ireghash[REG_DEC]         = m_reghash["dec"];
    m_reghash["decar"]      = &(spr[SPRN_DECAR]);       m_ireghash[REG_DECAR]       = m_reghash["decar"];
    m_reghash["esr"]        = &(spr[SPRN_ESR]);         m_ireghash[REG_ESR]         = m_reghash["esr"];
    m_reghash["iac1"]       = &(spr[SPRN_IAC1]);        m_ireghash[REG_IAC1]        = m_reghash["iac1"];
    m_reghash["iac2"]       = &(spr[SPRN_IAC2]);        m_ireghash[REG_IAC2]        = m_reghash["iac2"];
    m_reghash["ivor0"]      = &(spr[SPRN_IVOR0]);       m_ireghash[REG_IVOR0]       = m_reghash["ivor0"];
    m_reghash["ivor1"]      = &(spr[SPRN_IVOR1]);       m_ireghash[REG_IVOR1]       = m_reghash["ivor1"]; 
    m_reghash["ivor2"]      = &(spr[SPRN_IVOR2]);       m_ireghash[REG_IVOR2]       = m_reghash["ivor2"];
    m_reghash["ivor3"]      = &(spr[SPRN_IVOR3]);       m_ireghash[REG_IVOR3]       = m_reghash["ivor3"];
    m_reghash["ivor4"]      = &(spr[SPRN_IVOR4]);       m_ireghash[REG_IVOR4]       = m_reghash["ivor4"];
    m_reghash["ivor5"]      = &(spr[SPRN_IVOR5]);       m_ireghash[REG_IVOR5]       = m_reghash["ivor5"];
    m_reghash["ivor6"]      = &(spr[SPRN_IVOR6]);       m_ireghash[REG_IVOR6]       = m_reghash["ivor6"];
    m_reghash["ivor8"]      = &(spr[SPRN_IVOR8]);       m_ireghash[REG_IVOR8]       = m_reghash["ivor8"];
    m_reghash["ivor10"]     = &(spr[SPRN_IVOR10]);      m_ireghash[REG_IVOR10]      = m_reghash["ivor10"];
    m_reghash["ivor11"]     = &(spr[SPRN_IVOR11]);      m_ireghash[REG_IVOR11]      = m_reghash["ivor11"];
    m_reghash["ivor12"]     = &(spr[SPRN_IVOR12]);      m_ireghash[REG_IVOR12]      = m_reghash["ivor12"];
    m_reghash["ivor13"]     = &(spr[SPRN_IVOR13]);      m_ireghash[REG_IVOR13]      = m_reghash["ivor13"];
    m_reghash["ivor14"]     = &(spr[SPRN_IVOR14]);      m_ireghash[REG_IVOR14]      = m_reghash["ivor14"];
    m_reghash["ivor15"]     = &(spr[SPRN_IVOR15]);      m_ireghash[REG_IVOR15]      = m_reghash["ivor15"];
    m_reghash["ivpr"]       = &(spr[SPRN_IVPR]);        m_ireghash[REG_IVPR]        = m_reghash["ivpr"];
    m_reghash["lr"]         = &(spr[SPRN_LR]);          m_ireghash[REG_LR]          = m_reghash["lr"];
    m_reghash["pid"]        = &(spr[SPRN_PID]);         m_ireghash[REG_PID]         = m_reghash["pid"];
    m_reghash["pir"]        = &(spr[SPRN_PIR]);         m_ireghash[REG_PIR]         = m_reghash["pir"];
    m_reghash["pvr"]        = &(spr[SPRN_PVR]);         m_ireghash[REG_PVR]         = m_reghash["pvr"];
    m_reghash["sprg0"]      = &(spr[SPRN_SPRG0]);       m_ireghash[REG_SPRG0]       = m_reghash["sprg0"];
    m_reghash["sprg1"]      = &(spr[SPRN_SPRG1]);       m_ireghash[REG_SPRG1]       = m_reghash["sprg1"];
    m_reghash["sprg2"]      = &(spr[SPRN_SPRG2]);       m_ireghash[REG_SPRG2]       = m_reghash["sprg2"];
    m_reghash["sprg3r"]     = &(spr[SPRN_SPRG3R]);      m_ireghash[REG_SPRG3R]      = m_reghash["sprg3r"];
    m_reghash["sprg3"]      = &(spr[SPRN_SPRG3]);       m_ireghash[REG_SPRG3]       = m_reghash["sprg3"];
    m_reghash["sprg4r"]     = &(spr[SPRN_SPRG4R]);      m_ireghash[REG_SPRG4R]      = m_reghash["sprg4r"];
    m_reghash["sprg4"]      = &(spr[SPRN_SPRG4]);       m_ireghash[REG_SPRG4]       = m_reghash["sprg4"];
    m_reghash["sprg5r"]     = &(spr[SPRN_SPRG5R]);      m_ireghash[REG_SPRG5R]      = m_reghash["sprg5r"];
    m_reghash["sprg5"]      = &(spr[SPRN_SPRG5]);       m_ireghash[REG_SPRG5]       = m_reghash["sprg5"];
    m_reghash["sprg6r"]     = &(spr[SPRN_SPRG6R]);      m_ireghash[REG_SPRG6R]      = m_reghash["sprg6r"];
    m_reghash["sprg6"]      = &(spr[SPRN_SPRG6]);       m_ireghash[REG_SPRG6]       = m_reghash["sprg6"];
    m_reghash["sprg7r"]     = &(spr[SPRN_SPRG7R]);      m_ireghash[REG_SPRG7R]      = m_reghash["sprg7r"];
    m_reghash["sprg7"]      = &(spr[SPRN_SPRG7]);       m_ireghash[REG_SPRG7]       = m_reghash["sprg7"];
    m_reghash["srr0"]       = &(spr[SPRN_SRR0]);        m_ireghash[REG_SRR0]        = m_reghash["srr0"];
    m_reghash["srr1"]       = &(spr[SPRN_SRR1]);        m_ireghash[REG_SRR1]        = m_reghash["srr1"];
    m_reghash["tbrl"]       = &(spr[SPRN_TBRL]);        m_ireghash[REG_TBRL]        = m_reghash["tbrl"];
    m_reghash["tbwl"]       = &(spr[SPRN_TBWL]);        m_ireghash[REG_TBWL]        = m_reghash["tbwl"];
    m_reghash["tbru"]       = &(spr[SPRN_TBRU]);        m_ireghash[REG_TBRU]        = m_reghash["tbru"];
    m_reghash["tbwu"]       = &(spr[SPRN_TBWU]);        m_ireghash[REG_TBWU]        = m_reghash["tbwu"];
    m_reghash["tcr"]        = &(spr[SPRN_TCR]);         m_ireghash[REG_TCR]         = m_reghash["tcr"];
    m_reghash["tsr"]        = &(spr[SPRN_TSR]);         m_ireghash[REG_TSR]         = m_reghash["tsr"];
    m_reghash["usprg0"]     = &(spr[SPRN_USPRG0]);      m_ireghash[REG_USPRG0]      = m_reghash["usprg0"];
    m_reghash["xer"]        = &(spr[SPRN_XER]);         m_ireghash[REG_XER]         = m_reghash["xer"];

    m_reghash["bbear"]      = &(spr[SPRN_BBEAR]);       m_ireghash[REG_BBEAR]       = m_reghash["bbear"];
    m_reghash["bbtar"]      = &(spr[SPRN_BBTAR]);       m_ireghash[REG_BBTAR]       = m_reghash["bbtar"];
    m_reghash["bucsr"]      = &(spr[SPRN_BUCSR]);       m_ireghash[REG_BUCSR]       = m_reghash["bucsr"];
    m_reghash["hid0"]       = &(spr[SPRN_HID0]);        m_ireghash[REG_HID0]        = m_reghash["hid0"];
    m_reghash["hid1"]       = &(spr[SPRN_HID1]);        m_ireghash[REG_HID1]        = m_reghash["hid1"];
    m_reghash["ivor32"]     = &(spr[SPRN_IVOR32]);      m_ireghash[REG_IVOR32]      = m_reghash["ivor32"];
    m_reghash["ivor33"]     = &(spr[SPRN_IVOR33]);      m_ireghash[REG_IVOR33]      = m_reghash["ivor33"];
    m_reghash["ivor34"]     = &(spr[SPRN_IVOR34]);      m_ireghash[REG_IVOR34]      = m_reghash["ivor34"];
    m_reghash["ivor35"]     = &(spr[SPRN_IVOR35]);      m_ireghash[REG_IVOR35]      = m_reghash["ivor35"];
    m_reghash["l1cfg0"]     = &(spr[SPRN_L1CFG0]);      m_ireghash[REG_L1CFG0]      = m_reghash["l1cfg0"];
    m_reghash["l1cfg1"]     = &(spr[SPRN_L1CFG1]);      m_ireghash[REG_L1CFG1]      = m_reghash["l1cfg1"];
    m_reghash["l1csr0"]     = &(spr[SPRN_L1CSR0]);      m_ireghash[REG_L1CSR0]      = m_reghash["l1csr0"];
    m_reghash["l1csr1"]     = &(spr[SPRN_L1CSR1]);      m_ireghash[REG_L1CSR1]      = m_reghash["l1csr1"];
    m_reghash["mas0"]       = &(spr[SPRN_MAS0]);        m_ireghash[REG_MAS0]        = m_reghash["mas0"];
    m_reghash["mas1"]       = &(spr[SPRN_MAS1]);        m_ireghash[REG_MAS1]        = m_reghash["mas1"];
    m_reghash["mas2"]       = &(spr[SPRN_MAS2]);        m_ireghash[REG_MAS2]        = m_reghash["mas2"];
    m_reghash["mas3"]       = &(spr[SPRN_MAS3]);        m_ireghash[REG_MAS3]        = m_reghash["mas3"];
    m_reghash["mas4"]       = &(spr[SPRN_MAS4]);        m_ireghash[REG_MAS4]        = m_reghash["mas4"];
    m_reghash["mas5"]       = &(spr[SPRN_MAS5]);        m_ireghash[REG_MAS5]        = m_reghash["mas5"];
    m_reghash["mas6"]       = &(spr[SPRN_MAS6]);        m_ireghash[REG_MAS6]        = m_reghash["mas6"];
    m_reghash["mas7"]       = &(spr[SPRN_MAS7]);        m_ireghash[REG_MAS7]        = m_reghash["mas7"];
    m_reghash["mcar"]       = &(spr[SPRN_MCAR]);        m_ireghash[REG_MCAR]        = m_reghash["mcar"];
    m_reghash["mcsr"]       = &(spr[SPRN_MCSR]);        m_ireghash[REG_MCSR]        = m_reghash["mcsr"];
    m_reghash["mcsrr0"]     = &(spr[SPRN_MCSRR0]);      m_ireghash[REG_MCSRR0]      = m_reghash["mcsrr0"];
    m_reghash["mcsrr1"]     = &(spr[SPRN_MCSRR1]);      m_ireghash[REG_MCSRR1]      = m_reghash["mcsrr1"];
    m_reghash["mmucfg"]     = &(spr[SPRN_MMUCFG]);      m_ireghash[REG_MMUCFG]      = m_reghash["mmucfg"];
    m_reghash["mmucsr0"]    = &(spr[SPRN_MMUCSR0]);     m_ireghash[REG_MMUCSR0]     = m_reghash["mmucsr0"];
    m_reghash["pid0"]       = &(spr[SPRN_PID0]);        m_ireghash[REG_PID0]        = m_reghash["pid0"];
    m_reghash["pid1"]       = &(spr[SPRN_PID1]);        m_ireghash[REG_PID1]        = m_reghash["pid1"];
    m_reghash["pid2"]       = &(spr[SPRN_PID2]);        m_ireghash[REG_PID2]        = m_reghash["pid2"];
    m_reghash["spefscr"]    = &(spr[SPRN_SPEFSCR]);     m_ireghash[REG_SPEFSCR]     = m_reghash["spefscr"];
    m_reghash["svr"]        = &(spr[SPRN_SVR]);         m_ireghash[REG_SVR]         = m_reghash["svr"];
    m_reghash["tlb0cfg"]    = &(spr[SPRN_TLB0CFG]);     m_ireghash[REG_TLB0CFG]     = m_reghash["tlb0cfg"];
    m_reghash["tlb1cfg"]    = &(spr[SPRN_TLB1CFG]);     m_ireghash[REG_TLB1CFG]     = m_reghash["tlb1cfg"];

    // PMRs
    m_reghash["pmc0"]       = &(pmr[PMRN_PMC0]);        m_ireghash[REG_PMC0]        = m_reghash["pmc0"];
    m_reghash["pmc1"]       = &(pmr[PMRN_PMC1]);        m_ireghash[REG_PMC1]        = m_reghash["pmc1"];
    m_reghash["pmc2"]       = &(pmr[PMRN_PMC2]);        m_ireghash[REG_PMC2]        = m_reghash["pmc2"];
    m_reghash["pmc3"]       = &(pmr[PMRN_PMC3]);        m_ireghash[REG_PMC3]        = m_reghash["pmc3"];
    m_reghash["pmlca0"]     = &(pmr[PMRN_PMLCA0]);      m_ireghash[REG_PMLCA0]      = m_reghash["pmlca0"];
    m_reghash["pmlca1"]     = &(pmr[PMRN_PMLCA1]);      m_ireghash[REG_PMLCA1]      = m_reghash["pmlca1"];
    m_reghash["pmlca2"]     = &(pmr[PMRN_PMLCA2]);      m_ireghash[REG_PMLCA2]      = m_reghash["pmlca2"];
    m_reghash["pmlca3"]     = &(pmr[PMRN_PMLCA3]);      m_ireghash[REG_PMLCA3]      = m_reghash["pmlca3"];
    m_reghash["pmlcb0"]     = &(pmr[PMRN_PMLCB0]);      m_ireghash[REG_PMLCB0]      = m_reghash["pmlcb0"];
    m_reghash["pmlcb1"]     = &(pmr[PMRN_PMLCB1]);      m_ireghash[REG_PMLCB1]      = m_reghash["pmlcb1"];
    m_reghash["pmlcb2"]     = &(pmr[PMRN_PMLCB2]);      m_ireghash[REG_PMLCB2]      = m_reghash["pmlcb2"];
    m_reghash["pmlcb3"]     = &(pmr[PMRN_PMLCB3]);      m_ireghash[REG_PMLCB3]      = m_reghash["pmlcb3"];
    m_reghash["pmgc0"]      = &(pmr[PMRN_PMGC0]);       m_ireghash[REG_PMGC0]       = m_reghash["pmgc0"];
    m_reghash["upmc0"]      = &(pmr[PMRN_UPMC0]);       m_ireghash[REG_UPMC0]       = m_reghash["upmc0"];
    m_reghash["upmc1"]      = &(pmr[PMRN_UPMC1]);       m_ireghash[REG_UPMC1]       = m_reghash["upmc1"];
    m_reghash["upmc2"]      = &(pmr[PMRN_UPMC2]);       m_ireghash[REG_UPMC2]       = m_reghash["upmc2"];
    m_reghash["upmc3"]      = &(pmr[PMRN_UPMC3]);       m_ireghash[REG_UPMC3]       = m_reghash["upmc3"];
    m_reghash["upmlca0"]    = &(pmr[PMRN_UPMLCA0]);     m_ireghash[REG_UPMLCA0]     = m_reghash["upmlca0"];
    m_reghash["upmlca1"]    = &(pmr[PMRN_UPMLCA1]);     m_ireghash[REG_UPMLCA1]     = m_reghash["upmlca1"];
    m_reghash["upmlca2"]    = &(pmr[PMRN_UPMLCA2]);     m_ireghash[REG_UPMLCA2]     = m_reghash["upmlca2"];
    m_reghash["upmlca3"]    = &(pmr[PMRN_UPMLCA3]);     m_ireghash[REG_UPMLCA3]     = m_reghash["upmlca3"];
    m_reghash["upmlcb0"]    = &(pmr[PMRN_UPMLCB0]);     m_ireghash[REG_UPMLCB0]     = m_reghash["upmlcb0"];
    m_reghash["upmlcb1"]    = &(pmr[PMRN_UPMLCB1]);     m_ireghash[REG_UPMLCB1]     = m_reghash["upmlcb1"];
    m_reghash["upmlcb2"]    = &(pmr[PMRN_UPMLCB2]);     m_ireghash[REG_UPMLCB2]     = m_reghash["upmlcb2"];
    m_reghash["upmlcb3"]    = &(pmr[PMRN_UPMLCB3]);     m_ireghash[REG_UPMLCB3]     = m_reghash["upmlcb3"];
    m_reghash["upmgc0"]     = &(pmr[PMRN_UPMGC0]);      m_ireghash[REG_UPMGC0]      = m_reghash["upmgc0"];

    // GPRS ad FPRS
    for (size_t i=0; i<PPC_NGPRS; i++){
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "r" << i))->str()] = &(gpr[i]); 
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "gpr" << i))->str()] = &(gpr[i]);
        m_ireghash[REG_GPR0 + i] = &(gpr[i]);
    }
    for (size_t i=0; i<PPC_NFPRS; i++){
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "f" << i))->str()] = &(fpr[i]);
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "fpr" << i))->str()] = &(fpr[i]);
        m_ireghash[REG_FPR0 + i] = &(fpr[i]);
    }

    // Add more later on
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Update CR0
void cpu_ppc_booke::update_cr0(bool use_host, uint64_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    int c;
    if(use_host){
       if(host_state.flags & X86_FLAGS_SF){ c = 8; }
       if(host_state.flags & X86_FLAGS_ZF){ c |= 2; }
    }else{
        if (bits == 64) {
            if ((int64_t)value < 0)
                c = 8;
            else if ((int64_t)value > 0)
                c = 4;
            else
                c = 2;
        } else {
            if ((int32_t)value < 0)
                c = 8;
            else if ((int32_t)value > 0)
                c = 4;
            else
                c = 2;
        }
    }

    /*  SO bit, copied from XER:  */
    c |= ((spr[SPRN_XER] >> 31) & 1);

    cr &= ~((uint32_t)0xf << 28);
    cr |= ((uint32_t)c << 28);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Updates CR[bf] with val i.e CR[bf] <- (val & 0xf)
// bf may range from [0:7]
void cpu_ppc_booke::update_crF(unsigned bf, uint64_t val){
    LOG("DEBUG4") << MSG_FUNC_START;
    bf &= 0x7;
    val &= 0xf;
    cr &= ~( 0xf << (7 - bf)*4 );
    cr |=  ((val & 0xf) << (7 - bf)*4);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get crF  ( F -> [0:7] )
unsigned cpu_ppc_booke::get_crF(unsigned bf){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (cr >> (7 - bf)*4) & 0xf;
}

// Update CR by exact field value [0:31]
void cpu_ppc_booke::update_crf(unsigned field, unsigned value){
    LOG("DEBUG4") << MSG_FUNC_START;
    field &= 0x1f;
    value &= 0x1;
    cr &= ~(0x1 << (31 - field));
    cr |= (value << (31 - field));
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get CR bit at exact field
unsigned cpu_ppc_booke::get_crf(unsigned field){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (cr >> (31 - field)) & 0x1;
}

// Update XER
void cpu_ppc_booke::update_xer(bool use_host, uint64_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint32_t c = 0;
    if(!use_host){
        // Don't use host facilities 
    }else{
        if(host_state.flags & X86_FLAGS_CF){ c = 2; }
        if(host_state.flags & X86_FLAGS_OF){
            c |= 4;
            if(curr_instr != "mtspr"){   /* If current instruction is not "mtspr", set SO bit also */
                c |= 8;
            }
        }
    }
    /* Set XER */
    spr[SPRN_XER] &= ~((uint32_t)0xf << 28);
    spr[SPRN_XER] |= (c << 28);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// bf field can be only 3 bits wide. If not, it's truncated to 3 bits
// val can be 4 bits only.
void cpu_ppc_booke::update_xerF(unsigned bf, unsigned val){
    LOG("DEBUG4") << MSG_FUNC_START;
    bf &= 0x7;
    val &= 0xf;
    spr[SPRN_XER] &= ~( 0xf << (7 - bf)*4 );
    spr[SPRN_XER] |=  ((val & 0xf) << (7 - bf)*4);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// value can be either 1 or 0 
void cpu_ppc_booke::update_xerf(unsigned field, unsigned value){
    LOG("DEBUG4") << MSG_FUNC_START;
    field &= 0x1f;
    value &= 0x1;
    spr[SPRN_XER] &= ~(0x1 << (31 - field));
    spr[SPRN_XER] |= (value << (31 - field));
    LOG("DEBUG4") << MSG_FUNC_END;
}

unsigned cpu_ppc_booke::get_xerF(unsigned bf){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (spr[SPRN_XER] >> (7 - bf)*4) & 0xf;
}

unsigned cpu_ppc_booke::get_xerf(unsigned field){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (spr[SPRN_XER] >> (31 - field)) & 0x1;
}

// Get XER[SO]
unsigned cpu_ppc_booke::get_xer_so(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return ((spr[SPRN_XER] & XER_SO) ? 1:0);
}

unsigned cpu_ppc_booke::get_xer_ca(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return ((spr[SPRN_XER] & XER_CA) ? 1:0);
}

// set cr
void cpu_ppc_booke::set_cr(uint32_t value) throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    cr = static_cast<uint64_t>(value);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get GPR value
uint64_t cpu_ppc_booke::get_gpr(int gprno) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(gprno >= PPC_NGPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal gprno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return gpr[gprno];
}

// Get spr value
uint64_t cpu_ppc_booke::get_spr(int sprno) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(sprno >= PPC_NSPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal sprno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return spr[sprno];
}

// Get pmr value
uint64_t cpu_ppc_booke::get_pmr(int pmrno) throw (sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(pmrno >= PPC_NPMRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal pmrno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return pmr[pmrno];
}

// Get fpr value
uint64_t cpu_ppc_booke::get_fpr(int fprno) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(fprno >= PPC_NFPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal fprno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return fpr[fprno];
}

// Get msr
uint64_t cpu_ppc_booke::get_msr() throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return msr;
}

// Get cr
uint64_t cpu_ppc_booke::get_cr() throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return cr;
}

// Get fpscr
uint64_t cpu_ppc_booke::get_fpscr() throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return fpscr;
}

// Get register value by name
uint64_t cpu_ppc_booke::get_reg(std::string reg_name) throw(sim_exception) {
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_reghash.find(reg_name) == m_reghash.end()) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal register name");
    LOG("DEBUG4") << MSG_FUNC_END;
    return *m_reghash[reg_name];
}

// Dump CPU state
void cpu_ppc_booke::dump_state(int columns, std::ostream &ostr, int dump_all_sprs){
    LOG("DEBUG4") << MSG_FUNC_START;
    int i;
    int colno = 0;
    ostringstream strout;

    // dump msr and cr
    ostr << BAR0 << std::endl;
    ostr << "CPU STATE" << std::endl;
    ostr << BAR0 << std::endl;

    ostr << "msr = " << std::hex << std::showbase << msr << std::endl;
    ostr << "cr  = " << std::hex << std::showbase << cr << std::endl;
    ostr << "iar = " << std::hex << std::showbase << pc << std::endl;
    ostr << std::endl;

    // dump gprs
    for(i=0; i<PPC_NGPRS; i++){
        strout << "r" << std::dec << i << " = " << std::hex << std::showbase << *m_ireghash[REG_GPR0 + i];
        ostr << std::left << std::setw(23) << strout.str() << "    ";
        strout.str("");
        if(++colno >= columns){ ostr << std::endl; colno = 0; }
    }
    ostr << std::endl;
    ostr << std::endl;
    colno = 0;

    // dump fprs
    for(i=0; i<PPC_NFPRS; i++){
        strout << "f" << std::dec << i << " = " << std::hex << std::showbase << *m_ireghash[REG_FPR0 + i];
        ostr << std::left << std::setw(23) << strout.str() << "    ";
        strout.str("");
        if(++colno >= columns){ ostr << std::endl; colno = 0; }
    }
    ostr << std::endl;
    colno = 0;

    // dump sprs
    if(dump_all_sprs){
        for(i=0; i<PPC_NSPRS; i++){
            // TODO: do a check for valid sprs later on
            strout << "spr[" << std::dec << i << "] = " << std::hex << std::showbase << spr[i];
            ostr << std::left << std::setw(26) << strout.str() << "    ";
            strout.str("");
            if(++colno >= columns){ ostr << std::endl; colno = 0; }
        }
        ostr << std::endl;
        colno = 0;
    }else{
        ostr << std::hex << std::showbase;
        ostr << "ctr = " << *m_reghash["ctr"] << std::endl;
        ostr << "xer = " << *m_reghash["xer"] << std::endl;
        ostr << "lr  = " << *m_reghash["lr"] << std::endl;
        ostr << std::endl;

        ostr << "mas0 = " << *m_reghash["mas0"] << std::endl;
        ostr << "mas1 = " << *m_reghash["mas1"] << std::endl;
        ostr << "mas2 = " << *m_reghash["mas2"] << std::endl;
        ostr << "mas3 = " << *m_reghash["mas3"] << std::endl;
        ostr << "mas4 = " << *m_reghash["mas4"] << std::endl;
        ostr << "mas5 = " << *m_reghash["mas5"] << std::endl;
        ostr << "mas6 = " << *m_reghash["mas6"] << std::endl;
        ostr << "mas7 = " << *m_reghash["mas7"] << std::endl;

    }
    ostr << BAR0 << std::endl;
    LOG("DEBUG4") << MSG_FUNC_END;
}

#endif    /*  CPU_PPC_H  */
