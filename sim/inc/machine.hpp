#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "config.h"
#include "cpu_ppc.hpp"
#include "memory.hpp"

// Machine class
template <int n_cpus, int m_bits, int cl_size, int tlb4K_ns, int tlb4K_nw, int tlbCam_ne> struct machine {
    typedef CPU_PPC<cl_size, m_bits, tlb4K_ns, tlb4K_nw, tlbCam_ne>  cpu_t;
    typedef memory<m_bits>                                           memory_t;
    cpu_t                                                            m_cpu[n_cpus];
    memory_t                                                         m_memory;   // one memory module
    const int                                                        m_ncpus;

    // Constructor
    machine() : m_ncpus(n_cpus){
        LOG("DEBUG4") << MSG_FUNC_START;

        uint64_t cpuid = 0x81008100;   // An id
        std::string name = "e500v2";
        std::ostringstream ostr;
        for(int i=0; i<m_ncpus; i++){
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

    // Common run ( invokes run of invoked CPUs using multiple threads )
    void run(unsigned mask=0){
        if(mask == 0){
            m_cpu[0].run();
            return;
        }
        // FIXME: Use a multithreaded approach here.
        //for(size_t i=0; i<sizeof(mask); i++){
        //    if((mask >> i) & 0x1){
        //        m_cpu[0].run();
        //    }
        //}
        return;
    }

    // for boost::python
    template<int cpu_no> cpu_t& get_cpu(){
        return m_cpu[cpu_no];
    }

};

#endif
