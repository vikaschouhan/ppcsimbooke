#ifndef    _CPU_PPC_HPP_
#define    _CPU_PPC_HPP_

#include "config.h"
#include "tlb_booke.hpp"
#include "ppc_exception.hpp"
#include "cpu.hpp"
#include "cpu_host.h"
#include "cpu_ppc_quirks.hpp"
#include "memory.hpp"
#include "third_party/ppc/reg.h"
#include "regfields.h"


#define    PPC_NGPRS        32
#define    PPC_NFPRS        32
#define    PPC_NVRS         32
#define    PPC_NSPRS        1024
#define    PPC_NPMRS        1024
#define    PPC_NTGPRS       4

/* 64 bit MSRs were used in older powerPC designs */
/* All BookE cores have 32 bit MSRs only */

/*
 * booke cpu
 * NOTE: All registers are 64 bits.
 *       ( even if some of them are 32 bits in actual hardware. Upper 32 bits are just ignored in those cases )
 */
class ppc_cpu_booke : public cpu {

    public:

    // All virtual functions
    int run_instr(instr_call *ic);
    int translate_v2p(uint64_t vaddr, uint64_t *return_paddr, int flags);
    void update_translation_table(uint64_t vaddr_page, unsigned char *host_page, int writeflag, uint64_t paddr_page);
    void invalidate_translation_caches(uint64_t paddr, int flags);
    void invalidate_code_translation(uint64_t paddr, int flags);
    // Overloaded run
    int run_instr(std::string opcode, std::string arg0="", std::string arg1="", std::string arg2="",
            std::string arg3="", std::string arg4="", std::string arg5="");

    protected:
    // Set GPR value
    void set_gpr(int gprno, uint64_t value) throw(sim_exception);
    // Set spr value
    void set_spr(int sprno, uint64_t value) throw(sim_exception);
    // Set pmr value
    void set_pmr(int pmrno, uint64_t value) throw(sim_exception);
    // set fpr value
    void set_fpr(int fprno, uint64_t value) throw(sim_exception);
    // set msr
    void set_msr(uint32_t value) throw();
    // set cr
    void set_cr(uint32_t value) throw();
    // set fpscr
    void set_fprscr(uint32_t value) throw();

    public:
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
    uint64_t get_fprscr() throw();
    // Dump CPU state
    void dump_state(int columns=0, std::ostream &ostr=std::cout, int dump_all_sprs=0);

    // Create a new CPU instance
    static ppc_cpu_booke *create(uint64_t cpuid, std::string name);
    // Destroy cpu instance
    static void destroy(ppc_cpu_booke *cpu);

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
    // Update XER
    void update_xer(bool use_host, uint64_t value=0);
    void update_xerF(unsigned bf, unsigned val);
    void update_xerf(unsigned field, unsigned value);
    unsigned get_xerF(unsigned bf);
    unsigned get_xerf(unsigned field);
    // Get XER[SO]
    unsigned get_xer_so();
    unsigned get_xer_ca();

    protected:
    /* We are taking both contructors and destructors private */
    /* 
     * @func : constructor for this cpu
     * @args : cpuid and name
     *
     * @notes : Default program counter at beginning ( reset vector ) is 0xfffffffc
     *
     * @brief : inittializes a new cpu instance
     */
    ppc_cpu_booke(uint64_t cpuid, string name): cpu(cpuid, name, 0xfffffffc){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->cpu_no = this->ncpus++;
        init_reghash();
        /* Add other things */
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    /*
     * @func : destructor
     */
    ~ppc_cpu_booke(){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->ncpus--;
        LOG("DEBUG4") << MSG_FUNC_END;
    }

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
    friend void gen_ppc_opc_func_hash(ppc_cpu_booke *cpu);

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

    uint64_t    ll_addr;           /*  Load-linked / store-conditional  */
    int         ll_bit;

    uint8_t     cpu_no;            /* Numerical cpu no */

    /* Processor bookkeeping */
    static size_t                         ncpus;
    static std::list<ppc_cpu_booke *>     cpu_list;  /* vector of pointers to cpus */

    // Pointers to generic registers/stuff hashed by name
    std::map<std::string, uint64_t*>      m_reghash;
    typedef std::map<std::string, uint64_t*>::iterator m_reghash_iter;

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
    typedef void (*ppc_opc_fun_ptr)(ppc_cpu_booke *, struct instr_call *);
    std::map<std::string, ppc_opc_fun_ptr>  ppc_func_hash;

    // Implementaion specific fixes
    static ppc_cpu_booke_quirks  quirks;

};

// --------------------------- STATIC DATA ---------------------------------------------------

// This is list of pointers to all alive cpu objects , so that we could refer them using it's class type only.
std::list<ppc_cpu_booke *>     ppc_cpu_booke::cpu_list; /* vector of pointers to cpus */
size_t                         ppc_cpu_booke::ncpus = 0; /* Current number of powerpc cpus */

// Cpu fixes for booke
ppc_cpu_booke_quirks           ppc_cpu_booke::quirks;

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
int ppc_cpu_booke::run_instr(instr_call *ic){
    LOG("DEBUG4") << MSG_FUNC_START;
    ppc_func_hash[ic->opcode](this, ic);
    LOG("DEBUG4") << MSG_FUNC_END;
    return 0;
}

int ppc_cpu_booke::translate_v2p(uint64_t vaddr, uint64_t *return_paddr, int flags){
    return 0;
}

void ppc_cpu_booke::update_translation_table(uint64_t vaddr_page, unsigned char *host_page, int writeflag, uint64_t paddr_page){
}

void ppc_cpu_booke::invalidate_translation_caches(uint64_t paddr, int flags){
}

void ppc_cpu_booke::invalidate_code_translation(uint64_t paddr, int flags){
}

// Second form of run_instr
// Seems like c++ doesn't have an very effective way of handling non POD objects
//  with variadic arg funcs
int ppc_cpu_booke::run_instr(std::string opcode, std::string arg0, std::string arg1, std::string arg2,
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
void ppc_cpu_booke::init_spr_attrs(){
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
void ppc_cpu_booke::ppc_exception(int exception_nr, uint64_t subtype=0, uint64_t ea)
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
void ppc_cpu_booke::init_reghash(){
     LOG("DEBUG4") << MSG_FUNC_START;
    // SPRS 
    m_reghash["pc"]         = &(pc);
    m_reghash["msr"]        = &(msr);
    m_reghash["ctr"]        = &(spr[SPRN_CTR]);
    m_reghash["xer"]        = &(spr[SPRN_XER]);
    m_reghash["dec"]        = &(spr[SPRN_DEC]);
    m_reghash["srr0"]       = &(spr[SPRN_SRR0]);
    m_reghash["srr1"]       = &(spr[SPRN_SRR1]);
    m_reghash["csrr0"]      = &(spr[SPRN_CSRR0]);
    m_reghash["csrr1"]      = &(spr[SPRN_CSRR1]);
    m_reghash["lr"]         = &(spr[SPRN_LR]);
    m_reghash["mas0"]       = &(spr[SPRN_MAS0]);
    m_reghash["mas1"]       = &(spr[SPRN_MAS1]);
    m_reghash["mas2"]       = &(spr[SPRN_MAS2]);
    m_reghash["mas3"]       = &(spr[SPRN_MAS3]);
    m_reghash["mas4"]       = &(spr[SPRN_MAS4]);
    m_reghash["mas5"]       = &(spr[SPRN_MAS5]);
    m_reghash["mas6"]       = &(spr[SPRN_MAS6]);
    m_reghash["mas7"]       = &(spr[SPRN_MAS7]);
    m_reghash["tbrl"]       = &(spr[SPRN_TBRL]);
    m_reghash["tbru"]       = &(spr[SPRN_TBRU]);
    m_reghash["tbwl"]       = &(spr[SPRN_TBWL]);
    m_reghash["tbwu"]       = &(spr[SPRN_TBWU]);
    m_reghash["pir"]        = &(spr[SPRN_PIR]);
    m_reghash["cr"]         = &(cr);
    m_reghash["fpscr"]      = &(fpscr);

    // GPRS ad FPRS
    for (size_t i=0; i<PPC_NGPRS; i++){
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "r" << i))->str()] = &(gpr[i]); 
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "gpr" << i))->str()] = &(gpr[i]);
    }
    for (size_t i=0; i<PPC_NFPRS; i++){
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "f" << i))->str()] = &(fpr[i]);
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "fpr" << i))->str()] = &(fpr[i]);
    }

    // Add more later on
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Update CR0
void ppc_cpu_booke::update_cr0(bool use_host, uint64_t value){
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
void ppc_cpu_booke::update_crF(unsigned bf, uint64_t val){
    LOG("DEBUG4") << MSG_FUNC_START;
    bf &= 0x7;
    val &= 0xf;
    cr &= ~( 0xf << (7 - bf)*4 );
    cr |=  ((val & 0xf) << (7 - bf)*4);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get crF  ( F -> [0:7] )
unsigned ppc_cpu_booke::get_crF(unsigned bf){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (cr >> (7 - bf)*4) & 0xf;
}

// Update CR by exact field value [0:31]
void ppc_cpu_booke::update_crf(unsigned field, unsigned value){
    LOG("DEBUG4") << MSG_FUNC_START;
    field &= 0x1f;
    value &= 0x1;
    cr &= ~(0x1 << (31 - field));
    cr |= (value << (31 - field));
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get CR bit at exact field
unsigned ppc_cpu_booke::get_crf(unsigned field){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (cr >> (31 - field)) & 0x1;
}

// Update XER
void ppc_cpu_booke::update_xer(bool use_host, uint64_t value){
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
void ppc_cpu_booke::update_xerF(unsigned bf, unsigned val){
    LOG("DEBUG4") << MSG_FUNC_START;
    bf &= 0x7;
    val &= 0xf;
    spr[SPRN_XER] &= ~( 0xf << (7 - bf)*4 );
    spr[SPRN_XER] |=  ((val & 0xf) << (7 - bf)*4);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// value can be either 1 or 0 
void ppc_cpu_booke::update_xerf(unsigned field, unsigned value){
    LOG("DEBUG4") << MSG_FUNC_START;
    field &= 0x1f;
    value &= 0x1;
    spr[SPRN_XER] &= ~(0x1 << (31 - field));
    spr[SPRN_XER] |= (value << (31 - field));
    LOG("DEBUG4") << MSG_FUNC_END;
}

unsigned ppc_cpu_booke::get_xerF(unsigned bf){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (spr[SPRN_XER] >> (7 - bf)*4) & 0xf;
}

unsigned ppc_cpu_booke::get_xerf(unsigned field){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (spr[SPRN_XER] >> (31 - field)) & 0x1;
}

// Get XER[SO]
unsigned ppc_cpu_booke::get_xer_so(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return ((spr[SPRN_XER] & XER_SO) ? 1:0);
}

unsigned ppc_cpu_booke::get_xer_ca(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return ((spr[SPRN_XER] & XER_CA) ? 1:0);
}

// Set GPR value
void ppc_cpu_booke::set_gpr(int gprno, uint64_t value) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(gprno >= PPC_NGPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal gprno");
    gpr[gprno] = value;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Set spr value
void ppc_cpu_booke::set_spr(int sprno, uint64_t value) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(sprno >= PPC_NSPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal sprno");
    spr[sprno] = value;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Set pmr value
void ppc_cpu_booke::set_pmr(int pmrno, uint64_t value) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(pmrno >= PPC_NPMRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal pmrno");
    pmr[pmrno] = value;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// set fpr value
void ppc_cpu_booke::set_fpr(int fprno, uint64_t value) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(fprno >= PPC_NFPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal fprno");
    fpr[fprno] = value;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// set msr
void ppc_cpu_booke::set_msr(uint32_t value) throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    msr = static_cast<uint64_t>(value);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// set cr
void ppc_cpu_booke::set_cr(uint32_t value) throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    cr = static_cast<uint64_t>(value);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// set fpscr
void ppc_cpu_booke::set_fprscr(uint32_t value) throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    fpscr = static_cast<uint64_t>(value);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get GPR value
uint64_t ppc_cpu_booke::get_gpr(int gprno) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(gprno >= PPC_NGPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal gprno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return gpr[gprno];
}

// Get spr value
uint64_t ppc_cpu_booke::get_spr(int sprno) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(sprno >= PPC_NSPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal sprno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return spr[sprno];
}

// Get pmr value
uint64_t ppc_cpu_booke::get_pmr(int pmrno) throw (sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(pmrno >= PPC_NPMRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal pmrno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return pmr[pmrno];
}

// Get fpr value
uint64_t ppc_cpu_booke::get_fpr(int fprno) throw(sim_exception){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(fprno >= PPC_NFPRS) throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Illegal fprno");
    LOG("DEBUG4") << MSG_FUNC_END;
    return fpr[fprno];
}

// Get msr
uint64_t ppc_cpu_booke::get_msr() throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return msr;
}

// Get cr
uint64_t ppc_cpu_booke::get_cr() throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return cr;
}

// Get fpscr
uint64_t ppc_cpu_booke::get_fprscr() throw() {
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return fpscr;
}

// Dump CPU state
void ppc_cpu_booke::dump_state(int columns, std::ostream &ostr, int dump_all_sprs){
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
    ostr << "GPRS" << std::endl;
    for(i=0; i<PPC_NGPRS; i++){
        strout << "r" << std::dec << i << " = " << std::hex << std::showbase << gpr[i];
        ostr << std::left << std::setw(23) << strout.str() << "    ";
        strout.str("");
        if(++colno >= columns){ ostr << std::endl; colno = 0; }
    }
    ostr << std::endl;
    colno = 0;

    // dump fprs
    ostr << "FPRS" << std::endl;
    for(i=0; i<PPC_NFPRS; i++){
        strout << "f" << std::dec << i << " = " << std::hex << std::showbase << fpr[i];
        ostr << std::left << std::setw(23) << strout.str() << "    ";
        strout.str("");
        if(++colno >= columns){ ostr << std::endl; colno = 0; }
    }
    ostr << std::endl;
    colno = 0;

    // dump sprs
    ostr << "SPRS" << std::endl;
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

        ostr << "MAS registers" << std::endl;
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


// Create a new CPU instance
ppc_cpu_booke* ppc_cpu_booke::create(uint64_t cpuid, string name){
    LOG("DEBUG4") << MSG_FUNC_START;
    ppc_cpu_booke *new_cpu = new ppc_cpu_booke(cpuid, name);
    // Generate opc to func_ptr hash
    gen_ppc_opc_func_hash(new_cpu);
    cpu_list.push_back(new_cpu);
    LOG("DEBUG4") << MSG_FUNC_END;
    return new_cpu;
}

// Destroy cpu instance
void ppc_cpu_booke::destroy(ppc_cpu_booke *cpu){
    LOG("DEBUG4") << MSG_FUNC_START;
    cpu_list.remove(cpu);
    LOG("DEBUG4") << MSG_FUNC_END;
    delete cpu;
}

#endif    /*  CPU_PPC_H  */
