// machine.hpp (top level machine wrapper)
// 
// Author : Vikas Chouhan (presentisgood@gmail.com)
// Copyright 2012.
// 
// This file is part of ppc-sim library.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.
// 
// It is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file; see the file COPYING.  If not, write to the
// Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

#define MACHINE_T
#define MACHINE        machine
#define MACHINE_CL_T   MACHINE

// Machine class
struct MACHINE {
    typedef CPU_PPC                                                  cpu_t;
    typedef memory                                                   memory_t;
    cpu_t                                                            m_cpu[N_CPUS];
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

MACHINE_T MACHINE_CL_T::MACHINE() : m_ncpus(N_CPUS){
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
