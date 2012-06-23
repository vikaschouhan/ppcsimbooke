#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

class machine {
    public:
    cpu_ppc_booke         m_cpu[1];   // only 1 cpu
    memory                m_memory;   // one memory module

    machine(){
        uint64_t cpuid = 0x81008100;   // An id
        std::string name = "e500v2";
        std::ostringstream ostr;
        for(int i=0; i<1; i++){
            ostr.clear();
            ostr << i;
            m_cpu[i].init_cpu_ppc_booke(cpuid, "e500v2_" + ostr.str());
        }
        m_memory.init_memory(36);
    }
};

#endif
