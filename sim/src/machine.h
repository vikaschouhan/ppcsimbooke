// machine.h (top level machine wrapper)
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
#include "cpu_ppc.h"
#include "memory.h"

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

#endif
