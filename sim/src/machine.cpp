#include "machine.h"

// Member functions

ppcsimbooke::machine::machine() : m_ncpus(N_CPUS){
    LOG_DEBUG4(MSG_FUNC_START);

    uint64_t cpuid = 0x81008100;   // An id
    std::string name = "e500v2";
    std::ostringstream ostr;
    for(int i=0; i<m_ncpus; i++){
        ostr.clear();
        ostr << i;
        m_cpu[i].init(cpuid, "e500v2_" + ostr.str());
        m_cpu[i].register_mem(m_memory);
    }
    trace_to_file("machine_trace.log");

    LOG_DEBUG4(MSG_FUNC_END);
}

// Load an elf binary directly into machine's memory
void ppcsimbooke::machine::load_elf(std::string filename){
    LOG_DEBUG4(MSG_FUNC_START);
    m_memory.load_elf(filename);
    LOG_DEBUG4(MSG_FUNC_END);
}

// Common run ( invokes run of invoked CPUs using multiple threads )
// TODO:  for time being we are not keeping any record of threads, just starting them and
//        leaving them to detatch
//        We need to do some bookeeping in future.
void ppcsimbooke::machine::run(unsigned mask){
    LOG_DEBUG4(MSG_FUNC_START);
    if(mask == 0){
        m_cpu[0].run();
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }
    for(size_t i=0; i<sizeof(mask); i++){
        if((mask >> i) & 0x1){
            m_cpu[i].run();
        }
    }
    LOG_DEBUG4(MSG_FUNC_END);
    return;
}

void ppcsimbooke::machine::stop(unsigned mask){
    LOG_DEBUG4(MSG_FUNC_START);
    if(mask == 0){
        m_cpu[0].stop();
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }
    // FIXME: All threads should be stopped at once.
    //        We will look into this later on.
    for(size_t i=0; i<sizeof(mask); i++){
        if((mask >> i) & 0x1){
            m_cpu[i].stop();
        }
    }
    LOG_DEBUG4(MSG_FUNC_END);
    return;
}

void ppcsimbooke::machine::run_mode(unsigned mask){
    LOG_DEBUG4(MSG_FUNC_START);
    if(mask == 0){
        m_cpu[0].run_mode();
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }
    // FIXME: All threads should be stopped at once.
    //        We will look into this later on.
    for(size_t i=0; i<sizeof(mask); i++){
        if((mask >> i) & 0x1){
            m_cpu[i].run_mode();
        }
    }
    LOG_DEBUG4(MSG_FUNC_END);
    return;
}

void ppcsimbooke::machine::trace_to_file(std::string filename){
    LOG_DEBUG4(MSG_FUNC_START);
    cpu_t::sm_instr_tracer.direct_to_file(filename);
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::machine::trace_enable(){
    LOG_DEBUG4(MSG_FUNC_START);
    cpu_t::sm_instr_tracer.enable();
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::machine::trace_disable(){
    LOG_DEBUG4(MSG_FUNC_START);
    cpu_t::sm_instr_tracer.disable();
    LOG_DEBUG4(MSG_FUNC_END);
}

bool ppcsimbooke::machine::is_trace_enabled(){
    LOG_DEBUG4(MSG_FUNC_START);
    LOG_DEBUG4(MSG_FUNC_END);
    return cpu_t::sm_instr_tracer.is_enabled();
}

void ppcsimbooke::machine::gen_cov_logs(){
    LOG_DEBUG4(MSG_FUNC_START);
    for(int i=0; i<m_ncpus; i++)
        m_cpu[i].gen_cov_log();
    LOG_DEBUG4(MSG_FUNC_END);
}
