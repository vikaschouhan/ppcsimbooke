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
    // TODO:  for time being we are not keeping any record of threads, just starting them and
    //        leaving them to detatch
    //        We need to do some bookeeping in future.
    void run(unsigned mask=0){
        LOG("DEBUG4") << MSG_FUNC_START;
        if(mask == 0){
            boost::thread thr0(boost::bind(&cpu_t::run, &(m_cpu[0])));
            LOG("DEBUG4") << MSG_FUNC_END;
            return;
        }
        for(size_t i=0; i<sizeof(mask); i++){
            if((mask >> i) & 0x1){
                boost::thread thr0(boost::bind(&cpu_t::run, &(m_cpu[i])));
            }
        }
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }

    void stop(unsigned mask=0){
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

    // for boost::python
    template<int cpu_no> cpu_t& get_cpu(){
        return m_cpu[cpu_no];
    }

};

#endif
