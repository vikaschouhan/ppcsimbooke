#include "cpu_ppc.h"

// --------------------------- STATIC DATA ---------------------------------------------------

CPU_T size_t                         CPU_PPC_T::sm_ncpus = 0;            // Current number of powerpc cpus
CPU_T Log<1>                         CPU_PPC_T::sm_instr_tracer;         // Instruction tracer
CPU_T std::map<uint64_t,
    std::pair<bool, uint8_t> >       CPU_PPC_T::sm_resv_map;             // This keeps track of global reservation map


// CPU Member function definitions -----------------------------------

// Default constructor
CPU_T CPU_PPC_T::CPU_PPC(): m_cache_line_size(CPU_CACHE_LINE_SIZE){
    LOG("DEBUG4") << MSG_FUNC_START;
    init_common();
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T CPU_PPC_T::CPU_PPC(uint64_t cpuid, std::string name):
    m_cpu_name(name), m_cache_line_size(CPU_CACHE_LINE_SIZE), m_pc(0xfffffffc), m_cpu_id(cpuid){
    LOG("DEBUG4") << MSG_FUNC_START;
    init_common(); 
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T CPU_PPC_T::~CPU_PPC(){
    LOG("DEBUG4") << MSG_FUNC_START;
    sm_ncpus--;
    LOG("DEBUG4") << MSG_FUNC_END;
} 

CPU_T void CPU_PPC_T::init(uint64_t cpuid, std::string name){    // Initialize CPU
    m_cpu_id   = cpuid;
    m_cpu_name = name;
    m_pc       = 0xfffffffc;
}

CPU_T void CPU_PPC_T::register_mem(memory &mem){
    if(m_mem_ptr == NULL)
        m_mem_ptr = &mem;
}

CPU_T size_t CPU_PPC_T::get_ninstrs(){
    return m_ninstrs;
}

CPU_T uint64_t CPU_PPC_T::get_pc(){
    return m_pc;
}

// run (non blocking)
CPU_T void CPU_PPC_T::run(){
    LOG("DEBUG4") << MSG_FUNC_START;
    boost::thread thr0(&CPU_PPC_T::run_b, this);

    // Try to rethrow the exception from daughter thread
    if(sim_except_ptr)
        std::rethrow_exception(sim_except_ptr);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// step operation
CPU_T void CPU_PPC_T::step(size_t instr_cnt){
    LOG("DEBUG4") << MSG_FUNC_START;

    size_t t=0;
    if(!instr_cnt) return;

    std::pair<uint64_t, bool> last_bkpt = m_bm.last_breakpoint();
    m_cpu_mode = CPU_MODE_STEPPING;

    clear_ctrs();

    // run this instruction if this was last breakpointed
    if(last_bkpt.first == m_pc and last_bkpt.second == true){
        m_bm.disable_breakpoints();
        run_curr_instr();
        instr_cnt--;                  // decrement instr count
        m_bm.clear_last_breakpoint();
        m_bm.enable_breakpoints();
    }

#define I     run_curr_instr()

    for(t=0; t<instr_cnt; t++){
        try{
            I;
        }
        catch(sim_except_ppc& e){
            sim_except_ppc(e.err_code0(), e.err_code1(), e.addr());
        }
    }
    m_cpu_mode = CPU_MODE_STOPPED;

    m_ninstrs += m_ninstrs_last;
#undef I
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::halt(){
    LOG("DEBUG4") << MSG_FUNC_START;
    // FIXME: should change it under mutex control
    m_cpu_mode = CPU_MODE_HALTED;
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::stop(){
    LOG("DEBUG4") << MSG_FUNC_START;
    // FIXME: should change it under mutex control
    m_cpu_mode = CPU_MODE_STOPPED;
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::run_mode(){
    LOG("DEBUG4") << MSG_FUNC_START;
    switch(m_cpu_mode){
        case CPU_MODE_RUNNING  : std::cout << "Running"  << std::endl; break;
        case CPU_MODE_STEPPING : std::cout << "Stepping" << std::endl; break;
        case CPU_MODE_HALTED   : std::cout << "Halted"   << std::endl; break;
        case CPU_MODE_STOPPED  : std::cout << "Stopped"  << std::endl; break;
        default                : std::cout << "Unknown"  << std::endl; break;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

// virtual form of run_instr
// Seems like c++ doesn't have an very effective way of handling non POD objects
//  with variadic arg funcs
CPU_T void CPU_PPC_T::run_instr(std::string instr){
    LOG("DEBUG4") << MSG_FUNC_START;
    instr_call call_this;

    call_this = m_dis.disasm(instr, m_pc);
    m_ppc_func_hash.at(call_this.opc)(this, &call_this);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Overloaded form of run_instr ( specifically for CPU_PPC )
CPU_T void CPU_PPC_T::run_instr(uint32_t opcd){
    LOG("DEBUG4") << MSG_FUNC_START;
    instr_call call_this;

    call_this = m_dis.disasm(opcd, m_pc);
    m_ppc_func_hash.at(call_this.opc)(this, &call_this);
    LOG("DEBUG4") << MSG_FUNC_END;
}

#define TO_RWX(r, w, x) (((r & 0x1) << 2) | ((w & 0x1) << 1) | (x & 0x1))
// Translate EA to RA ( for data only )
// NOTE: All exceptions ( hardware/software ) will be handled at run_instr() or run() level.
CPU_T std::pair<uint64_t, uint8_t> CPU_PPC_T::xlate(uint64_t addr, bool wr){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::pair<uint64_t, uint8_t> res;
    uint8_t  perm = (wr) ? TO_RWX(0, 1, 0) : TO_RWX(1, 0, 0);
    bool as = EBF(PPCREG(REG_MSR), MSR_DS);
    bool pr = EBF(PPCREG(REG_MSR), MSR_PR);

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

// Get register pointer using regid
// TODO : Check Permissions
CPU_T inline ppc_reg64* CPU_PPC_T::reg(int regid){
    return m_cpu_regs.m_ireg.at(regid);
}

// Get register pointer using reg name
CPU_T inline ppc_reg64* CPU_PPC_T::regn(std::string regname){
    return m_cpu_regs.m_reg.at(regname);
}

// Memory I/O functions
CPU_T uint8_t CPU_PPC_T::read8(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);
 
    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read8(res.first, (res.second & 0x1));
}

CPU_T void CPU_PPC_T::write8(uint64_t addr, uint8_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write8(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T uint16_t CPU_PPC_T::read16(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read16(res.first, (res.second & 0x1));
}


CPU_T void CPU_PPC_T::write16(uint64_t addr, uint16_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write16(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T uint32_t CPU_PPC_T::read32(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read32(res.first, (res.second & 0x1));
}

CPU_T void CPU_PPC_T::write32(uint64_t addr, uint32_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write32(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T uint64_t CPU_PPC_T::read64(uint64_t addr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 0);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_mem_ptr->read64(res.first, (res.second & 0x1));
}

CPU_T void CPU_PPC_T::write64(uint64_t addr, uint64_t value){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(addr, 1);

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write64(res.first, value, (res.second & 0x1));
    LOG("DEBUG4") << MSG_FUNC_END;
}


// Interrupt handling routines ( they handle exceptions at hardware level only and redirect control
//                               to appropriate ISR. ).
// @args :
//     exception_nr -> exception number
//     subtype -> a flag denoting the event which caused the exception
//     ea -> effective address at the time fault occured ( used in case of DSI faults etc )
//
// FIXME : We don't support hardware exceptions yet. This is planned.
CPU_T void CPU_PPC_T::ppc_exception(int exception_nr, uint64_t subtype, uint64_t ea)
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

            PPCREG(REG_CSRR0) = m_pc;
            PPCREG(REG_CSRR1) = PPCREG(REG_MSR);
           
            // Clear default MSR bits. Also clear CE bit 
            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_CE;
            m_pc = GET_PC_FROM_IVOR_NUM(0);
            break;
        case  PPC_EXCEPTION_MC:
            if((PPCREG(REG_MSR) & MSR_ME) == 0){
                std::cerr << "Received Machine Check and MSR[ME]=0. Going into checkstop." << std::endl;
                exit(1);
            }
            
            PPCREG(REG_MCSRR0) = m_pc;
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
            m_pc = GET_PC_FROM_IVOR_NUM(1);
            break;
        case  PPC_EXCEPTION_DSI:
            PPCREG(REG_SRR0) = m_pc;
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
            m_pc = GET_PC_FROM_IVOR_NUM(2); 
            break;
        case  PPC_EXCEPTION_ISI:
            PPCREG(REG_SRR0) = m_pc;
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
            m_pc = GET_PC_FROM_IVOR_NUM(3);
            break;
        case  PPC_EXCEPTION_EI:
            if((PPCREG(REG_MSR) & MSR_EE) == 0){ RETURNVOID(DEBUG4); }
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();  // MSR[EE] is also cleared by this.
            m_pc = GET_PC_FROM_IVOR_NUM(4);
            break;
        case  PPC_EXCEPTION_ALIGN:
            PPCREG(REG_SRR0) = m_pc;
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
            m_pc = GET_PC_FROM_IVOR_NUM(5);
            break;
        case  PPC_EXCEPTION_PRG:
            PPCREG(REG_SRR0) = m_pc;
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
            m_pc = GET_PC_FROM_IVOR_NUM(6);
            break;
        case  PPC_EXCEPTION_FPU:
            // Is FPU there in e500v2 ??
            break;
        case  PPC_EXCEPTION_SC:
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            m_pc = GET_PC_FROM_IVOR_NUM(8);
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
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_TSR) |= TSR_DIS; // Why the hell, am I doing it ?? TSR[DIS] is already set. Isn't it ?
            m_pc = GET_PC_FROM_IVOR_NUM(10);
            break;
        case  PPC_EXCEPTION_FIT:
            // A fixed interval timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[FIS] = 1) and the interrupt is enabled
            // (TCR[FIE] = 1 and MSR[EE] = 1)
            if((PPCREG(REG_TSR) & TSR_FIS) && (PPCREG(REG_TCR) & TCR_FIE) && (PPCREG(REG_MSR) & MSR_EE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_TSR) |= TSR_FIS;
            m_pc = GET_PC_FROM_IVOR_NUM(11);
            break;
        case  PPC_EXCEPTION_WTD:
            // A watchdog timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[WIS] = 1) and the interrupt is enabled
            // (TCR[WIE] = 1 and MSR[CE] = 1)
            if((PPCREG(REG_TSR) & TSR_WIS) && (PPCREG(REG_TCR) & TCR_WIE) && (PPCREG(REG_MSR) & MSR_CE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCREG(REG_CSRR0) = m_pc;
            PPCREG(REG_CSRR1) = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_CE;               // Clear CE bit, since WDT is critical type
            PPCREG(REG_TSR) |= TSR_WIS;
            m_pc = GET_PC_FROM_IVOR_NUM(12);
            break;
        case  PPC_EXCEPTION_DTLB:
            PPCREG(REG_SRR0) = m_pc;
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
            m_pc = GET_PC_FROM_IVOR_NUM(13);
            break;
        case  PPC_EXCEPTION_ITLB:
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            m_pc = GET_PC_FROM_IVOR_NUM(14);
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
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_TIE;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC1) == PPC_EXCEPT_DBG_IAC1){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_IAC1;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC2) == PPC_EXCEPT_DBG_IAC2){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_IAC2;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1R) == PPC_EXCEPT_DBG_DAC1R){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC1R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1W) == PPC_EXCEPT_DBG_DAC1W){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC1W;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2R) == PPC_EXCEPT_DBG_DAC2R){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC2R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2W) == PPC_EXCEPT_DBG_DAC2W){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_DAC2W;
                }
                if((subtype & PPC_EXCEPT_DBG_ICMP) == PPC_EXCEPT_DBG_ICMP){
                    *srr0 = m_pc + 4;
                    PPCREG(REG_DBSR) |= DBSR_IC;
                }
                if((subtype & PPC_EXCEPT_DBG_BRT) == PPC_EXCEPT_DBG_BRT){
                    *srr0 = m_pc;
                    PPCREG(REG_DBSR) |= DBSR_BT;
                }
                if((subtype & PPC_EXCEPT_DBG_RET) == PPC_EXCEPT_DBG_RET){
                    *srr0 = m_pc + 4;
                    PPCREG(REG_DBSR) |= DBSR_RET;
                }
                if((subtype & PPC_EXCEPT_DBG_IRPT) == PPC_EXCEPT_DBG_IRPT){
                    //srr0 = intr vector
                    PPCREG(REG_DBSR) |= DBSR_IRPT;
                }
                if((subtype & PPC_EXCEPT_DBG_UDE) == PPC_EXCEPT_DBG_UDE){
                    *srr0 = m_pc + 4;
                    PPCREG(REG_DBSR) |= DBSR_UDE;
                }
            }
            
            *srr1 = PPCREG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCREG(REG_MSR) &= ~MSR_DE;               // Clear DE bit
            m_pc = GET_PC_FROM_IVOR_NUM(15);
            break;
        case  PPC_EXCEPTION_SPE_UA:
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            PPCREG(REG_ESR)  = ESR_SPV;              // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            m_pc = GET_PC_FROM_IVOR_NUM(32);
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
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            PPCREG(REG_ESR)  = ESR_SPV;  // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            m_pc = GET_PC_FROM_IVOR_NUM(33);
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
            PPCREG(REG_SRR0) = m_pc;
            PPCREG(REG_SRR1) = PPCREG(REG_MSR);

            PPCREG(REG_ESR) = ESR_SPV;   // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            m_pc = GET_PC_FROM_IVOR_NUM(34);
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
CPU_T instr_call CPU_PPC_T::get_instr(){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::pair<uint64_t, uint8_t> res;           // pair of <ra, wimge>
    instr_call call_this;

    call_this = m_instr_cache[m_pc];

    if(!m_instr_cache.error()){
        LOG("DEBUG4") << MSG_FUNC_END;
        return call_this;
    }

    uint8_t  perm = TO_RWX(1, 0, 1);            // rx = 0b11
    bool as = EBF(PPCREG(REG_MSR), MSR_IS);  // as = MSR[IS]
    bool pr = EBF(PPCREG(REG_MSR), MSR_PR);  // pr = MSR[pr]

    // Try hits with PID0, PID1 and PID2
    res = m_l2tlb.xlate(m_pc, as, PPCREG(REG_PID0), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0;
    res = m_l2tlb.xlate(m_pc, as, PPCREG(REG_PID1), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0; 
    res = m_l2tlb.xlate(m_pc, as, PPCREG(REG_PID2), perm, pr); if(res.first != static_cast<uint64_t>(-1)) goto exit_loop_0;

    // We encountered ITLB miss. Throw exceptions
    std::cout << "ITLB miss" << std::endl;
    LTHROW(sim_except_ppc(PPC_EXCEPTION_ITLB, PPC_EXCEPT_ITLB_MISS, "ITLB miss."), DEBUG4);

    exit_loop_0:
    LOG("DEBUG4") << std::hex << std::showbase << "instr Xlation : " << m_pc << " -> " << res.first << std::endl;

    LASSERT_THROW(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    // Disassemble the instr at curr pc
    call_this = m_dis.disasm(m_mem_ptr->read32(res.first, (res.second & 0x1)), m_pc, (res.second & 0x1));

    // Update instruction cache.
    m_instr_cache[m_pc] = call_this;

    LOG("DEBUG4") << MSG_FUNC_END;
    return call_this;
}

/*
 * @func  : check_for_dbg_events
 * @args  : flags ( bit mask of various events ), ea ( used only for DAC events )
 *
 * @brief : flags signal for various debug events
 */
CPU_T void CPU_PPC_T::check_for_dbg_events(int flags, uint64_t ea){
    bool event_occurred = 0;
    uint64_t event_type = 0;
    uint64_t event_addr = 0;

    // Check for IAC event
    if(flags & DBG_EVENT_IAC){
        event_occurred= 0;
        // DBCR1[IAC12M] is not implemented right now
        if(PPCREGMASK(REG_DBCR0, DBCR0_IAC1)
           &&
           (PPCREG(REG_IAC1) == m_pc)
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
            event_addr     = m_pc;
        }
        // DBSR[IAC12M] isn't implemented right now
        if(PPCREGMASK(REG_DBCR0, DBCR0_IAC2)
           &&
           (PPCREG(REG_IAC2) == m_pc)
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
            event_addr     = m_pc;
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

// Blocking run
CPU_T void CPU_PPC_T::run_b(){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::pair<uint64_t, bool> last_bkpt = m_bm.last_breakpoint();
    m_cpu_mode = CPU_MODE_RUNNING;
    static const int n_instrs_per_pass = 100;
    int i;

    // get first timing & clear all counters
    m_prev_stamp = boost::posix_time::microsec_clock::local_time();
    clear_ctrs();

    // run this instruction if this was last breakpointed
    if(last_bkpt.first == m_pc and last_bkpt.second == true){
        m_bm.disable_breakpoints();
        run_curr_instr();
        m_bm.clear_last_breakpoint();
        m_bm.enable_breakpoints();
    }

#define I  run_curr_instr() 
        
    // Catch all non-hardware exceptions & save them into sim_except_ptr
    // to be thrown later.
    try {
        for(;;){
            // Observe each instruction for possible exceptions
            try {
                // Run this much instructions before checking for other conditions
                for(i=0; i<n_instrs_per_pass; i++){
                    I;
                }
            }
            catch(sim_except_ppc& e){
                ppc_exception(e.err_code0(), e.err_code1(), e.addr());
            }

            // If running status is changed to stopped/halted, exit out of loop
            if unlikely(m_cpu_mode == CPU_MODE_HALTED or m_cpu_mode == CPU_MODE_STOPPED){
                goto loop_exit_0;
            }
        }
    }
    catch(...){
        sim_except_ptr = std::current_exception();
    }

    loop_exit_0:
    m_cpu_mode = CPU_MODE_STOPPED;

    m_ninstrs += m_ninstrs_last;  // update total instrs cnt

    // get current time & calculate the diff
    m_next_stamp = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration t_d = m_next_stamp - m_prev_stamp;

    // calculate instrs/sec
    double instrs_per_sec = (static_cast<double>(m_ninstrs_last)/t_d.total_microseconds()) * 1000;
    std::cout << "Average Speed = " << instrs_per_sec << " KIPS." << std::endl;

#undef I
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T inline void CPU_PPC_T::clear_ctrs(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_ninstrs_last = 0;
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : run current instr
 */
CPU_T inline void CPU_PPC_T::run_curr_instr(){
    LOG("DEBUG4") << MSG_FUNC_START;

    m_nip += 4;   // Update NIP

    /* Get Instr call frame at next NIP */
    instr_call call_this = get_instr();
    LOG("DEBUG4") << "INSTR : " << call_this.get_instr_str() << std::endl;

    // Trace instructions
    //sm_instr_tracer("DEBUG") << "[CPU_" << (int)m_cpu_no << std::hex << "]\t" << "PC: 0x" << m_pc << "\t" << call_this.get_instr_str() << std::endl;
    // Log coverage
    m_cov_logger.probe(call_this.opcname);

    if(m_bm.check_pc(m_pc)){
        // Throw a software breakpoint exception
        LTHROW(sim_except(SIM_EXCEPT_SBKPT, "Software breakpoint"), DEBUG4);
    }

    // Do this while stepping
    if(m_cpu_mode == CPU_MODE_STEPPING){
        std::cout << std::hex << "PC:" <<  m_pc << " [ " << call_this.get_instr_str() << " ]" << std::endl;
    }

    // Check for any debug events for IAC
    // FIXME : This may not work at this time
    check_for_dbg_events(DBG_EVENT_IAC);
 
    /* call handler function for this call frame */ 
    m_ppc_func_hash.at(call_this.opc)(this, &call_this);

    // book-keeping
    m_pc = m_nip;     // Update PC
    m_ninstrs_last++;

    LOG("DEBUG4") << MSG_FUNC_END;
}

// Initialize all common parameters
CPU_T inline void CPU_PPC_T::init_common(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cpu_no = sm_ncpus++;                 // Increment global cpu cnt
    gen_ppc_opc_func_hash(this);           // Initialize opcode function pointer table
    m_instr_cache.set_size(4096);          // LRU cache size = 4096 instrs
    m_ctxt_switch = 0;                     // Initialize flag to zero
    m_cpu_mode = CPU_MODE_HALTED;
    m_ncycles = 0;

    // Init logging facilities
    std::ostringstream ostr;
    ostr << "cpu_" << int(m_cpu_no) << "_cov.log";
    m_cov_logger.log_to_file(ostr.str());
    
    LOG("DEBUG4") << MSG_FUNC_END;
}


// set reservation
CPU_T void CPU_PPC_T::set_resv(uint64_t ea, size_t size){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(ea, 0);
    m_resv_addr = res.first;
    m_resv_set  = true;
    m_resv_size = size;
    // We need a mutex here
    sm_resv_map[m_resv_addr & ~(m_cache_line_size - 1)] = std::make_pair(true, m_cpu_no);  // Global reservation always act on resv granules
    LOG("DEBUG4") << MSG_FUNC_END;
}

// clear resv.
CPU_T void CPU_PPC_T::clear_resv(uint64_t ea){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_resv_set = false;
    // Need a mutex
    sm_resv_map[m_resv_addr & ~(m_cache_line_size - 1)].first = false;
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Check resv
CPU_T bool CPU_PPC_T::check_resv(uint64_t ea, size_t size){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::pair<uint64_t, uint8_t> res = xlate(ea, 1);  // Reservation is checked during stwcx.
    uint64_t caddr = res.first & ~(m_cache_line_size - 1);    // Get granule addr
    if(sm_resv_map.find(caddr) == sm_resv_map.end()){
        return false;
    }
    if(res.first == m_resv_addr and m_resv_set == true and m_resv_size == size and
            sm_resv_map[caddr].first == true and sm_resv_map[caddr].second == m_cpu_no){
        LOG("DEBUG4") << MSG_FUNC_END;
        return true;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return false;
}

// Notify of context switches. LRU cache uses this parameter to flush it's
// instruction cache when a context switch happens.
CPU_T void CPU_PPC_T::notify_ctxt_switch(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_ctxt_switch = 1;
    m_instr_cache.clear();
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get register value by name
CPU_T uint64_t CPU_PPC_T::get_reg(std::string reg_name) throw(sim_except) {
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_cpu_regs.m_reg.find(reg_name) == m_cpu_regs.m_reg.end()) throw sim_except(SIM_EXCEPT_EINVAL, "Illegal register name");
    LOG("DEBUG4") << MSG_FUNC_END;
    return PPCREGN(reg_name);
}

// Dump CPU state
CPU_T void CPU_PPC_T::dump_state(int columns, std::ostream &ostr, int dump_all_sprs){
    LOG("DEBUG4") << MSG_FUNC_START;
    int i;
    int colno = 0;
    std::ostringstream strout;

    // dump msr and cr
    ostr << BAR0 << std::endl;
    ostr << "CPU STATE" << std::endl;
    ostr << BAR0 << std::endl;
    ostr << "cpu no = " << (int)m_cpu_no << std::endl;
    ostr << "msr = " << std::hex << std::showbase << PPCREG(REG_MSR) << std::endl;
    ostr << "cr  = " << std::hex << std::showbase << PPCREG(REG_CR) << std::endl;
    ostr << "iar = " << std::hex << std::showbase << m_pc << std::endl;
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

    // dump sprs
    if(dump_all_sprs){
        // FIXME: fix this at some point in future.
        //
        //for(i=0; i<PPC_NSPRS; i++){
        //    // TODO: do a check for valid sprs later on
        //    strout << "spr[" << std::dec << i << "] = " << std::hex << std::showbase << PPCREG(REG_SPR0 + i);
        //    ostr << std::left << std::setw(26) << strout.str() << "    ";
        //    strout.str("");
        //    if(++colno >= columns){ ostr << std::endl; colno = 0; }
        //}
        //ostr << std::endl;
        //colno = 0;
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
CPU_T void CPU_PPC_T::print_L2tlbs(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_l2tlb.print_tlbs2();
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::enable_cov_log(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_logger.enable();
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::disable_cov_log(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_logger.disable();
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T bool CPU_PPC_T::is_cov_log_enabled(){
    LOG("DEBUG4") << MSG_FUNC_START;
    return m_cov_logger.is_enabled();
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::gen_cov_log(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_logger.add_ext_info("###############################################################");
    m_cov_logger.add_ext_info("  COVERAGE FOR CPU " + boost::lexical_cast<std::string>(int(m_cpu_no)));
    m_cov_logger.add_ext_info("###############################################################\n");
    m_cov_logger.generate_log();
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_T void CPU_PPC_T::log_cov_to_file(std::string filename){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_logger.log_to_file(filename);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// friend function
CPU_T void gen_ppc_opc_func_hash(CPU_PPC_T *pcpu){
    #include "cpu_ppc_instr.cc"
}
