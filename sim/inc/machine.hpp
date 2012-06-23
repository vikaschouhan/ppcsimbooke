#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

class machine {
    public:
    int                           m_ncpus;
    std::vector<cpu_ppc_booke>    m_cpu;   // only 1 cpu
    memory                        m_memory;   // one memory module

    machine(){
        uint64_t cpuid = 0x81008100;   // An id
        std::string name = "e500v2";
        std::ostringstream ostr;
        m_ncpus = 2;
        m_cpu.resize(m_ncpus);
        for(int i=0; i<m_ncpus; i++){
            ostr.clear();
            ostr << i;
            m_cpu[i].init_cpu_ppc_booke(cpuid, "e500v2_" + ostr.str());
        }
        m_memory.init_memory(36);
        
    }
    template<int cpu_no> cpu_ppc_booke& get_cpu(){
        return m_cpu[cpu_no];
    }

};

#endif
