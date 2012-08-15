#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

// Machine class
template <int ncpus, int nbits> struct machine {
    
    CPU_PPC                       m_cpu[ncpus];
    memory                        m_memory;   // one memory module
    const int                     m_ncpus;

    // Constructor
    machine() : m_memory(nbits), m_ncpus(ncpus){
        LOG("DEBUG4") << MSG_FUNC_START;

        uint64_t cpuid = 0x81008100;   // An id
        std::string name = "e500v2";
        std::ostringstream ostr;
        for(int i=0; i<ncpus; i++){
            ostr.clear();
            ostr << i;
            m_cpu[i].CAT(init_, CPU_PPC)(cpuid, "e500v2_" + ostr.str());
            m_cpu[i].register_mem(m_memory);
        }

        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // Load an elf binary directly into machine's memory
    void load_elf(std::string filename){
        LOG("DEBUG4") << MSG_FUNC_START;
        m_memory.load_elf(filename);
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // for boost::python
    template<int cpu_no> CPU_PPC& get_cpu(){
        return m_cpu[cpu_no];
    }

};

#endif
