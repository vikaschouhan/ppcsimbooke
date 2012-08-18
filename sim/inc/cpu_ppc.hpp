#ifndef    _CPU_PPC_HPP_
#define    _CPU_PPC_HPP_

#include "config.h"
#include "tlb_booke.hpp"             // BookE MMU
#include "ppc_exception.h"           // Exception tables
#include "ppc_dis.hpp"               // Disassembler module
#include "cpu.hpp"                   // Parent CPU class
#include "cpu_ppc_regs.h"            // PPC register table
#include "cpu_host.h"                // Some host definitions
#include "memory.hpp"                // Memory module
#include "bm.hpp"                    // Software Breakpoint manager

// powerPC register attribute flags
// if attr=0, this means the register is illegal
#define REG_READ_SUP        0x00000001UL                         // Register has read access in supervisor mode
#define REG_WRITE_SUP       0x00000002UL                         // Register has write access in supervisor mode
#define REG_READ_USR        0x00000004UL                         // Register has read access in user mode
#define REG_WRITE_USR       0x00000008UL                         // Register has write access in user mode
#define REG_CLEAR_W1C       0x00000010UL                         // Register can be cleared by writing 1 to the bits
#define REG_REQ_SYNC        0x00000020UL                         // Register write requires synchronization

/* 64 bit MSRs were used in older powerPC designs */
/* All BookE cores have 32 bit MSRs only */

/*
 * booke cpu
 * NOTE: All registers are 64 bits.
 *       ( even if some of them are 32 bits in actual hardware. Upper 32 bits are just ignored in those cases )
 */
#define CPU_PPC cpu_ppc

class CPU_PPC : public cpu {

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
    CPU_PPC(){
        LOG("DEBUG4") << MSG_FUNC_START;
        init_common();
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    CPU_PPC(uint64_t cpuid, std::string name): cpu(cpuid, name, 0xfffffffc){
        LOG("DEBUG4") << MSG_FUNC_START;
        init_common(); 
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    /*
     * @func : destructor
     */
    ~CPU_PPC(){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->ncpus--;
        LOG("DEBUG4") << MSG_FUNC_END;
    } 

    // Initalize CPU ( for objects  being created by default constructor )
    void CAT(init_, CPU_PPC)(uint64_t cpuid, std::string name){
        init_cpu(cpuid, name, 0xfffffffc);
    }
    
    // Register memory
    void register_mem(memory &mem){
        if(m_mem_ptr == NULL)
            m_mem_ptr = &mem;
    }

    // All virtual functions
    void run();
    void run_instr(std::string instr);
    void step(size_t instr_cnt=1);      // by default step by 1 ic cnt

    // normal overloaded version of run_instr ( NOTE: it's not virtual )
    void run_instr(uint32_t opcd);

    // All memory read/write functions ( these act on effective addresses and address
    // translation is done by the tlb module )
    // These only act on data pages
    // Even if storing to an instruction page, it should
    // be treated as data only i.e a separate data tlb entry should be defined which maps
    // to the instruction physical page ).
    // NOTE : store to an instruction page always goes through DTLB. ITLB is only used for
    // instruction fetches.
    uint8_t read8(uint64_t addr);
    void write8(uint64_t addr, uint8_t value);
    uint16_t read16(uint64_t addr);
    void write16(uint64_t addr, uint16_t value);
    uint32_t read32(uint64_t addr);
    void write32(uint64_t addr, uint32_t value);
    uint64_t read64(uint64_t addr);
    void write64(uint64_t addr, uint64_t value); 
    
    // Get Register by name
    uint64_t get_reg(std::string name) throw(sim_except);
    // Dump CPU state
    void dump_state(int columns=0, std::ostream &ostr=std::cout, int dump_all_sprs=0);
    // print L2 tlbs
    void print_L2tlbs();

    // Initialize register attributes
    void init_reg_attrs();
    //
    // for boost::python
    ppc_regs& ___get_regs(){
        return regs;
    }
    bool operator==(CPU_PPC const &x) const {
        return cpu_no == x.cpu_no;
    }
    bool operator!=(CPU_PPC const &x) const {
        return cpu_no != x.cpu_no;
    }

    protected:
    // notify of context switches ( called by context syncronizing instructions
    // such as rfi, sc etc. )
    void notify_ctxt_switch();
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

    // Translate ( conver EA to PA )
    // Returns a pair ( first arg is the address, second is the wimge attribute )
    std::pair<uint64_t, uint8_t> xlate(uint64_t addr, bool wr=0);

    // Accessing registers using reghash interface ( for use with ppc code translation unit )
    // Permissions will also be checked here itself
    // TODO : Check Permissions
    inline uint64_t& reg(int regid){
        LASSERT_THROW(m_ireghash.find(regid) != m_ireghash.end(),
               sim_except(SIM_EXCEPT_EINVAL, "Invalid register id " + boost::lexical_cast<std::string>(regid)), DEBUG4);
        return m_ireghash[regid]->value;
    }
    inline uint64_t& regn(std::string regname){
        LASSERT_THROW(m_reghash.find(regname) != m_reghash.end(),
               sim_except(SIM_EXCEPT_EINVAL, "Invalid register name " + regname), DEBUG4);
        // Do several checks
        if(!m_reghash[regname]->attr){
            std::cout << "Warning !! Invalid register " << regname << std::endl;
        }
        return m_reghash[regname]->value;
    }

    private:
    void init_reghash();
    void ppc_exception(int exception_nr, uint64_t subtype, uint64_t ea=0xffffffffffffffffULL);
    instr_call get_instr();           // Automatically tries to read instr from next NIP(PC)
#define DBG_EVENT_IAC      0x00000001UL
#define DBG_EVENT_DAC_LD   0x00000002UL
#define DBG_EVENT_DAC_ST   0x00000004UL
    // Throws debug events
    // ea is used only in case of DAC events
    void check_for_dbg_events(int flags, uint64_t ea=0);   // check for debug events
    // run curr instr
    inline void run_curr_instr();
    // Init common stuff
    void init_common(){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->cpu_no = this->ncpus++;          // Increment global cpu cnt
        init_reghash();                        // Initialize registers' hash 
        init_reg_attrs();                      // Initialize registers' attributes
        gen_ppc_opc_func_hash(this);           // Initialize opcode function pointer table
        m_instr_cache.set_size(512);           // LRU cache size = 512 instrs
        m_ctxt_switch = 0;                     // Initialize flag to zero
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    public:
    // Breakpoint manager
    BM                                     m_bm;
    
    private: 
    // This function defines nested functions ( using struct encapsulation technique )
    // Each function implements the pseudocode for one ( exactly one ) ppc opcode.
    //
    // For eg. there is one function for "add", another for "or" and likewise
    //
    // NOTE : It's the responsibility of this function to populate ppc_func_hash and
    //        it has to be called once in the constructor.
    friend void gen_ppc_opc_func_hash(CPU_PPC *cpu);

    int         mode;              /*  MODE_PPC or MODE_POWER  */
    int         bits;              /*  32 or 64  */

    int         irq_asserted;      /*  External Interrupt flag  */
    int         dec_intr_pending;  /* Decrementer interrupt pending  */
    uint64_t    zero;              /*  A zero register  */

    // powerPC register file
    ppc_regs    regs;                      // PPC register file

    // Book keeping
    uint8_t                               cpu_no;            /* Numerical cpu no */
    static size_t                         ncpus;

    // Pointers to generic registers/stuff hashed by name and numerical identifiers
    std::map<std::string, ppc_reg64*>      m_reghash;
    std::map<int, ppc_reg64*>              m_ireghash;
#define PPCREG(reg_id)                 (m_ireghash[reg_id]->value)
#define PPCREGN(reg_name)              (m_reghash[reg_name]->value)
#define PPCREGMASK(reg_id, mask)       EBMASK(PPCREG(reg_id),    mask)
#define PPCREGNMASK(reg_name, mask)    EBMASK(PPCREGN(reg_name), mask)

    // Disassembler module
    DIS_PPC                                m_dis;
    // Tlb
#if CORE_TYPE == e500v2
    TLB_PPC<128,4,16>                      m_l2tlb;            /* Only L2 tlb is implemented */
#else
    TLB_PPC<128,2,16>                      m_l2tlb;
#endif
    // memory module
    memory                                 *m_mem_ptr;
    

    /* Host specific stuff */
#if HOST_ARCH == x86_64
    struct x64_cpu_state host_state;
#elif HOST_ARCH == i686
    struct x86_cpu_state host_state;
#endif

    // A Cache of recently called instrs.
    lru_cache<uint64_t, instr_call>        m_instr_cache;     // Cache of recently used instrs
    bool                                   m_ctxt_switch;     // Flag indicating that a ctxt switch happened

    // opcode to function pointer map
    typedef void (*ppc_opc_fun_ptr)(CPU_PPC *, struct instr_call *);
    std::map<uint64_t, ppc_opc_fun_ptr>    ppc_func_hash;

};

// --------------------------- STATIC DATA ---------------------------------------------------

size_t                         CPU_PPC::ncpus = 0;        // Current number of powerpc cpus


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
// TODO:  this is very new. May or may not work.
//        NO ppc exception support at this time
void CPU_PPC::run(){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::pair<uint64_t, bool> last_bkpt = m_bm.last_breakpoint();

    // run this instruction if this was last breakpointed
    if(last_bkpt.first == pc and last_bkpt.second == true){
        m_bm.disable_breakpoints();
        run_curr_instr();
        m_bm.clear_last_breakpoint();
        m_bm.enable_breakpoints();
    }

#define I  run_curr_instr() 
        

    for(;;){
        // Observe each instruction for possible exceptions
        try {
            // Execute 32 instrs without looping again
            // Loop unrolling above 32 instrs, makes compilation slow like crazy
            // ( it already takes too much ), so I am sticking with a low count here.
            I; I; I; I; I; I; I; I;         I; I; I; I; I; I; I; I;
            I; I; I; I; I; I; I; I;         I; I; I; I; I; I; I; I;
        }
        catch(sim_except_ppc& e){
            ppc_exception(e.err_code0(), e.err_code1(), e.addr());
        }

        // Periodically check for any python error signals ( only for boost python )
        if(py_signal_callback::callback != NULL)
            if(py_signal_callback::callback())
                goto loop_exit_0;
    }
    loop_exit_0:
    ;
#undef I
    LOG("DEBUG4") << MSG_FUNC_END;
}

// step operation
void CPU_PPC::step(size_t instr_cnt){
    LOG("DEBUG4") << MSG_FUNC_START;

    size_t t=0;
    if(!instr_cnt) return;

#define I     run_curr_instr()

    for(t=0; t<instr_cnt; t++){
        try{
            I;
        }
        catch(sim_except_ppc& e){
            sim_except_ppc(e.err_code0(), e.err_code1(), e.addr());
        }
    }
#undef I
    LOG("DEBUG4") << MSG_FUNC_END;
}

// virtual form of run_instr
// Seems like c++ doesn't have an very effective way of handling non POD objects
//  with variadic arg funcs
void CPU_PPC::run_instr(std::string instr){
    LOG("DEBUG4") << MSG_FUNC_START;
    instr_call call_this;

    call_this = m_dis.disasm(instr, pc);

    if(call_this.fptr){ (reinterpret_cast<CPU_PPC::ppc_opc_fun_ptr>(call_this.fptr))(this, &call_this); }
    
    LASSERT_THROW(ppc_func_hash.find(call_this.opc) != ppc_func_hash.end(),
            sim_except(SIM_EXCEPT_EINVAL, "Invalid/Unimplemented opcode " + call_this.opcname), DEBUG4);
    call_this.fptr = reinterpret_cast<void*>(ppc_func_hash[call_this.opc]);
    ppc_func_hash[call_this.opc](this, &call_this);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Overloaded form of run_instr ( specifically for CPU_PPC )
void CPU_PPC::run_instr(uint32_t opcd){
    LOG("DEBUG4") << MSG_FUNC_START;
    instr_call call_this;

    call_this = m_dis.disasm(opcd, pc);
    if(call_this.fptr){ (reinterpret_cast<CPU_PPC::ppc_opc_fun_ptr>(call_this.fptr))(this, &call_this); }

    LASSERT_THROW(ppc_func_hash.find(call_this.opc) != ppc_func_hash.end(),
            sim_except(SIM_EXCEPT_EINVAL, "Invalid/Unimplemented opcode " + call_this.opcname), DEBUG4);
    call_this.fptr = reinterpret_cast<void*>(ppc_func_hash[call_this.opc]);
    ppc_func_hash[call_this.opc](this, &call_this);
    LOG("DEBUG4") << MSG_FUNC_END;
}

#define TO_RWX(r, w, x) (((r & 0x1) << 2) | ((w & 0x1) << 1) | (x & 0x1))
// Translate EA to RA ( for data only )
// NOTE: All exceptions ( hardware/software ) will be handled at run_instr() or run() level.
std::pair<uint64_t, uint8_t> CPU_PPC::xlate(uint64_t addr, bool wr){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::pair<uint64_t, uint8_t> res;
    uint8_t  perm = (wr) ? TO_RWX(0, 1, 0) : TO_RWX(1, 0, 0);
    bool as = EBMASK(PPCREG(REG_MSR), MSR_DS);
    bool pr = EBMASK(PPCREG(REG_MSR), MSR_PR);

    // Try hits with PID0, PID1 and PID2
    res = m_l2tlb.xlate(addr, as, PPCREG(REG_PID0), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0;
    res = m_l2tlb.xlate(addr, as, PPCREG(REG_PID1), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0; 
    res = m_l2tlb.xlate(addr, as, PPCREG(REG_PID2), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0;

    // We encountered TLB miss. Throw exceptions
    std::cout << "DTLB miss" << std::endl;
    if(wr){
        LTHROW(sim_except_ppc(PPC_EXCEPTION_DTLB, PPC_EXCEPT_DTLB_MISS_ST, "DTLB miss on store."), DEBUG4);
    }else{
        LTHROW(sim_except_ppc(PPC_EXCEPTION_DTLB, PPC_EXCEPT_DTLB_MISS_LD, "DTLB miss on load."), DEBUG4);
    }

    exit_loop_0:
    LOG("DEBUG4") << std::hex << std::showbase << "Xlation : " << addr << " -> " << res.first << std::endl;
    LOG("DEBUG4") << MSG_FUNC_END;
    return std::pair<uint64_t, uint8_t>(res.first, res.second);
}

// Memory I/O functions
uint8_t CPU_PPC::read8(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);
 
    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read8(res.first, (res.second & 0x1));
}

void CPU_PPC::write8(uint64_t addr, uint8_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write8(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

uint16_t CPU_PPC::read16(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read16(res.first, (res.second & 0x1));
}


void CPU_PPC::write16(uint64_t addr, uint16_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write16(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

uint32_t CPU_PPC::read32(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read32(res.first, (res.second & 0x1));
}

void CPU_PPC::write32(uint64_t addr, uint32_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write32(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

uint64_t CPU_PPC::read64(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read64(res.first, (res.second & 0x1));
}

void CPU_PPC::write64(uint64_t addr, uint64_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write64(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Initialize register attributes
void CPU_PPC::init_reg_attrs(){
    // spr_no[5] denotes priviledge level of SPR
    // If 1 -> the SPR is supervisor only,
    // If 0 -> it's accessible from both User and supervisor mode
#define PPCREGATTR(regtype, regno)  (regs.regtype[regno].attr)
    int i;

    // Set MSR attributes
    regs.msr.attr  = REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR  ;

    // Set GPRs and FPRs attributes
    for(i=0; i<PPC_NGPRS; i++){
        PPCREGATTR(gpr, i)  = REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR  | REG_WRITE_USR ;
    }
    for(i=0; i<PPC_NFPRS; i++){
        PPCREGATTR(fpr, i)  = REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR  | REG_WRITE_USR ;
    }

    // Set SPRs attributes
    PPCREGATTR(spr, SPRN_ATBL     )    = REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR                         ;
    PPCREGATTR(spr, SPRN_ATBU     )    = REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR                         ;
    PPCREGATTR(spr, SPRN_CSRR0    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_CSRR1    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_CTR      )    = REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR  | REG_WRITE_USR        ;  
    PPCREGATTR(spr, SPRN_DAC1     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_DAC2     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_DBCR0    )    = REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                         ;
    PPCREGATTR(spr, SPRN_DBCR1    )    = REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                         ; 
    PPCREGATTR(spr, SPRN_DBCR2    )    = REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                         ;
    PPCREGATTR(spr, SPRN_DBSR     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_DEAR     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_DEC      )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_DECAR    )    = REG_WRITE_SUP                                                          ;
    PPCREGATTR(spr, SPRN_ESR      )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IAC1     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IAC2     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR0    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR1    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR2    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR3    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR4    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR5    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR6    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
#if CORE_TYPE != e500v2
    PPCREGATTR(spr, SPRN_IVOR7    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
#endif
    PPCREGATTR(spr, SPRN_IVOR8    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
#if CORE_TYPE != e500v2
    PPCREGATTR(spr, SPRN_IVOR9    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
#endif
    PPCREGATTR(spr, SPRN_IVOR10   )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR11   )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR12   )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR13   )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR14   )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVOR15   )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_IVPR     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_LR       )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR | REG_WRITE_USR          ;
    PPCREGATTR(spr, SPRN_PID      )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_PIR      )    = REG_READ_SUP                                                           ;
    PPCREGATTR(spr, SPRN_PVR      )    = REG_READ_SUP                                                           ;
    PPCREGATTR(spr, SPRN_SPRG0    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG1    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG2    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG3R   )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_SPRG3    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG4R   )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_SPRG4    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG5R   )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_SPRG5    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG6R   )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_SPRG6    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SPRG7R   )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_SPRG7    )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SRR0     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_SRR1     )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_TBRL     )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_TBWL     )    = REG_WRITE_SUP                                                          ;
    PPCREGATTR(spr, SPRN_TBRU     )    = REG_READ_USR                                                           ;
    PPCREGATTR(spr, SPRN_TBWU     )    = REG_WRITE_SUP                                                          ;
    PPCREGATTR(spr, SPRN_TCR      )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_TSR      )    = REG_READ_SUP  | REG_WRITE_SUP                                          ;
    PPCREGATTR(spr, SPRN_USPRG0   )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR         ;
    PPCREGATTR(spr, SPRN_XER      )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR         ;

    PPCREGATTR(spr, SPRN_BBEAR    )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR   | REG_REQ_SYNC  ;
    PPCREGATTR(spr, SPRN_BBTAR    )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR   | REG_REQ_SYNC  ;
    PPCREGATTR(spr, SPRN_BUCSR    )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_HID0     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_HID1     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_IVOR32   )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_IVOR33   )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_IVOR34   )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_IVOR35   )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_L1CFG0   )    = REG_READ_SUP                                                                     ;
    PPCREGATTR(spr, SPRN_L1CFG1   )    = REG_READ_SUP                                                                     ;
    PPCREGATTR(spr, SPRN_L1CSR0   )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_L1CSR1   )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS0     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS1     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS2     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS3     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS4     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS5     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;    // Needs to be verified
    PPCREGATTR(spr, SPRN_MAS6     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MAS7     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_MCAR     )    = REG_READ_SUP                                                                     ;
    PPCREGATTR(spr, SPRN_MCSR     )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_MCSRR0   )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_MCSRR1   )    = REG_READ_SUP  | REG_WRITE_SUP                                                    ;
    PPCREGATTR(spr, SPRN_MMUCFG   )    = REG_READ_SUP                                                                     ;
    PPCREGATTR(spr, SPRN_MMUCSR0  )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_PID0     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_PID1     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_PID2     )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_SPEFSCR  )    = REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                    ;
    PPCREGATTR(spr, SPRN_SVR      )    = REG_READ_SUP                                                                     ;
    PPCREGATTR(spr, SPRN_TLB0CFG  )    = REG_READ_SUP                                                                     ;
    PPCREGATTR(spr, SPRN_TLB1CFG  )    = REG_READ_SUP                                                                     ;

#undef PPCREGATTR
}                              

// Interrupt handling routines ( they handle exceptions at hardware level only and redirect control
//                               to appropriate ISR. ).
// @args :
//     exception_nr -> exception number
//     subtype -> a flag denoting the event which caused the exception
//     ea -> effective address at the time fault occured ( used in case of DSI faults etc )
//
// FIXME : We don't support hardware exceptions yet. This is planned.
void CPU_PPC::ppc_exception(int exception_nr, uint64_t subtype=0, uint64_t ea)
{
    LOG("DEBUG4") << MSG_FUNC_START;

    uint64_t *srr0 = NULL;
    uint64_t *srr1 = NULL;

#define GET_PC_FROM_IVOR_NUM(ivor_num)                          \
    ((PPCREG(REG_IVPR) & 0xffff) << 16) | ((PPCREG(REG_IVOR##ivor_num) & 0xfff) << 4)
#define CLR_DEFAULT_MSR_BITS()                                  \
    PPCREG(REG_MSR) &= ~MSR_SPE & ~MSR_WE & ~MSR_EE & ~MSR_PR & ~MSR_FP & ~MSR_FE0 & ~MSR_FE1 & ~MSR_IS & ~MSR_DS

    switch(exception_nr){
        case  PPC_EXCEPTION_CR:
            if((PPCREG(REG_MSR) & MSR_CE) == 0){ RETURNVOID(DEBUG4); }

            PPCREG(REG_CSRR0) = pc;
            PPCREG(REG_CSRR1) = PPCREG(REG_MSR);
           
            // Clear default MSR bits. Also clear CE bit 
            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_CE;
            pc = GET_PC_FROM_IVOR_NUM(0);
            break;
        case  PPC_EXCEPTION_MC:
            if((PPCREG(REG_MSR) & MSR_ME) == 0){
                std::cerr << "Received Machine Check and MSR[ME]=0. Going into checkstop." << std::endl;
                exit(1);
            }
            
            PPCREG(REG_MCSRR0) = pc;
            PPCREG(REG_MCSRR1) = PPCREG(REG_MSR);
            PPCREG(REG_MCAR)   = ea;

            // If no cause specified, skip straight away
            if(subtype != 0ULL){

                // Check for sub types
                if((subtype & PPC_EXCEPT_MC_DCPERR) == PPC_EXCEPT_MC_DCPERR){
                    PPCREG(REG_MCSR) |= MCSR_DCPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RAERR) == PPC_EXCEPT_MC_BUS_RAERR){
                    PPCREG(REG_MCSR) |= MCSR_BUS_RAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RBERR) == PPC_EXCEPT_MC_BUS_RBERR){
                    PPCREG(REG_MCSR) |= MCSR_BUS_RBERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RPERR) == PPC_EXCEPT_MC_BUS_RPERR){
                    PPCREG(REG_MCSR) |= MCSR_BUS_RPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_WAERR) == PPC_EXCEPT_MC_BUS_WAERR){
                    PPCREG(REG_MCSR) |= MCSR_BUS_WAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_WBERR) == PPC_EXCEPT_MC_BUS_WBERR){
                    PPCREG(REG_MCSR) |= MCSR_BUS_WBERR;
                }
                if((subtype & PPC_EXCEPT_MC_DCP_PERR) == PPC_EXCEPT_MC_DCP_PERR){
                    PPCREG(REG_MCSR) |= MCSR_DCP_PERR;
                }
                if((subtype & PPC_EXCEPT_MC_ICPERR) == PPC_EXCEPT_MC_ICPERR){
                    PPCREG(REG_MCSR) |= MCSR_ICPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IAERR) == PPC_EXCEPT_MC_BUS_IAERR){
                    PPCREG(REG_MCSR) |= MCSR_BUS_IAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IBERR) == PPC_EXCEPT_MC_BUS_IBERR){
                     PPCREG(REG_MCSR) |= MCSR_BUS_IBERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IPERR) == PPC_EXCEPT_MC_BUS_IPERR){
                        PPCREG(REG_MCSR) |= MCSR_BUS_IPERR;
                }
            }

            // Clear default MSR bits.
            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_ME;
            pc = GET_PC_FROM_IVOR_NUM(1);
            break;
        case  PPC_EXCEPTION_DSI:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);
            PPCREG(REG_DEAR) = ea;
            PPCREG(REG_ESR) = 0;   // Clear ESR first

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DSI_ACS_W) == PPC_EXCEPT_DSI_ACS_W){
                    PPCREG(REG_ESR) |= ESR_ST;
                }
                if((subtype & PPC_EXCEPT_DSI_CL) == PPC_EXCEPT_DSI_CL){
                    PPCREG(REG_ESR) |= ESR_DLK;
                }
                if((subtype & PPC_EXCEPT_DSI_BO) == PPC_EXCEPT_DSI_BO){
                    PPCREG(REG_ESR) |= ESR_BO;
                }
            }

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(2); 
            break;
        case  PPC_EXCEPTION_ISI:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);
            PPCREG(REG_ESR) = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_ISI_BO) == PPC_EXCEPT_ISI_BO){
                    PPCREG(REG_ESR) |= ESR_BO;
                }
                if((subtype & PPC_EXCEPT_ISI_ACS) == PPC_EXCEPT_ISI_ACS){
                    // No bit is said to be affected for this in e500 core RM
                }
            }

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(3);
            break;
        case  PPC_EXCEPTION_EI:
            if((PPCREG(REG_MSR) & MSR_EE) == 0){ RETURNVOID(DEBUG4); }
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();  // MSR[EE] is also cleared by this.
            pc = GET_PC_FROM_IVOR_NUM(4);
            break;
        case  PPC_EXCEPTION_ALIGN:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);
            PPCREG(REG_DEAR) = ea;
            PPCREG(REG_ESR)  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_ALIGN_SPE) == PPC_EXCEPT_ALIGN_SPE){
                    PPCREG(REG_ESR) |= ESR_SPV;
                }
                if((subtype & PPC_EXCEPT_ALIGN_ST) == PPC_EXCEPT_ALIGN_ST){
                    PPCREG(REG_ESR) |= ESR_ST;
                }
            }

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(5);
            break;
        case  PPC_EXCEPTION_PRG:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);
            PPCREG(REG_ESR)  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_PRG_ILG) == PPC_EXCEPT_PRG_ILG){
                    PPCREG(REG_ESR) |= ESR_PIL;
                }
                if((subtype & PPC_EXCEPT_PRG_PRV) == PPC_EXCEPT_PRG_PRV){
                    PPCREG(REG_ESR) |= ESR_PPR;
                }
                if((subtype & PPC_EXCEPT_PRG_TRAP) == PPC_EXCEPT_PRG_TRAP){
                    PPCREG(REG_ESR) |= ESR_PTR;
                }
            }
      
            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(6);
            break;
        case  PPC_EXCEPTION_FPU:
            // Is FPU there in e500v2 ??
            break;
        case  PPC_EXCEPTION_SC:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(8);
            break;
        case  PPC_EXCEPTION_DEC:
            // A decrementer intr. occurs when no higher priority exception exists ,
            // a decrementer exception exists (TSR[DIS] = 1), and the interrupt is
            // enabled (TCR[DIE] =1 and MSR[EE] = 1)
            if((PPCREG(REG_TSR) & TSR_DIS) && (PPCREG(REG_TCR) & TCR_DIE) && (PPCREG(REG_MSR) & MSR_EE)){
                // Do nothing
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_TSR) |= TSR_DIS; // Why the hell, am I doing it ?? TSR[DIS] is already set. Isn't it ?
            pc = GET_PC_FROM_IVOR_NUM(10);
            break;
        case  PPC_EXCEPTION_FIT:
            // A fixed interval timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[FIS] = 1) and the interrupt is enabled
            // (TCR[FIE] = 1 and MSR[EE] = 1)
            if((PPCREG(REG_TSR) & TSR_FIS) && (PPCREG(REG_TCR) & TCR_FIE) && (PPCREG(REG_MSR) & MSR_EE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_TSR) |= TSR_FIS;
            pc = GET_PC_FROM_IVOR_NUM(11);
            break;
        case  PPC_EXCEPTION_WTD:
            // A watchdog timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[WIS] = 1) and the interrupt is enabled
            // (TCR[WIE] = 1 and MSR[CE] = 1)
            if((PPCREG(REG_TSR) & TSR_WIS) && (PPCREG(REG_TCR) & TCR_WIE) && (PPCREG(REG_MSR) & MSR_CE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCREG(REG_CSRR0) = pc;
            PPCREG(REG_CSRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_CE;               // Clear CE bit, since WDT is critical type
            PPCREG(REG_TSR) |= TSR_WIS;
            pc = GET_PC_FROM_IVOR_NUM(12);
            break;
        case  PPC_EXCEPTION_DTLB:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);
            PPCREG(REG_DEAR) = ea;
            PPCREG(REG_ESR)  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DTLB_MISS_ST) == PPC_EXCEPT_DTLB_MISS_ST){
                    PPCREG(REG_ESR) |= ESR_ST;
                }
            }

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(13);
            break;
        case  PPC_EXCEPTION_ITLB:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(14);
            break;
        case  PPC_EXCEPTION_DBG:
            // First check if DBCR0[IDM] is set then check if MSR[DE] is set
            if((PPCREG(REG_DBCR0) & DBCR0_IDM) == 0){ RETURNVOID(DEBUG4); }
            if((PPCREG(REG_MSR) & MSR_DE) == 0){ RETURNVOID(DEBUG4); }

            // Debug exceptions are of "CRITICAL" type in e500v2, but "DEBUG" type in e500mc and later
#ifdef CONFIG_E500
            srr0 = &PPCREG(REG_CSRR0);
            srr1 = &PPCREG(REG_CSRR1);
#else
            srr0 = &PPCREG(REG_DSRR0);
            srr1 = &PPCREG(REG_DSRR0);
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
                    PPCREG(REG_DBSR) |= DBSR_TIE;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC1) == PPC_EXCEPT_DBG_IAC1){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_IAC1;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC2) == PPC_EXCEPT_DBG_IAC2){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_IAC2;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1R) == PPC_EXCEPT_DBG_DAC1R){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC1R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1W) == PPC_EXCEPT_DBG_DAC1W){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC1W;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2R) == PPC_EXCEPT_DBG_DAC2R){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC2R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2W) == PPC_EXCEPT_DBG_DAC2W){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC2W;
                }
                if((subtype & PPC_EXCEPT_DBG_ICMP) == PPC_EXCEPT_DBG_ICMP){
                    *srr0 = pc + 4;
                    PPCREG(REG_DBSR) |= DBSR_IC;
                }
                if((subtype & PPC_EXCEPT_DBG_BRT) == PPC_EXCEPT_DBG_BRT){
                    *srr0 = pc;
                    PPCREG(REG_DBSR) |= DBSR_BT;
                }
                if((subtype & PPC_EXCEPT_DBG_RET) == PPC_EXCEPT_DBG_RET){
                    *srr0 = pc + 4;
                    PPCREG(REG_DBSR) |= DBSR_RET;
                }
                if((subtype & PPC_EXCEPT_DBG_IRPT) == PPC_EXCEPT_DBG_IRPT){
                    //srr0 = intr vector
                    PPCREG(REG_DBSR) |= DBSR_IRPT;
                }
                if((subtype & PPC_EXCEPT_DBG_UDE) == PPC_EXCEPT_DBG_UDE){
                    *srr0 = pc + 4;
                    PPCREG(REG_DBSR) |= DBSR_UDE;
                }
            }
            
            *srr1 = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_DE;               // Clear DE bit
            pc = GET_PC_FROM_IVOR_NUM(15);
            break;
        case  PPC_EXCEPTION_SPE_UA:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            PPCREG(REG_ESR)  = ESR_SPV;              // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(32);
            break;
        case  PPC_EXCEPTION_EM_FP_D:
            // Check conditions
            if(((PPCREG(REG_SPEFSCR) & SPEFSCR_FINVE) != 0) && (((PPCREG(REG_SPEFSCR) & SPEFSCR_FINVH) != 0) ||
                        ((PPCREG(REG_SPEFSCR) & SPEFSCR_FINV) != 0))){
                goto skip_0;
            }
            if(((PPCREG(REG_SPEFSCR) & SPEFSCR_FDBZE) != 0) && (((PPCREG(REG_SPEFSCR) & SPEFSCR_FDBZH) != 0) ||
                        ((PPCREG(REG_SPEFSCR) & SPEFSCR_FDBZ) != 0))){
                goto skip_0;
            }
            if(((PPCREG(REG_SPEFSCR) & SPEFSCR_FUNFE) != 0) && (((PPCREG(REG_SPEFSCR) & SPEFSCR_FUNFH) != 0) ||
                        ((PPCREG(REG_SPEFSCR) & SPEFSCR_FUNF) != 0))){
                goto skip_0;
            }
            if(((PPCREG(REG_SPEFSCR) & SPEFSCR_FOVFE) != 0) && (((PPCREG(REG_SPEFSCR) & SPEFSCR_FOVFH) != 0) ||
                        ((PPCREG(REG_SPEFSCR) & SPEFSCR_FOVF) != 0))){
                goto skip_0;
            }
            RETURNVOID(DEBUG4);

            skip_0:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            PPCREG(REG_ESR)  = ESR_SPV;  // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            pc = GET_PC_FROM_IVOR_NUM(33);
            break;
        case  PPC_EXCEPTION_EM_FP_R:
            // Check conditions
            if(((PPCREG(REG_SPEFSCR) & SPEFSCR_FINXE) != 0) &&
                   (((PPCREG(REG_SPEFSCR) & SPEFSCR_FGH) != 0) ||
                   ((PPCREG(REG_SPEFSCR) & SPEFSCR_FXH) != 0)  ||
                   ((PPCREG(REG_SPEFSCR) & SPEFSCR_FG) != 0)  ||
                   ((PPCREG(REG_SPEFSCR) & SPEFSCR_FX) != 0))){
                goto skip_1;
            }
            if(((PPCREG(REG_SPEFSCR) & SPEFSCR_FRMC) == 0x2) || ((PPCREG(REG_SPEFSCR) & SPEFSCR_FRMC) == 0x3)){
                goto skip_1;
            }
            RETURNVOID(DEBUG4);

            skip_1:
            PPCREG(REG_SRR0) = pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            PPCREG(REG_ESR) = ESR_SPV;   // Set ESR[SPE]

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
 * @func  : get_instr()
 * @args  : none
 *
 * @brief : Read instruction at next NIP
 */
instr_call CPU_PPC::get_instr(){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::pair<uint64_t, uint8_t> res;           // pair of <ra, wimge>
    instr_call call_this;
    uint8_t  perm = TO_RWX(1, 0, 1);            // rx = 0b11
    bool as = EBMASK(PPCREG(REG_MSR), MSR_IS);  // as = MSR[IS]
    bool pr = EBMASK(PPCREG(REG_MSR), MSR_PR);  // pr = MSR[pr]

    // Try hits with PID0, PID1 and PID2
    res = m_l2tlb.xlate(pc, as, PPCREG(REG_PID0), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0;
    res = m_l2tlb.xlate(pc, as, PPCREG(REG_PID1), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0; 
    res = m_l2tlb.xlate(pc, as, PPCREG(REG_PID2), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0;

    // We encountered ITLB miss. Throw exceptions
    std::cout << "ITLB miss" << std::endl;
    LTHROW(sim_except_ppc(PPC_EXCEPTION_ITLB, PPC_EXCEPT_ITLB_MISS, "ITLB miss."), DEBUG4);

    exit_loop_0:
    LOG("DEBUG4") << std::hex << std::showbase << "instr Xlation : " << pc << " -> " << res.first << std::endl;

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    // Disassemble the instr at curr pc
    call_this = m_dis.disasm(m_mem_ptr->read32(res.first, (res.second & 0x1)), pc, (res.second & 0x1));

    // Next instr ptr
    nip = (pc + 4);

    LOG("DEBUG4") << MSG_FUNC_END;
    return call_this;
}

/*
 * @func  : check_for_dbg_events
 * @args  : flags ( bit mask of various events ), ea ( used only for DAC events )
 *
 * @brief : flags signal for various debug events
 */
void CPU_PPC::check_for_dbg_events(int flags, uint64_t ea){
    bool event_occurred = 0;
    uint64_t event_type = 0;
    uint64_t event_addr = 0;

    // Check for IAC event
    if(flags & DBG_EVENT_IAC){
        event_occurred= 0;
        // DBCR1[IAC12M] is not implemented right now
        if(PPCREGMASK(REG_DBCR0, DBCR0_IAC1)
           &&
           (PPCREG(REG_IAC1) == pc)
           &&
           (
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC1US) == 2) && !PPCREGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC1US) == 3) && PPCREGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC1US) != 2) && (PPCREGMASK(REG_DBCR1, DBCR1_IAC1US) != 3))
           )
           &&
           (
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC1ER) == 2) && !PPCREGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC1ER) == 3) && PPCREGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC1ER) != 2) && (PPCREGMASK(REG_DBCR1, DBCR1_IAC1ER) != 3))
           )
          ){
            event_occurred = 1;
            event_type     = PPC_EXCEPT_DBG_IAC1;
            event_addr     = pc;
        }
        // DBSR[IAC12M] isn't implemented right now
        if(PPCREGMASK(REG_DBCR0, DBCR0_IAC2)
           &&
           (PPCREG(REG_IAC2) == pc)
           &&
           (
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC2US) == 2) && !PPCREGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC2US) == 3) && PPCREGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC2US) != 2) && (PPCREGMASK(REG_DBCR1, DBCR1_IAC2US) != 3))
           )
           &&
           (
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC2ER) == 2) && !PPCREGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC2ER) == 3) && PPCREGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCREGMASK(REG_DBCR1, DBCR1_IAC2ER) != 2) && (PPCREGMASK(REG_DBCR1, DBCR1_IAC2ER) != 3))
           )
          ){
            event_occurred = 1;
            event_type     = PPC_EXCEPT_DBG_IAC2;
            event_addr     = pc;
        }
    }

    // final steps
    if(event_occurred){
        ppc_exception(PPC_EXCEPTION_DBG, event_type, event_addr);
        if(PPCREGMASK(REG_DBCR0, DBCR0_EDM)){
            // FIXME: Fix this ( if applicable )
            //PPCREG(REG_EDBSR0) |= EDBSR0_XXX;
            // Do some additional steps
            LTHROW(sim_except_ppc_halt("Cpu halted due to debug exception."), DEBUG4);
        }
    }
}

/*
 * @func : run current instr
 */
inline void CPU_PPC::run_curr_instr(){
    LOG("DEBUG4") << MSG_FUNC_START;

    /* Get Instr call frame at next NIP */
    instr_call call_this = get_instr();
    LOG("DEBUG4") << "INSTR : " << call_this.get_instr_str() << std::endl;

    if(m_bm.check_pc(pc)){
        // Throw a software breakpoint exception
        LTHROW(sim_except(SIM_EXCEPT_SBKPT, "Software breakpoint"), DEBUG4);
    }

    // Check for any debug events for IAC
    // FIXME : This may not work at this time
    check_for_dbg_events(DBG_EVENT_IAC);
 
    pc += 4; /* Increment pc just before executing instr */
    /* If there is a func pointer already registered, call it */
    if(call_this.fptr){ (reinterpret_cast<CPU_PPC::ppc_opc_fun_ptr>(call_this.fptr))(this, &call_this); }
 
    LASSERT_THROW(ppc_func_hash.find(call_this.opc) != ppc_func_hash.end(),
            sim_except(SIM_EXCEPT_EINVAL, "Invalid/Unimplemented opcode " + call_this.opcname), DEBUG4);
    call_this.fptr = reinterpret_cast<void*>(ppc_func_hash[call_this.opc]);
    /* call handler function for this call frame */ 
    ppc_func_hash[call_this.opc](this, &call_this);
    ninstrs++;

    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : init_reghash
 * @args : none
 *
 * @brief : Initialize register pointers' hash for easy accessibility
 */
void CPU_PPC::init_reghash(){
     LOG("DEBUG4") << MSG_FUNC_START;

    m_reghash["msr"]        = &(regs.msr);                   m_ireghash[REG_MSR]         = m_reghash["msr"];
    m_reghash["cr"]         = &(regs.cr);                    m_ireghash[REG_CR]          = m_reghash["cr"];
    m_reghash["fpscr"]      = &(regs.fpscr);                 m_ireghash[REG_FPSCR]       = m_reghash["fpscr"];

    m_reghash["atbl"]       = &(regs.spr[SPRN_ATBL]);        m_ireghash[REG_ATBL]        = m_reghash["atbl"];
    m_reghash["atbu"]       = &(regs.spr[SPRN_ATBU]);        m_ireghash[REG_ATBU]        = m_reghash["atbu"];
    m_reghash["csrr0"]      = &(regs.spr[SPRN_CSRR0]);       m_ireghash[REG_CSRR0]       = m_reghash["csrr0"];
    m_reghash["csrr1"]      = &(regs.spr[SPRN_CSRR1]);       m_ireghash[REG_CSRR1]       = m_reghash["csrr1"];
    m_reghash["ctr"]        = &(regs.spr[SPRN_CTR]);         m_ireghash[REG_CTR]         = m_reghash["ctr"];
    m_reghash["dac1"]       = &(regs.spr[SPRN_DAC1]);        m_ireghash[REG_DAC1]        = m_reghash["dac1"];
    m_reghash["dac2"]       = &(regs.spr[SPRN_DAC2]);        m_ireghash[REG_DAC2]        = m_reghash["dac2"];
    m_reghash["dbcr0"]      = &(regs.spr[SPRN_DBCR0]);       m_ireghash[REG_DBCR0]       = m_reghash["dbcr0"];
    m_reghash["dbcr1"]      = &(regs.spr[SPRN_DBCR1]);       m_ireghash[REG_DBCR1]       = m_reghash["dbcr1"];
    m_reghash["dbcr2"]      = &(regs.spr[SPRN_DBCR2]);       m_ireghash[REG_DBCR2]       = m_reghash["dbcr2"];
    m_reghash["dbsr"]       = &(regs.spr[SPRN_DBSR]);        m_ireghash[REG_DBSR]        = m_reghash["dbsr"];
    m_reghash["dear"]       = &(regs.spr[SPRN_DEAR]);        m_ireghash[REG_DEAR]        = m_reghash["dear"];
    m_reghash["dec"]        = &(regs.spr[SPRN_DEC]);         m_ireghash[REG_DEC]         = m_reghash["dec"];
    m_reghash["decar"]      = &(regs.spr[SPRN_DECAR]);       m_ireghash[REG_DECAR]       = m_reghash["decar"];
    m_reghash["esr"]        = &(regs.spr[SPRN_ESR]);         m_ireghash[REG_ESR]         = m_reghash["esr"];
    m_reghash["iac1"]       = &(regs.spr[SPRN_IAC1]);        m_ireghash[REG_IAC1]        = m_reghash["iac1"];
    m_reghash["iac2"]       = &(regs.spr[SPRN_IAC2]);        m_ireghash[REG_IAC2]        = m_reghash["iac2"];
    m_reghash["ivor0"]      = &(regs.spr[SPRN_IVOR0]);       m_ireghash[REG_IVOR0]       = m_reghash["ivor0"];
    m_reghash["ivor1"]      = &(regs.spr[SPRN_IVOR1]);       m_ireghash[REG_IVOR1]       = m_reghash["ivor1"]; 
    m_reghash["ivor2"]      = &(regs.spr[SPRN_IVOR2]);       m_ireghash[REG_IVOR2]       = m_reghash["ivor2"];
    m_reghash["ivor3"]      = &(regs.spr[SPRN_IVOR3]);       m_ireghash[REG_IVOR3]       = m_reghash["ivor3"];
    m_reghash["ivor4"]      = &(regs.spr[SPRN_IVOR4]);       m_ireghash[REG_IVOR4]       = m_reghash["ivor4"];
    m_reghash["ivor5"]      = &(regs.spr[SPRN_IVOR5]);       m_ireghash[REG_IVOR5]       = m_reghash["ivor5"];
    m_reghash["ivor6"]      = &(regs.spr[SPRN_IVOR6]);       m_ireghash[REG_IVOR6]       = m_reghash["ivor6"];
    m_reghash["ivor7"]      = &(regs.spr[SPRN_IVOR7]);       m_ireghash[REG_IVOR7]       = m_reghash["ivor7"];
    m_reghash["ivor8"]      = &(regs.spr[SPRN_IVOR8]);       m_ireghash[REG_IVOR8]       = m_reghash["ivor8"];
    m_reghash["ivor9"]      = &(regs.spr[SPRN_IVOR9]);       m_ireghash[REG_IVOR9]       = m_reghash["ivor9"];
    m_reghash["ivor10"]     = &(regs.spr[SPRN_IVOR10]);      m_ireghash[REG_IVOR10]      = m_reghash["ivor10"];
    m_reghash["ivor11"]     = &(regs.spr[SPRN_IVOR11]);      m_ireghash[REG_IVOR11]      = m_reghash["ivor11"];
    m_reghash["ivor12"]     = &(regs.spr[SPRN_IVOR12]);      m_ireghash[REG_IVOR12]      = m_reghash["ivor12"];
    m_reghash["ivor13"]     = &(regs.spr[SPRN_IVOR13]);      m_ireghash[REG_IVOR13]      = m_reghash["ivor13"];
    m_reghash["ivor14"]     = &(regs.spr[SPRN_IVOR14]);      m_ireghash[REG_IVOR14]      = m_reghash["ivor14"];
    m_reghash["ivor15"]     = &(regs.spr[SPRN_IVOR15]);      m_ireghash[REG_IVOR15]      = m_reghash["ivor15"];
    m_reghash["ivpr"]       = &(regs.spr[SPRN_IVPR]);        m_ireghash[REG_IVPR]        = m_reghash["ivpr"];
    m_reghash["lr"]         = &(regs.spr[SPRN_LR]);          m_ireghash[REG_LR]          = m_reghash["lr"];
    m_reghash["pid"]        = &(regs.spr[SPRN_PID]);         m_ireghash[REG_PID]         = m_reghash["pid"];
    m_reghash["pir"]        = &(regs.spr[SPRN_PIR]);         m_ireghash[REG_PIR]         = m_reghash["pir"];
    m_reghash["pvr"]        = &(regs.spr[SPRN_PVR]);         m_ireghash[REG_PVR]         = m_reghash["pvr"];
    m_reghash["sprg0"]      = &(regs.spr[SPRN_SPRG0]);       m_ireghash[REG_SPRG0]       = m_reghash["sprg0"];
    m_reghash["sprg1"]      = &(regs.spr[SPRN_SPRG1]);       m_ireghash[REG_SPRG1]       = m_reghash["sprg1"];
    m_reghash["sprg2"]      = &(regs.spr[SPRN_SPRG2]);       m_ireghash[REG_SPRG2]       = m_reghash["sprg2"];
    m_reghash["sprg3r"]     = &(regs.spr[SPRN_SPRG3R]);      m_ireghash[REG_SPRG3R]      = m_reghash["sprg3r"];
    m_reghash["sprg3"]      = &(regs.spr[SPRN_SPRG3]);       m_ireghash[REG_SPRG3]       = m_reghash["sprg3"];
    m_reghash["sprg4r"]     = &(regs.spr[SPRN_SPRG4R]);      m_ireghash[REG_SPRG4R]      = m_reghash["sprg4r"];
    m_reghash["sprg4"]      = &(regs.spr[SPRN_SPRG4]);       m_ireghash[REG_SPRG4]       = m_reghash["sprg4"];
    m_reghash["sprg5r"]     = &(regs.spr[SPRN_SPRG5R]);      m_ireghash[REG_SPRG5R]      = m_reghash["sprg5r"];
    m_reghash["sprg5"]      = &(regs.spr[SPRN_SPRG5]);       m_ireghash[REG_SPRG5]       = m_reghash["sprg5"];
    m_reghash["sprg6r"]     = &(regs.spr[SPRN_SPRG6R]);      m_ireghash[REG_SPRG6R]      = m_reghash["sprg6r"];
    m_reghash["sprg6"]      = &(regs.spr[SPRN_SPRG6]);       m_ireghash[REG_SPRG6]       = m_reghash["sprg6"];
    m_reghash["sprg7r"]     = &(regs.spr[SPRN_SPRG7R]);      m_ireghash[REG_SPRG7R]      = m_reghash["sprg7r"];
    m_reghash["sprg7"]      = &(regs.spr[SPRN_SPRG7]);       m_ireghash[REG_SPRG7]       = m_reghash["sprg7"];
    m_reghash["srr0"]       = &(regs.spr[SPRN_SRR0]);        m_ireghash[REG_SRR0]        = m_reghash["srr0"];
    m_reghash["srr1"]       = &(regs.spr[SPRN_SRR1]);        m_ireghash[REG_SRR1]        = m_reghash["srr1"];
    m_reghash["tbrl"]       = &(regs.spr[SPRN_TBRL]);        m_ireghash[REG_TBRL]        = m_reghash["tbrl"];
    m_reghash["tbwl"]       = &(regs.spr[SPRN_TBWL]);        m_ireghash[REG_TBWL]        = m_reghash["tbwl"];
    m_reghash["tbru"]       = &(regs.spr[SPRN_TBRU]);        m_ireghash[REG_TBRU]        = m_reghash["tbru"];
    m_reghash["tbwu"]       = &(regs.spr[SPRN_TBWU]);        m_ireghash[REG_TBWU]        = m_reghash["tbwu"];
    m_reghash["tcr"]        = &(regs.spr[SPRN_TCR]);         m_ireghash[REG_TCR]         = m_reghash["tcr"];
    m_reghash["tsr"]        = &(regs.spr[SPRN_TSR]);         m_ireghash[REG_TSR]         = m_reghash["tsr"];
    m_reghash["usprg0"]     = &(regs.spr[SPRN_USPRG0]);      m_ireghash[REG_USPRG0]      = m_reghash["usprg0"];
    m_reghash["xer"]        = &(regs.spr[SPRN_XER]);         m_ireghash[REG_XER]         = m_reghash["xer"];

    m_reghash["bbear"]      = &(regs.spr[SPRN_BBEAR]);       m_ireghash[REG_BBEAR]       = m_reghash["bbear"];
    m_reghash["bbtar"]      = &(regs.spr[SPRN_BBTAR]);       m_ireghash[REG_BBTAR]       = m_reghash["bbtar"];
    m_reghash["bucsr"]      = &(regs.spr[SPRN_BUCSR]);       m_ireghash[REG_BUCSR]       = m_reghash["bucsr"];
    m_reghash["hid0"]       = &(regs.spr[SPRN_HID0]);        m_ireghash[REG_HID0]        = m_reghash["hid0"];
    m_reghash["hid1"]       = &(regs.spr[SPRN_HID1]);        m_ireghash[REG_HID1]        = m_reghash["hid1"];
    m_reghash["ivor32"]     = &(regs.spr[SPRN_IVOR32]);      m_ireghash[REG_IVOR32]      = m_reghash["ivor32"];
    m_reghash["ivor33"]     = &(regs.spr[SPRN_IVOR33]);      m_ireghash[REG_IVOR33]      = m_reghash["ivor33"];
    m_reghash["ivor34"]     = &(regs.spr[SPRN_IVOR34]);      m_ireghash[REG_IVOR34]      = m_reghash["ivor34"];
    m_reghash["ivor35"]     = &(regs.spr[SPRN_IVOR35]);      m_ireghash[REG_IVOR35]      = m_reghash["ivor35"];
    m_reghash["l1cfg0"]     = &(regs.spr[SPRN_L1CFG0]);      m_ireghash[REG_L1CFG0]      = m_reghash["l1cfg0"];
    m_reghash["l1cfg1"]     = &(regs.spr[SPRN_L1CFG1]);      m_ireghash[REG_L1CFG1]      = m_reghash["l1cfg1"];
    m_reghash["l1csr0"]     = &(regs.spr[SPRN_L1CSR0]);      m_ireghash[REG_L1CSR0]      = m_reghash["l1csr0"];
    m_reghash["l1csr1"]     = &(regs.spr[SPRN_L1CSR1]);      m_ireghash[REG_L1CSR1]      = m_reghash["l1csr1"];
    m_reghash["mas0"]       = &(regs.spr[SPRN_MAS0]);        m_ireghash[REG_MAS0]        = m_reghash["mas0"];
    m_reghash["mas1"]       = &(regs.spr[SPRN_MAS1]);        m_ireghash[REG_MAS1]        = m_reghash["mas1"];
    m_reghash["mas2"]       = &(regs.spr[SPRN_MAS2]);        m_ireghash[REG_MAS2]        = m_reghash["mas2"];
    m_reghash["mas3"]       = &(regs.spr[SPRN_MAS3]);        m_ireghash[REG_MAS3]        = m_reghash["mas3"];
    m_reghash["mas4"]       = &(regs.spr[SPRN_MAS4]);        m_ireghash[REG_MAS4]        = m_reghash["mas4"];
    m_reghash["mas5"]       = &(regs.spr[SPRN_MAS5]);        m_ireghash[REG_MAS5]        = m_reghash["mas5"];
    m_reghash["mas6"]       = &(regs.spr[SPRN_MAS6]);        m_ireghash[REG_MAS6]        = m_reghash["mas6"];
    m_reghash["mas7"]       = &(regs.spr[SPRN_MAS7]);        m_ireghash[REG_MAS7]        = m_reghash["mas7"];
    m_reghash["mcar"]       = &(regs.spr[SPRN_MCAR]);        m_ireghash[REG_MCAR]        = m_reghash["mcar"];
    m_reghash["mcsr"]       = &(regs.spr[SPRN_MCSR]);        m_ireghash[REG_MCSR]        = m_reghash["mcsr"];
    m_reghash["mcsrr0"]     = &(regs.spr[SPRN_MCSRR0]);      m_ireghash[REG_MCSRR0]      = m_reghash["mcsrr0"];
    m_reghash["mcsrr1"]     = &(regs.spr[SPRN_MCSRR1]);      m_ireghash[REG_MCSRR1]      = m_reghash["mcsrr1"];
    m_reghash["mmucfg"]     = &(regs.spr[SPRN_MMUCFG]);      m_ireghash[REG_MMUCFG]      = m_reghash["mmucfg"];
    m_reghash["mmucsr0"]    = &(regs.spr[SPRN_MMUCSR0]);     m_ireghash[REG_MMUCSR0]     = m_reghash["mmucsr0"];
    m_reghash["pid0"]       = &(regs.spr[SPRN_PID0]);        m_ireghash[REG_PID0]        = m_reghash["pid0"];
    m_reghash["pid1"]       = &(regs.spr[SPRN_PID1]);        m_ireghash[REG_PID1]        = m_reghash["pid1"];
    m_reghash["pid2"]       = &(regs.spr[SPRN_PID2]);        m_ireghash[REG_PID2]        = m_reghash["pid2"];
    m_reghash["spefscr"]    = &(regs.spr[SPRN_SPEFSCR]);     m_ireghash[REG_SPEFSCR]     = m_reghash["spefscr"];
    m_reghash["svr"]        = &(regs.spr[SPRN_SVR]);         m_ireghash[REG_SVR]         = m_reghash["svr"];
    m_reghash["tlb0cfg"]    = &(regs.spr[SPRN_TLB0CFG]);     m_ireghash[REG_TLB0CFG]     = m_reghash["tlb0cfg"];
    m_reghash["tlb1cfg"]    = &(regs.spr[SPRN_TLB1CFG]);     m_ireghash[REG_TLB1CFG]     = m_reghash["tlb1cfg"];

    // PMRs
    m_reghash["pmc0"]       = &(regs.pmr[PMRN_PMC0]);        m_ireghash[REG_PMC0]        = m_reghash["pmc0"];
    m_reghash["pmc1"]       = &(regs.pmr[PMRN_PMC1]);        m_ireghash[REG_PMC1]        = m_reghash["pmc1"];
    m_reghash["pmc2"]       = &(regs.pmr[PMRN_PMC2]);        m_ireghash[REG_PMC2]        = m_reghash["pmc2"];
    m_reghash["pmc3"]       = &(regs.pmr[PMRN_PMC3]);        m_ireghash[REG_PMC3]        = m_reghash["pmc3"];
    m_reghash["pmlca0"]     = &(regs.pmr[PMRN_PMLCA0]);      m_ireghash[REG_PMLCA0]      = m_reghash["pmlca0"];
    m_reghash["pmlca1"]     = &(regs.pmr[PMRN_PMLCA1]);      m_ireghash[REG_PMLCA1]      = m_reghash["pmlca1"];
    m_reghash["pmlca2"]     = &(regs.pmr[PMRN_PMLCA2]);      m_ireghash[REG_PMLCA2]      = m_reghash["pmlca2"];
    m_reghash["pmlca3"]     = &(regs.pmr[PMRN_PMLCA3]);      m_ireghash[REG_PMLCA3]      = m_reghash["pmlca3"];
    m_reghash["pmlcb0"]     = &(regs.pmr[PMRN_PMLCB0]);      m_ireghash[REG_PMLCB0]      = m_reghash["pmlcb0"];
    m_reghash["pmlcb1"]     = &(regs.pmr[PMRN_PMLCB1]);      m_ireghash[REG_PMLCB1]      = m_reghash["pmlcb1"];
    m_reghash["pmlcb2"]     = &(regs.pmr[PMRN_PMLCB2]);      m_ireghash[REG_PMLCB2]      = m_reghash["pmlcb2"];
    m_reghash["pmlcb3"]     = &(regs.pmr[PMRN_PMLCB3]);      m_ireghash[REG_PMLCB3]      = m_reghash["pmlcb3"];
    m_reghash["pmgc0"]      = &(regs.pmr[PMRN_PMGC0]);       m_ireghash[REG_PMGC0]       = m_reghash["pmgc0"];
    m_reghash["upmc0"]      = &(regs.pmr[PMRN_UPMC0]);       m_ireghash[REG_UPMC0]       = m_reghash["upmc0"];
    m_reghash["upmc1"]      = &(regs.pmr[PMRN_UPMC1]);       m_ireghash[REG_UPMC1]       = m_reghash["upmc1"];
    m_reghash["upmc2"]      = &(regs.pmr[PMRN_UPMC2]);       m_ireghash[REG_UPMC2]       = m_reghash["upmc2"];
    m_reghash["upmc3"]      = &(regs.pmr[PMRN_UPMC3]);       m_ireghash[REG_UPMC3]       = m_reghash["upmc3"];
    m_reghash["upmlca0"]    = &(regs.pmr[PMRN_UPMLCA0]);     m_ireghash[REG_UPMLCA0]     = m_reghash["upmlca0"];
    m_reghash["upmlca1"]    = &(regs.pmr[PMRN_UPMLCA1]);     m_ireghash[REG_UPMLCA1]     = m_reghash["upmlca1"];
    m_reghash["upmlca2"]    = &(regs.pmr[PMRN_UPMLCA2]);     m_ireghash[REG_UPMLCA2]     = m_reghash["upmlca2"];
    m_reghash["upmlca3"]    = &(regs.pmr[PMRN_UPMLCA3]);     m_ireghash[REG_UPMLCA3]     = m_reghash["upmlca3"];
    m_reghash["upmlcb0"]    = &(regs.pmr[PMRN_UPMLCB0]);     m_ireghash[REG_UPMLCB0]     = m_reghash["upmlcb0"];
    m_reghash["upmlcb1"]    = &(regs.pmr[PMRN_UPMLCB1]);     m_ireghash[REG_UPMLCB1]     = m_reghash["upmlcb1"];
    m_reghash["upmlcb2"]    = &(regs.pmr[PMRN_UPMLCB2]);     m_ireghash[REG_UPMLCB2]     = m_reghash["upmlcb2"];
    m_reghash["upmlcb3"]    = &(regs.pmr[PMRN_UPMLCB3]);     m_ireghash[REG_UPMLCB3]     = m_reghash["upmlcb3"];
    m_reghash["upmgc0"]     = &(regs.pmr[PMRN_UPMGC0]);      m_ireghash[REG_UPMGC0]      = m_reghash["upmgc0"];

    // GPRS ad FPRS
    for (size_t i=0; i<PPC_NGPRS; i++){
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "r" << i))->str()] = &(regs.gpr[i]); 
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "gpr" << i))->str()] = &(regs.gpr[i]);
        m_ireghash[REG_GPR0 + i] = &(regs.gpr[i]);
    }
    for (size_t i=0; i<PPC_NFPRS; i++){
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "f" << i))->str()] = &(regs.fpr[i]);
        m_reghash[static_cast<ostringstream *>(&(ostringstream() << "fpr" << i))->str()] = &(regs.fpr[i]);
        m_ireghash[REG_FPR0 + i] = &(regs.fpr[i]);
    }

    // Add more later on
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Notify of context switches. LRU cache uses this parameter to flush it's
// instruction cache when a context switch happens.
void CPU_PPC::notify_ctxt_switch(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_ctxt_switch = 1;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Update CR0
void CPU_PPC::update_cr0(bool use_host, uint64_t value){
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
    c |= ((PPCREG(REG_XER) >> 31) & 1);

    PPCREG(REG_CR) &= ~((uint32_t)0xf << 28);
    PPCREG(REG_CR) |= ((uint32_t)c << 28);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Updates CR[bf] with val i.e CR[bf] <- (val & 0xf)
// bf may range from [0:7]
void CPU_PPC::update_crF(unsigned bf, uint64_t val){
    LOG("DEBUG4") << MSG_FUNC_START;
    bf &= 0x7;
    val &= 0xf;
    PPCREG(REG_CR) &= ~( 0xf << (7 - bf)*4 );
    PPCREG(REG_CR) |=  ((val & 0xf) << (7 - bf)*4);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get crF  ( F -> [0:7] )
unsigned CPU_PPC::get_crF(unsigned bf){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (PPCREG(REG_CR) >> (7 - bf)*4) & 0xf;
}

// Update CR by exact field value [0:31]
void CPU_PPC::update_crf(unsigned field, unsigned value){
    LOG("DEBUG4") << MSG_FUNC_START;
    field &= 0x1f;
    value &= 0x1;
    PPCREG(REG_CR) &= ~(0x1 << (31 - field));
    PPCREG(REG_CR) |= (value << (31 - field));
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get CR bit at exact field
unsigned CPU_PPC::get_crf(unsigned field){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (PPCREG(REG_CR) >> (31 - field)) & 0x1;
}

// Update XER
void CPU_PPC::update_xer(bool use_host, uint64_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint32_t c = 0;
    if(!use_host){
        // Don't use host facilities 
    }else{
        if(host_state.flags & X86_FLAGS_CF){ c = 2; }
        if(host_state.flags & X86_FLAGS_OF){
            c |= 4;
            //if(curr_instr != "mtspr"){   /* If current instruction is not "mtspr", set SO bit also */
            //    c |= 8;
            //}
        }
    }
    /* Set XER */
    PPCREG(REG_XER) &= ~((uint32_t)0xf << 28);
    PPCREG(REG_XER) |= (c << 28);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// bf field can be only 3 bits wide. If not, it's truncated to 3 bits
// val can be 4 bits only.
void CPU_PPC::update_xerF(unsigned bf, unsigned val){
    LOG("DEBUG4") << MSG_FUNC_START;
    bf &= 0x7;
    val &= 0xf;
    PPCREG(REG_XER) &= ~( 0xf << (7 - bf)*4 );
    PPCREG(REG_XER) |=  ((val & 0xf) << (7 - bf)*4);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// value can be either 1 or 0 
void CPU_PPC::update_xerf(unsigned field, unsigned value){
    LOG("DEBUG4") << MSG_FUNC_START;
    field &= 0x1f;
    value &= 0x1;
    PPCREG(REG_XER) &= ~(0x1 << (31 - field));
    PPCREG(REG_XER) |= (value << (31 - field));
    LOG("DEBUG4") << MSG_FUNC_END;
}

unsigned CPU_PPC::get_xerF(unsigned bf){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (PPCREG(REG_XER) >> (7 - bf)*4) & 0xf;
}

unsigned CPU_PPC::get_xerf(unsigned field){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return (PPCREG(REG_XER) >> (31 - field)) & 0x1;
}

// Get XER[SO]
unsigned CPU_PPC::get_xer_so(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return ((PPCREG(REG_XER) & XER_SO) ? 1:0);
}

unsigned CPU_PPC::get_xer_ca(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return ((PPCREG(REG_XER) & XER_CA) ? 1:0);
}

// Get register value by name
uint64_t CPU_PPC::get_reg(std::string reg_name) throw(sim_except) {
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_reghash.find(reg_name) == m_reghash.end()) throw sim_except(SIM_EXCEPT_EINVAL, "Illegal register name");
    LOG("DEBUG4") << MSG_FUNC_END;
    return PPCREGN(reg_name);
}

// Dump CPU state
void CPU_PPC::dump_state(int columns, std::ostream &ostr, int dump_all_sprs){
    LOG("DEBUG4") << MSG_FUNC_START;
    int i;
    int colno = 0;
    ostringstream strout;

    // dump msr and cr
    ostr << BAR0 << std::endl;
    ostr << "CPU STATE" << std::endl;
    ostr << BAR0 << std::endl;
    ostr << "cpu no = " << (int)cpu_no << std::endl;
    ostr << "msr = " << std::hex << std::showbase << PPCREG(REG_MSR) << std::endl;
    ostr << "cr  = " << std::hex << std::showbase << PPCREG(REG_CR) << std::endl;
    ostr << "iar = " << std::hex << std::showbase << pc << std::endl;
    ostr << std::endl;

    // dump gprs
    for(i=0; i<PPC_NGPRS; i++){
        strout << "r" << std::dec << i << " = " << std::hex << std::showbase << PPCREG(REG_GPR0 + i);
        ostr << std::left << std::setw(23) << strout.str() << "    ";
        strout.str("");
        if(++colno >= columns){ ostr << std::endl; colno = 0; }
    }
    ostr << std::endl;
    ostr << std::endl;
    colno = 0;

    // dump fprs
    for(i=0; i<PPC_NFPRS; i++){
        strout << "f" << std::dec << i << " = " << std::hex << std::showbase << PPCREG(REG_FPR0 + i);
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
            strout << "spr[" << std::dec << i << "] = " << std::hex << std::showbase << PPCREG(REG_SPR0 + i);
            ostr << std::left << std::setw(26) << strout.str() << "    ";
            strout.str("");
            if(++colno >= columns){ ostr << std::endl; colno = 0; }
        }
        ostr << std::endl;
        colno = 0;
    }else{
        ostr << std::hex << std::showbase;
        ostr << "ctr = " << PPCREG(REG_CTR) << std::endl;
        ostr << "xer = " << PPCREG(REG_XER) << std::endl;
        ostr << "lr  = " << PPCREG(REG_LR) << std::endl;
        ostr << std::endl;

        ostr << "mas0 = " << PPCREG(REG_MAS0) << std::endl;
        ostr << "mas1 = " << PPCREG(REG_MAS1) << std::endl;
        ostr << "mas2 = " << PPCREG(REG_MAS2) << std::endl;
        ostr << "mas3 = " << PPCREG(REG_MAS3) << std::endl;
        ostr << "mas4 = " << PPCREG(REG_MAS4) << std::endl;
        ostr << "mas5 = " << PPCREG(REG_MAS5) << std::endl;
        ostr << "mas6 = " << PPCREG(REG_MAS6) << std::endl;
        ostr << "mas7 = " << PPCREG(REG_MAS7) << std::endl;

    }
    ostr << BAR0 << std::endl;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// print all tlbs
void CPU_PPC::print_L2tlbs(){
    m_l2tlb.print_tlbs2();
}

// friend function
void gen_ppc_opc_func_hash(CPU_PPC *pcpu){
    #include "cpu_ppc_instr.inc"
}

#endif    /*  CPU_PPC_H  */
