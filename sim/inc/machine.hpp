#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

#define MACHINE_T      template<int ta_n_cpus, int ta_m_bits, int ta_cl_size, int ta_tlb4K_ns, int ta_tlb4K_nw, int ta_tlbCam_ne>
#define MACHINE        machine
#define MACHINE_CL_T   MACHINE<ta_n_cpus, ta_m_bits, ta_cl_size, ta_tlb4K_ns, ta_tlb4K_nw, ta_tlbCam_ne>  

// Machine class
template <int n_cpus, int m_bits, int cl_size, int tlb4K_ns, int tlb4K_nw, int tlbCam_ne>
struct MACHINE {
    typedef CPU_PPC<cl_size, m_bits, tlb4K_ns, tlb4K_nw, tlbCam_ne>  cpu_t;
    typedef memory<m_bits>                                           memory_t;
    cpu_t                                                            m_cpu[n_cpus];
    memory_t                                                         m_memory;   // one memory module
    const int                                                        m_ncpus;

    // Constructor
    MACHINE();

    // Operation facilities
    void load_elf(std::string filename);         // loading elf files
    void run(unsigned mask=0);                   // run
    void stop(unsigned mask=0);                  // stop
    void run_mode(unsigned mask=0);              // display run modes

    // Tracing facilities
    void trace_to_file(std::string filename);
    void trace_enable();
    void trace_disable();
    bool is_trace_enabled();

    // Logging facilities
    void gen_cov_logs();                         // Generate coverage logs for all cpus

    // for boost::python
    template<int cpu_no> cpu_t& get_cpu(){
        return m_cpu[cpu_no];
    }
};

// Member functions

MACHINE_T MACHINE_CL_T::MACHINE() : m_ncpus(ta_n_cpus){
    LOG("DEBUG4") << MSG_FUNC_START;

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

    LOG("DEBUG4") << MSG_FUNC_END;
}

// Load an elf binary directly into machine's memory
MACHINE_T void MACHINE_CL_T::load_elf(std::string filename){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_memory.load_elf(filename);
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Common run ( invokes run of invoked CPUs using multiple threads )
// TODO:  for time being we are not keeping any record of threads, just starting them and
//        leaving them to detatch
//        We need to do some bookeeping in future.
MACHINE_T void MACHINE_CL_T::run(unsigned mask){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(mask == 0){
        m_cpu[0].run();
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }
    for(size_t i=0; i<sizeof(mask); i++){
        if((mask >> i) & 0x1){
            m_cpu[i].run();
        }
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return;
}

MACHINE_T void MACHINE_CL_T::stop(unsigned mask){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(mask == 0){
        m_cpu[0].stop();
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }
    // FIXME: All threads should be stopped at once.
    //        We will look into this later on.
    for(size_t i=0; i<sizeof(mask); i++){
        if((mask >> i) & 0x1){
            m_cpu[i].stop();
        }
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return;
}

MACHINE_T void MACHINE_CL_T::run_mode(unsigned mask){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(mask == 0){
        m_cpu[0].run_mode();
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }
    // FIXME: All threads should be stopped at once.
    //        We will look into this later on.
    for(size_t i=0; i<sizeof(mask); i++){
        if((mask >> i) & 0x1){
            m_cpu[i].run_mode();
        }
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return;
}

MACHINE_T void MACHINE_CL_T::trace_to_file(std::string filename){
    LOG("DEBUG4") << MSG_FUNC_START;
    cpu_t::sm_instr_tracer.direct_to_file(filename);
    LOG("DEBUG4") << MSG_FUNC_END;
}

MACHINE_T void MACHINE_CL_T::trace_enable(){
    LOG("DEBUG4") << MSG_FUNC_START;
    cpu_t::sm_instr_tracer.enable();
    LOG("DEBUG4") << MSG_FUNC_END;
}

MACHINE_T void MACHINE_CL_T::trace_disable(){
    LOG("DEBUG4") << MSG_FUNC_START;
    cpu_t::sm_instr_tracer.disable();
    LOG("DEBUG4") << MSG_FUNC_END;
}

MACHINE_T bool MACHINE_CL_T::is_trace_enabled(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return cpu_t::sm_instr_tracer.is_enabled();
}

MACHINE_T void MACHINE_CL_T::gen_cov_logs(){
    LOG("DEBUG4") << MSG_FUNC_START;
    for(int i=0; i<m_ncpus; i++)
        m_cpu[i].gen_cov_log();
    LOG("DEBUG4") << MSG_FUNC_END;
}

#endif
