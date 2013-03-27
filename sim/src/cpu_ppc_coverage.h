#ifndef _CPU_PPC_COVERAGE_HPP
#define _CPU_PPC_COVERAGE_HPP

//  cpu_ppc_coverage.hpp 
//  This file contains coverage logging facilities for powerpc cpu
//
//  Copyright 2012.
//  Author : Vikas Chouhan ( presentisgood@gmail.com )
//
//  This file is part of ppc-sim.
//
//  This library is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3, or (at your option)
//  any later version.
//
//  It is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this file; see the file COPYING.  If not, write to the
//  Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
//  MA 02110-1301, USA.

#include "config.h"

#define CPU_PPC_COVERAGE   cpu_ppc_coverage
class CPU_PPC_COVERAGE {
    std::map<std::string, uint64_t>          m_instr_hits;
    static std::vector<std::string>          sm_instr_list;
    std::ofstream                            m_report_logger;
    bool                                     m_cov_enabled;

    public:
    typedef    std::map<std::string, uint64_t>  instr_hits_type;
    typedef    instr_hits_type::iterator        instr_hits_iter_type;

    public:
    void        probe(std::string opcd);               // Probe instruction / coverage pt
    void        log_to_file(std::string filename);     // Specify a new file to divert logs to
    void        enable();                              // enable coverage logging
    void        disable();                             // disable coverage logging
    bool        is_enabled();                          // check if enabled
    void        generate_log();                        // Generate logs
    void        add_ext_info(std::string info);        // Add extra information to log ( headers for eg. )

    static int  get_num_opcodes(){ return sm_instr_list.size(); }

    CPU_PPC_COVERAGE();
    CPU_PPC_COVERAGE(const CPU_PPC_COVERAGE& c);
    CPU_PPC_COVERAGE(std::string filename, std::string header="");
    ~CPU_PPC_COVERAGE();
};

 
#endif
