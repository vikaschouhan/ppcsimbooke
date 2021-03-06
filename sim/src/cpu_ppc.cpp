#include "cpu_ppc.h"

// --------------------------- STATIC DATA ---------------------------------------------------

size_t                         ppcsimbooke::ppcsimbooke_cpu::cpu::sm_ncpus = 0;            // Current number of powerpc cpus
Log<1>                         ppcsimbooke::ppcsimbooke_cpu::cpu::sm_instr_tracer;         // Instruction tracer
std::map<uint64_t, std::pair<bool, uint8_t> >
                               ppcsimbooke::ppcsimbooke_cpu::cpu::sm_resv_map;             // This keeps track of global reservation map


// CPU Member function definitions -----------------------------------

// Default constructor
ppcsimbooke::ppcsimbooke_cpu::cpu::cpu(): m_cache_line_size(CPU_CACHE_LINE_SIZE){
    LOG_DEBUG4(MSG_FUNC_START);
    init_common();
    LOG_DEBUG4(MSG_FUNC_END);
}

ppcsimbooke::ppcsimbooke_cpu::cpu::cpu(uint64_t cpuid, std::string name):
    m_cpu_name(name), m_cache_line_size(CPU_CACHE_LINE_SIZE), m_cpu_id(cpuid){
    LOG_DEBUG4(MSG_FUNC_START);
    init_common(); 
    LOG_DEBUG4(MSG_FUNC_END);
}

ppcsimbooke::ppcsimbooke_cpu::cpu::~cpu(){
    LOG_DEBUG4(MSG_FUNC_START);
    sm_ncpus--;
    LOG_DEBUG4(MSG_FUNC_END);
} 

void ppcsimbooke::ppcsimbooke_cpu::cpu::init(uint64_t cpuid, std::string name){    // Initialize CPU
    m_cpu_id   = cpuid;
    m_cpu_name = name;
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::register_mem(ppcsimbooke_memory::memory &mem){
    if(m_mem_ptr == NULL)
        m_mem_ptr = &mem;
}

size_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_ninstrs(){
    return m_ninstrs;
}

uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_pc(){
    return PPCSIMBOOKE_CPU_PC;
}

uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_nip(){
    return PPCSIMBOOKE_CPU_NIP;
}

uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_bits(){
    return m_cpu_bits;
}

uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_pc_mask(){
    return (m_cpu_bits == 32) ? 0xffffffffULL : 0xffffffffffffffffULL;
}

// NOTE : There won't be any support for these in basic block based
//        execution.
//        1. Breakpoints.
//
// TODO:
// Exceptions are not supported at this time. They will be added later on
void ppcsimbooke::ppcsimbooke_cpu::cpu::run_basic_blocks_b(){
    LOG_DEBUG4(MSG_FUNC_START);

    // change cpu mode to running
    m_cpu_mode = CPU_MODE_RUNNING;
    static const size_t n_basic_blocks_per_pass = 50;

    // get first timing & clear all counters
    m_prev_stamp = boost::posix_time::microsec_clock::local_time();
    clear_ctrs();

    ppcsimbooke::ppcsimbooke_basic_block::basic_block* bb = NULL;

    // Catch all non-hardware exceptions & save them into sim_except_ptr
    // to be thrown later.
    try {
        for(;;){
            // Observe each instruction for possible exceptions
            try {
                // Run this much instructions before checking for other conditions
                for(size_t i=0; i<n_basic_blocks_per_pass; i++){
                    bb = m_bb_cache_unit.translate(*this);
                    bb->run(*this);
                    //std::cout << *bb << std::endl;
                    m_ninstrs_last += bb->transopscount;
                }
            }
            // FIXME : TODO : Exceptions are not supported at this time. This catch block
            //                is here just for sake of completeness.
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
}

// run (non blocking)
void ppcsimbooke::ppcsimbooke_cpu::cpu::run(){
    LOG_DEBUG4(MSG_FUNC_START);

    switch(m_cpu_exec_mode){
        case CPU_EXEC_MODE_INTERPRETIVE : { boost::thread thr0(&ppcsimbooke::ppcsimbooke_cpu::cpu::run_b, this); }
                                          break;
        case CPU_EXEC_MODE_THREADED     : { boost::thread thr0(&ppcsimbooke::ppcsimbooke_cpu::cpu::run_basic_blocks_b, this); }
                                          break;
        default                         : LTHROW(sim_except_fatal("Wrong execution mode."), DEBUG4);
    }

    // Try to rethrow the exception from daughter thread
    if(sim_except_ptr)
        std::rethrow_exception(sim_except_ptr);
    LOG_DEBUG4(MSG_FUNC_END);
}

// step operation
void ppcsimbooke::ppcsimbooke_cpu::cpu::step(size_t instr_cnt){
    LOG_DEBUG4(MSG_FUNC_START);

    size_t t=0;
    if(!instr_cnt) return;

    std::pair<uint64_t, bool> last_bkpt = m_bm.last_breakpoint();
    m_cpu_mode = CPU_MODE_STEPPING;

    clear_ctrs();

    // run this instruction if this was last breakpointed
    if(last_bkpt.first == PPCSIMBOOKE_CPU_PC and last_bkpt.second == true){
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
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::halt(){
    LOG_DEBUG4(MSG_FUNC_START);
    // FIXME: should change it under mutex control
    m_cpu_mode = CPU_MODE_HALTED;
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::stop(){
    LOG_DEBUG4(MSG_FUNC_START);
    // FIXME: should change it under mutex control
    m_cpu_mode = CPU_MODE_STOPPED;
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::run_mode(){
    LOG_DEBUG4(MSG_FUNC_START);
    switch(m_cpu_mode){
        case CPU_MODE_RUNNING  : std::cout << "Running"  << std::endl; break;
        case CPU_MODE_STEPPING : std::cout << "Stepping" << std::endl; break;
        case CPU_MODE_HALTED   : std::cout << "Halted"   << std::endl; break;
        case CPU_MODE_STOPPED  : std::cout << "Stopped"  << std::endl; break;
        default                : std::cout << "Unknown"  << std::endl; break;
    }
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::exec_mode(){
    LOG_DEBUG4(MSG_FUNC_START);
    switch(m_cpu_exec_mode){
        case CPU_EXEC_MODE_INTERPRETIVE : std::cout << "Interpretive Mode." << std::endl; break;
        case CPU_EXEC_MODE_THREADED     : std::cout << "Threaded Mode."     << std::endl; break;
        default                         : std::cout << "Unknown Mode."      << std::endl; break;
    }
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::set_exec_mode_interpretive(){
    m_cpu_exec_mode = CPU_EXEC_MODE_INTERPRETIVE;
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::set_exec_mode_threaded(){
    m_cpu_exec_mode = CPU_EXEC_MODE_THREADED;
}

// virtual form of run_instr
// Seems like c++ doesn't have an very effective way of handling non POD objects
//  with variadic arg funcs
void ppcsimbooke::ppcsimbooke_cpu::cpu::run_instr(std::string instr){
    LOG_DEBUG4(MSG_FUNC_START);
    instr_call call_this;

    call_this = m_dis.disasm(instr, PPCSIMBOOKE_CPU_PC);
    m_ppc_func_hash.at(call_this.hv)(this, &call_this);
    LOG_DEBUG4(MSG_FUNC_END);
}

// Overloaded form of run_instr ( specifically for CPU_PPC )
void ppcsimbooke::ppcsimbooke_cpu::cpu::run_instr(uint32_t opcd){
    LOG_DEBUG4(MSG_FUNC_START);
    instr_call call_this;

    call_this = m_dis.disasm(opcd, PPCSIMBOOKE_CPU_PC);
    m_ppc_func_hash.at(call_this.hv)(this, &call_this);
    LOG_DEBUG4(MSG_FUNC_END);
}

// get opcode implementation function
ppcsimbooke::ppcsimbooke_cpu::cpu::ppc_opc_fun_ptr ppcsimbooke::ppcsimbooke_cpu::cpu::get_opc_impl(uint64_t opcode_hash){
    LOG_DEBUG4(MSG_FUNC_START);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_ppc_func_hash.at(opcode_hash);
}

#define TO_RWX(r, w, x) (((r & 0x1) << 2) | ((w & 0x1) << 1) | (x & 0x1))
// Translate EA to RA
// NOTE: All exceptions ( hardware/software ) will be handled at run_instr() or run() level.
ppcsimbooke::ppcsimbooke_cpu::cpu::xlated_tlb_res ppcsimbooke::ppcsimbooke_cpu::cpu::xlate(uint64_t addr, bool wr, bool ex){
    LOG_DEBUG4(MSG_FUNC_START);

    // Check for wr=1, ex=1 on accessed page
    // I am not sure, if there is a ppc exception for this. Change this to throw hardware exception in that case.
    LASSERT_THROW_UNLIKELY(!((wr == true) && (ex == true)), sim_except(SIM_EXCEPT_EINVAL, "WR=1 on instruction page."), DEBUG4);

    xlated_tlb_res res;
    uint8_t  perm = ((wr) ? TO_RWX(0, 1, 0) : TO_RWX(1, 0, 0)) | TO_RWX(0, 0, ex);
    bool as = EBF(PPCSIMBOOKE_CPU_REG(REG_MSR), MSR_DS);
    bool pr = EBF(PPCSIMBOOKE_CPU_REG(REG_MSR), MSR_PR);

    // Try hits with PID0, PID1 and PID2
    res = m_l2tlb.xlate(addr, as, PPCSIMBOOKE_CPU_REG(REG_PID0), perm, pr); if(std::get<0>(res) != static_cast<uint64_t>(-1)) goto exit_loop_0;
    res = m_l2tlb.xlate(addr, as, PPCSIMBOOKE_CPU_REG(REG_PID1), perm, pr); if(std::get<0>(res) != static_cast<uint64_t>(-1)) goto exit_loop_0; 
    res = m_l2tlb.xlate(addr, as, PPCSIMBOOKE_CPU_REG(REG_PID2), perm, pr); if(std::get<0>(res) != static_cast<uint64_t>(-1)) goto exit_loop_0;

    // We encountered TLB miss. Throw exceptions. Also pass the faulting address as part of exception class
    if(ex){
        LOG_DEBUG4("ITLB miss.");
        LTHROW(sim_except_ppc(PPC_EXCEPTION_ITLB, PPC_EXCEPT_ITLB_MISS, "ITLB miss.", addr), DEBUG4);
    }else{
        LOG_DEBUG4("DTLB miss.");
    }

    if(wr){
        LTHROW(sim_except_ppc(PPC_EXCEPTION_DTLB, PPC_EXCEPT_DTLB_MISS_ST, "DTLB miss on store.", addr), DEBUG4);
    }else{
        LTHROW(sim_except_ppc(PPC_EXCEPTION_DTLB, PPC_EXCEPT_DTLB_MISS_LD, "DTLB miss on load.", addr), DEBUG4);
    }

    exit_loop_0:
    LOG_DEBUG4(std::hex, std::showbase, "Xlation : ", addr, " -> ", std::get<0>(res), std::endl);
    LOG_DEBUG4(MSG_FUNC_END);
    return res;
}

// Get register pointer using regid
// TODO : Check Permissions
inline ppcsimbooke::ppc_reg64* ppcsimbooke::ppcsimbooke_cpu::cpu::reg(int regid){
    return m_cpu_regs.m_ireg.at(regid);
}

// Get register pointer using reg name
inline ppcsimbooke::ppc_reg64* ppcsimbooke::ppcsimbooke_cpu::cpu::regn(std::string regname){
    return m_cpu_regs.m_reg.at(regname);
}

// Memory I/O functions
uint8_t ppcsimbooke::ppcsimbooke_cpu::cpu::read8(uint64_t addr){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 0);
 
    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_mem_ptr->read8(std::get<0>(res), (std::get<1>(res) & 0x1));
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::write8(uint64_t addr, uint8_t value){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 1);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write8(std::get<0>(res), value, (std::get<1>(res) & 0x1));
    LOG_DEBUG4(MSG_FUNC_END);
}

uint16_t ppcsimbooke::ppcsimbooke_cpu::cpu::read16(uint64_t addr){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 0);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_mem_ptr->read16(std::get<0>(res), (std::get<1>(res) & 0x1));
}


void ppcsimbooke::ppcsimbooke_cpu::cpu::write16(uint64_t addr, uint16_t value){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 1);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write16(std::get<0>(res), value, (std::get<1>(res) & 0x1));
    LOG_DEBUG4(MSG_FUNC_END);
}

uint32_t ppcsimbooke::ppcsimbooke_cpu::cpu::read32(uint64_t addr){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 0);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_mem_ptr->read32(std::get<0>(res), (std::get<1>(res) & 0x1));
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::write32(uint64_t addr, uint32_t value){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 1);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write32(std::get<0>(res), value, (std::get<1>(res) & 0x1));
    LOG_DEBUG4(MSG_FUNC_END);
}

uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::read64(uint64_t addr){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 0);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_mem_ptr->read64(std::get<0>(res), (std::get<1>(res) & 0x1));
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::write64(uint64_t addr, uint64_t value){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(addr, 1);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    m_mem_ptr->write64(std::get<0>(res), value, (std::get<1>(res) & 0x1));
    LOG_DEBUG4(MSG_FUNC_END);
}

size_t ppcsimbooke::ppcsimbooke_cpu::cpu::read_buff(uint64_t addr, uint8_t* buff, size_t buffsize, bool ex){
    LOG_DEBUG4(MSG_FUNC_START);

    uint64_t ps, pm;
    size_t size_curr_page;
    size_t rem_buffsize = buffsize;
    xlated_tlb_res res;

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    
    while(rem_buffsize > 0){
        // We may encounter tlb misses here
        try{
            res = xlate(addr, 0, ex);      // translate this address
        }catch(sim_except_ppc& e){
            switch(e.err_code<0>()){
                case PPC_EXCEPTION_ITLB :
                case PPC_EXCEPTION_DTLB : break;
                default                 : throw(sim_except_fatal("Unexpected exception in read_buff() !!"));
            }

            LOG_DEBUG4(MSG_FUNC_END);
            return buffsize - rem_buffsize;   // returning data size read
        }

        ps = std::get<2>(res);         // get page_size & page_mask
        pm = ~(ps - 1);
        size_curr_page = min(rem_buffsize, ((addr & pm) + ps - addr));
        m_mem_ptr->read_to_buffer(addr, buff, size_curr_page);
        buff         += size_curr_page;
        rem_buffsize -= size_curr_page;
        addr         += size_curr_page;
    }

    LOG_DEBUG4(MSG_FUNC_END);
    return buffsize;
}


// Interrupt handling routines ( they handle exceptions at hardware level only and redirect control
//                               to appropriate ISR. ).
// @args :
//     exception_nr -> exception number
//     subtype -> a flag denoting the event which caused the exception
//     ea -> effective address at the time fault occured ( used in case of DSI faults etc )
//
// FIXME : We don't support hardware exceptions yet. This is planned.
void ppcsimbooke::ppcsimbooke_cpu::cpu::ppc_exception(int exception_nr, int subtype, uint64_t ea)
{
    LOG_DEBUG4(MSG_FUNC_START);

    uint64_t *srr0 = NULL;
    uint64_t *srr1 = NULL;

#define GET_PC_FROM_IVOR_NUM(ivor_num)                          \
    ((PPCSIMBOOKE_CPU_REG(REG_IVPR) & 0xffff) << 16) | ((PPCSIMBOOKE_CPU_REG(REG_IVOR##ivor_num) & 0xfff) << 4)
#define CLR_DEFAULT_MSR_BITS()                                  \
    PPCSIMBOOKE_CPU_REG(REG_MSR) &= ~MSR_SPE & ~MSR_WE & ~MSR_EE & ~MSR_PR & ~MSR_FP & ~MSR_FE0 & ~MSR_FE1 & ~MSR_IS & ~MSR_DS

    switch(exception_nr){
        case  PPC_EXCEPTION_CR:
            if((PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_CE) == 0){ RETURNVOID(DEBUG4); }

            PPCSIMBOOKE_CPU_REG(REG_CSRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_CSRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
           
            // Clear default MSR bits. Also clear CE bit 
            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_REG(REG_MSR) &= ~MSR_CE;
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(0);
            break;
        case  PPC_EXCEPTION_MC:
            if((PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_ME) == 0){
                std::cerr << "Received Machine Check and MSR[ME]=0. Going into checkstop." << std::endl;
                exit(1);
            }
            
            PPCSIMBOOKE_CPU_REG(REG_MCSRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_MCSRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
            PPCSIMBOOKE_CPU_REG(REG_MCAR)   = ea;

            // If no cause specified, skip straight away
            if(subtype != 0ULL){

                // Check for sub types
                if((subtype & PPC_EXCEPT_MC_DCPERR) == PPC_EXCEPT_MC_DCPERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_DCPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RAERR) == PPC_EXCEPT_MC_BUS_RAERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_RAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RBERR) == PPC_EXCEPT_MC_BUS_RBERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_RBERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_RPERR) == PPC_EXCEPT_MC_BUS_RPERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_RPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_WAERR) == PPC_EXCEPT_MC_BUS_WAERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_WAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_WBERR) == PPC_EXCEPT_MC_BUS_WBERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_WBERR;
                }
                if((subtype & PPC_EXCEPT_MC_DCP_PERR) == PPC_EXCEPT_MC_DCP_PERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_DCP_PERR;
                }
                if((subtype & PPC_EXCEPT_MC_ICPERR) == PPC_EXCEPT_MC_ICPERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_ICPERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IAERR) == PPC_EXCEPT_MC_BUS_IAERR){
                    PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_IAERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IBERR) == PPC_EXCEPT_MC_BUS_IBERR){
                     PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_IBERR;
                }
                if((subtype & PPC_EXCEPT_MC_BUS_IPERR) == PPC_EXCEPT_MC_BUS_IPERR){
                        PPCSIMBOOKE_CPU_REG(REG_MCSR) |= MCSR_BUS_IPERR;
                }
            }

            // Clear default MSR bits.
            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_REG(REG_MSR) &= ~MSR_ME;
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(1);
            break;
        case  PPC_EXCEPTION_DSI:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
            PPCSIMBOOKE_CPU_REG(REG_DEAR) = ea;
            PPCSIMBOOKE_CPU_REG(REG_ESR) = 0;   // Clear ESR first

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DSI_ACS_W) == PPC_EXCEPT_DSI_ACS_W){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_ST;
                }
                if((subtype & PPC_EXCEPT_DSI_CL) == PPC_EXCEPT_DSI_CL){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_DLK;
                }
                if((subtype & PPC_EXCEPT_DSI_BO) == PPC_EXCEPT_DSI_BO){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_BO;
                }
            }

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(2); 
            break;
        case  PPC_EXCEPTION_ISI:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
            PPCSIMBOOKE_CPU_REG(REG_ESR) = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_ISI_BO) == PPC_EXCEPT_ISI_BO){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_BO;
                }
                if((subtype & PPC_EXCEPT_ISI_ACS) == PPC_EXCEPT_ISI_ACS){
                    // No bit is said to be affected for this in e500 core RM
                }
            }

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(3);
            break;
        case  PPC_EXCEPTION_EI:
            if((PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_EE) == 0){ RETURNVOID(DEBUG4); }
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();  // MSR[EE] is also cleared by this.
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(4);
            break;
        case  PPC_EXCEPTION_ALIGN:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
            PPCSIMBOOKE_CPU_REG(REG_DEAR) = ea;
            PPCSIMBOOKE_CPU_REG(REG_ESR)  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_ALIGN_SPE) == PPC_EXCEPT_ALIGN_SPE){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_SPV;
                }
                if((subtype & PPC_EXCEPT_ALIGN_ST) == PPC_EXCEPT_ALIGN_ST){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_ST;
                }
            }

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(5);
            break;
        case  PPC_EXCEPTION_PRG:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
            PPCSIMBOOKE_CPU_REG(REG_ESR)  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_PRG_ILG) == PPC_EXCEPT_PRG_ILG){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_PIL;
                }
                if((subtype & PPC_EXCEPT_PRG_PRV) == PPC_EXCEPT_PRG_PRV){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_PPR;
                }
                if((subtype & PPC_EXCEPT_PRG_TRAP) == PPC_EXCEPT_PRG_TRAP){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_PTR;
                }
            }
      
            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(6);
            break;
        case  PPC_EXCEPTION_FPU:
            // Is FPU there in e500v2 ??
            break;
        case  PPC_EXCEPTION_SC:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(8);
            break;
        case  PPC_EXCEPTION_DEC:
            // A decrementer intr. occurs when no higher priority exception exists ,
            // a decrementer exception exists (TSR[DIS] = 1), and the interrupt is
            // enabled (TCR[DIE] =1 and MSR[EE] = 1)
            if((PPCSIMBOOKE_CPU_REG(REG_TSR) & TSR_DIS) && (PPCSIMBOOKE_CPU_REG(REG_TCR) & TCR_DIE) && (PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_EE)){
                // Do nothing
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_REG(REG_TSR) |= TSR_DIS; // Why the hell, am I doing it ?? TSR[DIS] is already set. Isn't it ?
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(10);
            break;
        case  PPC_EXCEPTION_FIT:
            // A fixed interval timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[FIS] = 1) and the interrupt is enabled
            // (TCR[FIE] = 1 and MSR[EE] = 1)
            if((PPCSIMBOOKE_CPU_REG(REG_TSR) & TSR_FIS) && (PPCSIMBOOKE_CPU_REG(REG_TCR) & TCR_FIE) && (PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_EE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_REG(REG_TSR) |= TSR_FIS;
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(11);
            break;
        case  PPC_EXCEPTION_WTD:
            // A watchdog timer interrupt occurs when no higher priority exception exists,
            // and a FIT exception exists (TSR[WIS] = 1) and the interrupt is enabled
            // (TCR[WIE] = 1 and MSR[CE] = 1)
            if((PPCSIMBOOKE_CPU_REG(REG_TSR) & TSR_WIS) && (PPCSIMBOOKE_CPU_REG(REG_TCR) & TCR_WIE) && (PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_CE)){
            }else{
                RETURNVOID(DEBUG4);
            }
            PPCSIMBOOKE_CPU_REG(REG_CSRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_CSRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_REG(REG_MSR) &= ~MSR_CE;               // Clear CE bit, since WDT is critical type
            PPCSIMBOOKE_CPU_REG(REG_TSR) |= TSR_WIS;
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(12);
            break;
        case  PPC_EXCEPTION_DTLB:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);
            PPCSIMBOOKE_CPU_REG(REG_DEAR) = ea;
            PPCSIMBOOKE_CPU_REG(REG_ESR)  = 0;

            if(subtype != 0ULL){
                if((subtype & PPC_EXCEPT_DTLB_MISS_ST) == PPC_EXCEPT_DTLB_MISS_ST){
                    PPCSIMBOOKE_CPU_REG(REG_ESR) |= ESR_ST;
                }
            }

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(13);
            break;
        case  PPC_EXCEPTION_ITLB:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            // TODO:
            // Load Default MAS* values in MAS registers

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(14);
            break;
        case  PPC_EXCEPTION_DBG:
            // First check if DBCR0[IDM] is set then check if MSR[DE] is set
            if((PPCSIMBOOKE_CPU_REG(REG_DBCR0) & DBCR0_IDM) == 0){ RETURNVOID(DEBUG4); }
            if((PPCSIMBOOKE_CPU_REG(REG_MSR) & MSR_DE) == 0){ RETURNVOID(DEBUG4); }

            // Debug exceptions are of "CRITICAL" type in e500v2, but "DEBUG" type in e500mc and later
#ifdef CONFIG_E500
            srr0 = &PPCSIMBOOKE_CPU_REG(REG_CSRR0);
            srr1 = &PPCSIMBOOKE_CPU_REG(REG_CSRR1);
#else
            srr0 = &PPCSIMBOOKE_CPU_REG(REG_DSRR0);
            srr1 = &PPCSIMBOOKE_CPU_REG(REG_DSRR0);
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
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_TIE;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC1) == PPC_EXCEPT_DBG_IAC1){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_IAC1;
                }
                if((subtype & PPC_EXCEPT_DBG_IAC2) == PPC_EXCEPT_DBG_IAC2){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_IAC2;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1R) == PPC_EXCEPT_DBG_DAC1R){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_DAC1R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC1W) == PPC_EXCEPT_DBG_DAC1W){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_DAC1W;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2R) == PPC_EXCEPT_DBG_DAC2R){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_DAC2R;
                }
                if((subtype & PPC_EXCEPT_DBG_DAC2W) == PPC_EXCEPT_DBG_DAC2W){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_DAC2W;
                }
                if((subtype & PPC_EXCEPT_DBG_ICMP) == PPC_EXCEPT_DBG_ICMP){
                    *srr0 = PPCSIMBOOKE_CPU_PC + 4;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_IC;
                }
                if((subtype & PPC_EXCEPT_DBG_BRT) == PPC_EXCEPT_DBG_BRT){
                    *srr0 = PPCSIMBOOKE_CPU_PC;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_BT;
                }
                if((subtype & PPC_EXCEPT_DBG_RET) == PPC_EXCEPT_DBG_RET){
                    *srr0 = PPCSIMBOOKE_CPU_PC + 4;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_RET;
                }
                if((subtype & PPC_EXCEPT_DBG_IRPT) == PPC_EXCEPT_DBG_IRPT){
                    //srr0 = intr vector
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_IRPT;
                }
                if((subtype & PPC_EXCEPT_DBG_UDE) == PPC_EXCEPT_DBG_UDE){
                    *srr0 = PPCSIMBOOKE_CPU_PC + 4;
                    PPCSIMBOOKE_CPU_REG(REG_DBSR) |= DBSR_UDE;
                }
            }
            
            *srr1 = PPCSIMBOOKE_CPU_REG(REG_MSR);

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_REG(REG_MSR) &= ~MSR_DE;               // Clear DE bit
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(15);
            break;
        case  PPC_EXCEPTION_SPE_UA:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            PPCSIMBOOKE_CPU_REG(REG_ESR)  = ESR_SPV;              // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(32);
            break;
        case  PPC_EXCEPTION_EM_FP_D:
            // Check conditions
            if(((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FINVE) != 0) && (((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FINVH) != 0) ||
                        ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FINV) != 0))){
                goto skip_0;
            }
            if(((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FDBZE) != 0) && (((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FDBZH) != 0) ||
                        ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FDBZ) != 0))){
                goto skip_0;
            }
            if(((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FUNFE) != 0) && (((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FUNFH) != 0) ||
                        ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FUNF) != 0))){
                goto skip_0;
            }
            if(((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FOVFE) != 0) && (((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FOVFH) != 0) ||
                        ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FOVF) != 0))){
                goto skip_0;
            }
            RETURNVOID(DEBUG4);

            skip_0:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            PPCSIMBOOKE_CPU_REG(REG_ESR)  = ESR_SPV;  // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(33);
            break;
        case  PPC_EXCEPTION_EM_FP_R:
            // Check conditions
            if(((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FINXE) != 0) &&
                   (((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FGH) != 0) ||
                   ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FXH) != 0)  ||
                   ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FG) != 0)  ||
                   ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FX) != 0))){
                goto skip_1;
            }
            if(((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FRMC) == 0x2) || ((PPCSIMBOOKE_CPU_REG(REG_SPEFSCR) & SPEFSCR_FRMC) == 0x3)){
                goto skip_1;
            }
            RETURNVOID(DEBUG4);

            skip_1:
            PPCSIMBOOKE_CPU_REG(REG_SRR0) = PPCSIMBOOKE_CPU_PC;
            PPCSIMBOOKE_CPU_REG(REG_SRR1) = PPCSIMBOOKE_CPU_REG(REG_MSR);

            PPCSIMBOOKE_CPU_REG(REG_ESR) = ESR_SPV;   // Set ESR[SPE]

            CLR_DEFAULT_MSR_BITS();
            PPCSIMBOOKE_CPU_PC = GET_PC_FROM_IVOR_NUM(34);
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

#undef GET_PC_FROM_IVOR_NUM
#undef CLR_DEFAULT_MSR_BITS

    LOG_DEBUG4(MSG_FUNC_END);
}


/*
 * @func  : get_instr()
 * @args  : none
 *
 * @brief : Read instruction at next NIP
 */
ppcsimbooke::instr_call ppcsimbooke::ppcsimbooke_cpu::cpu::get_instr(){
    LOG_DEBUG4(MSG_FUNC_START);

    xlated_tlb_res res;           // tuple of <ra, wimge, ps>
    instr_call call_this;

    call_this = m_instr_cache[PPCSIMBOOKE_CPU_PC];

    if(!m_instr_cache.error()){
        LOG_DEBUG4(MSG_FUNC_END);
        return call_this;
    }

    // Translate program counter to physical address
    res = xlate(PPCSIMBOOKE_CPU_PC, 0, 1);  // wr=0, ex=1

    LOG_DEBUG4(std::hex, std::showbase, "instr Xlation : ", PPCSIMBOOKE_CPU_PC, " -> ", std::get<0>(res), std::endl);

    LASSERT_THROW_UNLIKELY(m_mem_ptr != NULL, sim_except_fatal("no memory module registered."), DEBUG4);
    // Disassemble the instr at curr pc
    call_this = m_dis.disasm(m_mem_ptr->read32(std::get<0>(res), (std::get<1>(res) & 0x1)), PPCSIMBOOKE_CPU_PC, (std::get<1>(res) & 0x1));

    // Update instruction cache.
    m_instr_cache[PPCSIMBOOKE_CPU_PC] = call_this;

    LOG_DEBUG4(MSG_FUNC_END);
    return call_this;
}

/*
 * @func  : check_for_dbg_events
 * @args  : flags ( bit mask of various events ), ea ( used only for DAC events )
 *
 * @brief : flags signal for various debug events
 */
void ppcsimbooke::ppcsimbooke_cpu::cpu::check_for_dbg_events(int flags, uint64_t ea){
    bool event_occurred = 0;
    uint64_t event_type = 0;
    uint64_t event_addr = 0;

    // Check for IAC event
    if(flags & DBG_EVENT_IAC){
        event_occurred= 0;
        // DBCR1[IAC12M] is not implemented right now
        if(PPCSIMBOOKE_CPU_REGMASK(REG_DBCR0, DBCR0_IAC1)
           &&
           (PPCSIMBOOKE_CPU_REG(REG_IAC1) == PPCSIMBOOKE_CPU_PC)
           &&
           (
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1US) == 2) && !PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1US) == 3) && PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1US) != 2) && (PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1US) != 3))
           )
           &&
           (
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1ER) == 2) && !PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1ER) == 3) && PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1ER) != 2) && (PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC1ER) != 3))
           )
          ){
            event_occurred = 1;
            event_type     = PPC_EXCEPT_DBG_IAC1;
            event_addr     = PPCSIMBOOKE_CPU_PC;
        }
        // DBSR[IAC12M] isn't implemented right now
        if(PPCSIMBOOKE_CPU_REGMASK(REG_DBCR0, DBCR0_IAC2)
           &&
           (PPCSIMBOOKE_CPU_REG(REG_IAC2) == PPCSIMBOOKE_CPU_PC)
           &&
           (
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2US) == 2) && !PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2US) == 3) && PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_PR))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2US) != 2) && (PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2US) != 3))
           )
           &&
           (
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2ER) == 2) && !PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2ER) == 3) && PPCSIMBOOKE_CPU_REGMASK(REG_MSR, MSR_IS))
            ||
            ((PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2ER) != 2) && (PPCSIMBOOKE_CPU_REGMASK(REG_DBCR1, DBCR1_IAC2ER) != 3))
           )
          ){
            event_occurred = 1;
            event_type     = PPC_EXCEPT_DBG_IAC2;
            event_addr     = PPCSIMBOOKE_CPU_PC;
        }
    }

    // final steps
    if(event_occurred){
        ppc_exception(PPC_EXCEPTION_DBG, event_type, event_addr);
        if(PPCSIMBOOKE_CPU_REGMASK(REG_DBCR0, DBCR0_EDM)){
            // FIXME: Fix this ( if applicable )
            //PPCSIMBOOKE_CPU_REG(REG_EDBSR0) |= EDBSR0_XXX;
            // Do some additional steps
            LTHROW(sim_except_ppc_halt("Cpu halted due to debug exception."), DEBUG4);
        }
    }
}

// Blocking run
void ppcsimbooke::ppcsimbooke_cpu::cpu::run_b(){
    LOG_DEBUG4(MSG_FUNC_START);

    std::pair<uint64_t, bool> last_bkpt = m_bm.last_breakpoint();
    m_cpu_mode = CPU_MODE_RUNNING;
    static const int n_instrs_per_pass = 100;
    int i;

    // get first timing & clear all counters
    m_prev_stamp = boost::posix_time::microsec_clock::local_time();
    clear_ctrs();

    // run this instruction if this was last breakpointed
    if(last_bkpt.first == PPCSIMBOOKE_CPU_PC and last_bkpt.second == true){
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
    LOG_DEBUG4(MSG_FUNC_END);
}

inline void ppcsimbooke::ppcsimbooke_cpu::cpu::clear_ctrs(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_ninstrs_last = 0;
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : run current instr
 */
inline void ppcsimbooke::ppcsimbooke_cpu::cpu::run_curr_instr(){
    LOG_DEBUG4(MSG_FUNC_START);

    /* Get Instr call frame at next NIP */
    instr_call call_this = get_instr();
    LOG_DEBUG4("INSTR : ", call_this.get_instr_str(), std::endl);

    // Trace instructions
    //sm_instr_tracer("DEBUG") << "[CPU_" << (int)m_cpu_no << std::hex << "]\t" << "PC: 0x" << PPCSIMBOOKE_CPU_PC << "\t" << call_this.get_instr_str() << std::endl;
    // Log coverage
    m_cov_logger.probe(call_this.opcname);

    if(m_bm.check_pc(PPCSIMBOOKE_CPU_PC)){
        // Throw a software breakpoint exception
        LTHROW(sim_except(SIM_EXCEPT_SBKPT, "Software breakpoint"), DEBUG4);
    }

    // Do this while stepping
    if(m_cpu_mode == CPU_MODE_STEPPING){
        std::cout << std::hex << "PC:" <<  PPCSIMBOOKE_CPU_PC << " [ " << call_this.get_instr_str() << " ]" << std::endl;
    }

    // Check for any debug events for IAC
    // FIXME : This may not work at this time
    check_for_dbg_events(DBG_EVENT_IAC);
 
    /* call handler function for this call frame */ 
    m_ppc_func_hash.at(call_this.hv)(this, &call_this);

    m_ninstrs_last++;

    LOG_DEBUG4(MSG_FUNC_END);
}

// Initialize all common parameters
inline void ppcsimbooke::ppcsimbooke_cpu::cpu::init_common(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cpu_no = sm_ncpus++;                 // Increment global cpu cnt
    gen_ppc_opc_func_hash(this);           // Initialize opcode function pointer table
    m_instr_cache.set_size(4096);          // LRU cache size = 4096 instrs
    m_ctxt_switch = 0;                     // Initialize flag to zero
    m_cpu_mode = CPU_MODE_HALTED;
    m_cpu_exec_mode = CPU_EXEC_MODE_INTERPRETIVE;   // Fix to interpretive mode
    m_ncycles = 0;
    m_cpu_bits = 32;                       // 32 bit machine

    // Init logging facilities
    std::ostringstream ostr;
    ostr << "cpu_" << int(m_cpu_no) << "_cov.log";
    m_cov_logger.log_to_file(ostr.str());
    
    LOG_DEBUG4(MSG_FUNC_END);
}


// set reservation
void ppcsimbooke::ppcsimbooke_cpu::cpu::set_resv(uint64_t ea, size_t size){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(ea, 0);
    m_resv_addr = std::get<0>(res);
    m_resv_set  = true;
    m_resv_size = size;
    // We need a mutex here
    sm_resv_map[m_resv_addr & ~(m_cache_line_size - 1)] = std::make_pair(true, m_cpu_no);  // Global reservation always act on resv granules
    LOG_DEBUG4(MSG_FUNC_END);
}

// clear resv.
void ppcsimbooke::ppcsimbooke_cpu::cpu::clear_resv(uint64_t ea){
    LOG_DEBUG4(MSG_FUNC_START);
    m_resv_set = false;
    // Need a mutex
    sm_resv_map[m_resv_addr & ~(m_cache_line_size - 1)].first = false;
    LOG_DEBUG4(MSG_FUNC_END);
}

// Check resv
bool ppcsimbooke::ppcsimbooke_cpu::cpu::check_resv(uint64_t ea, size_t size){
    LOG_DEBUG4(MSG_FUNC_START);
    xlated_tlb_res res = xlate(ea, 1);  // Reservation is checked during stwcx.
    uint64_t caddr = std::get<0>(res) & ~(m_cache_line_size - 1);    // Get granule addr
    if(sm_resv_map.find(caddr) == sm_resv_map.end()){
        return false;
    }
    if(std::get<0>(res) == m_resv_addr and m_resv_set == true and m_resv_size == size and
            sm_resv_map[caddr].first == true and sm_resv_map[caddr].second == m_cpu_no){
        LOG_DEBUG4(MSG_FUNC_END);
        return true;
    }
    LOG_DEBUG4(MSG_FUNC_END);
    return false;
}

// Notify of context switches. LRU cache uses this parameter to flush it's
// instruction cache when a context switch happens.
void ppcsimbooke::ppcsimbooke_cpu::cpu::notify_ctxt_switch(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_ctxt_switch = 1;
    m_instr_cache.clear();
    LOG_DEBUG4(MSG_FUNC_END);
}

// Get register value by name
uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_reg(std::string reg_name) throw(sim_except) {
    LOG_DEBUG4(MSG_FUNC_START);
    //if unlikely(m_cpu_regs.m_reg.find(reg_name) == m_cpu_regs.m_reg.end()) throw sim_except(SIM_EXCEPT_EINVAL, "Illegal register name.");
    LOG_DEBUG4(MSG_FUNC_END);
    return PPCSIMBOOKE_CPU_REGN(reg_name);
}

// Get register value by name
uint64_t ppcsimbooke::ppcsimbooke_cpu::cpu::get_reg(int reg_id) throw(sim_except) {
    LOG_DEBUG4(MSG_FUNC_START);
    //if unlikely(m_cpu_regs.m_ireg.find(reg_id) == m_cpu_regs.m_ireg.end()) throw sim_except(SIM_EXCEPT_EINVAL, "Illegal register ID.");
    LOG_DEBUG4(MSG_FUNC_END);
    return PPCSIMBOOKE_CPU_REG(reg_id);
}

// Dump CPU state
void ppcsimbooke::ppcsimbooke_cpu::cpu::dump_state(std::ostream &ostr){
    LOG_DEBUG4(MSG_FUNC_START);

    // print register file state
    ostr << m_cpu_regs;

    LOG_DEBUG4(MSG_FUNC_END);
}

// print all tlbs
void ppcsimbooke::ppcsimbooke_cpu::cpu::print_L2tlbs(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_l2tlb.print_tlbs2();
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::enable_cov_log(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_logger.enable();
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::disable_cov_log(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_logger.disable();
    LOG_DEBUG4(MSG_FUNC_END);
}

bool ppcsimbooke::ppcsimbooke_cpu::cpu::is_cov_log_enabled(){
    LOG_DEBUG4(MSG_FUNC_START);
    return m_cov_logger.is_enabled();
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::gen_cov_log(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_logger.add_ext_info("###############################################################");
    m_cov_logger.add_ext_info("  COVERAGE FOR CPU " + boost::lexical_cast<std::string>(int(m_cpu_no)));
    m_cov_logger.add_ext_info("###############################################################\n");
    m_cov_logger.generate_log();
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu::log_cov_to_file(std::string filename){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_logger.log_to_file(filename);
    LOG_DEBUG4(MSG_FUNC_END);
}

// friend function
void ppcsimbooke::ppcsimbooke_cpu::gen_ppc_opc_func_hash(ppcsimbooke::ppcsimbooke_cpu::cpu *pcpu){
    #include "cpu_ppc_instr.cc"
}
