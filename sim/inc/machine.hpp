#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

// Machine class
template <int ncpus, int nbits> class machine {
    public:
    cpu_ppc_booke                 m_cpu[ncpus];
    memory                        m_memory;   // one memory module
    const int                     m_ncpus;

    machine() : m_memory(nbits), m_ncpus(ncpus){
        uint64_t cpuid = 0x81008100;   // An id
        std::string name = "e500v2";
        std::ostringstream ostr;
        for(int i=0; i<ncpus; i++){
            ostr.clear();
            ostr << i;
            m_cpu[i].init_cpu_ppc_booke(cpuid, "e500v2_" + ostr.str());
        }
    }
    // for boost::python
    template<int cpu_no> cpu_ppc_booke& get_cpu(){
        return m_cpu[cpu_no];
    }

};

#endif
