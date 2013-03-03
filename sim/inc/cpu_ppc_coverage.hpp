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

// Probe instruction
void CPU_PPC_COVERAGE::probe(std::string opcd){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_cov_enabled == false){
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }

    //LASSERT_THROW(m_instr_hits.find(opcd) != m_instr_hits.end(), sim_except_fatal("Invalid opcode in cpu_ppc_coverage."), DEBUG4);
    if(m_instr_hits.find(opcd) == m_instr_hits.end()){
        std::cout << "Warning !! " << opcd << " doesn't exist in coverage logger." << std::endl;
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }
    m_instr_hits[opcd]++;
    LOG("DEBUG4") << MSG_FUNC_END;
}

void CPU_PPC_COVERAGE::log_to_file(std::string filename){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_cov_enabled == false){
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }
    if(m_report_logger.is_open())
        m_report_logger.close();
    m_report_logger.open(filename);
    LOG("DEBUG4") << MSG_FUNC_END;
}

void CPU_PPC_COVERAGE::enable(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_enabled = true;
    LOG("DEBUG4") << MSG_FUNC_END;
}

void CPU_PPC_COVERAGE::disable(){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_enabled = false;
    LOG("DEBUG4") << MSG_FUNC_END;
}

bool CPU_PPC_COVERAGE::is_enabled(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
    return m_cov_enabled;
}

void CPU_PPC_COVERAGE::generate_log(){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_cov_enabled == false){
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }

    int tot_hits = 0;
    if(m_report_logger.is_open()){
        m_report_logger << std::setw(20) << std::setfill(' ') << "cov. points" << std::setw(20) << std::setfill(' ') << "hits" << std::endl;
        for(instr_hits_iter_type it0 = m_instr_hits.begin(); it0 != m_instr_hits.end(); it0++){
            if(it0->second > 0)
                tot_hits++;
            m_report_logger << std::setw(20) << std::setfill(' ') << it0->first << std::setw(20) << std::setfill(' ') << it0->second << std::endl;
        }

        // Log hits/total
        m_report_logger << std::endl;
        m_report_logger << "Total hits : " << tot_hits << "/" << sm_instr_list.size() << std::endl;
    }else{
        std::cout << "No logger specified. Did you call log_to_file()." << std::endl;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

void CPU_PPC_COVERAGE::add_ext_info(std::string info){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_cov_enabled == false){
        LOG("DEBUG4") << MSG_FUNC_END;
        return;
    }

    if(m_report_logger.is_open()){
        m_report_logger << info << std::endl;
    }else{
        std::cout << "No logger specified. Did you call log_to_file()." << std::endl;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_PPC_COVERAGE::CPU_PPC_COVERAGE(){
    LOG("DEBUG4") << MSG_FUNC_START;
    for(size_t i=0; i<sm_instr_list.size(); i++){
        m_instr_hits[sm_instr_list[i]] = 0;
    }
    m_cov_enabled = true;
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_PPC_COVERAGE::CPU_PPC_COVERAGE(const CPU_PPC_COVERAGE& c){
    LOG("DEBUG4") << MSG_FUNC_START;
    m_cov_enabled = c.m_cov_enabled;
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_PPC_COVERAGE::CPU_PPC_COVERAGE(std::string filename, std::string header){
    LOG("DEBUG4") << MSG_FUNC_START;
    for(size_t i=0; i<sm_instr_list.size(); i++){
        m_instr_hits[sm_instr_list[i]] = 0;
    }
    m_cov_enabled = true;
    m_report_logger.open(filename);
    // Add supplied header
    m_report_logger << header << std::endl;
    LOG("DEBUG4") << MSG_FUNC_END;
}

CPU_PPC_COVERAGE::~CPU_PPC_COVERAGE(){
    LOG("DEBUG4") << MSG_FUNC_START;
    if(m_report_logger.is_open())
        m_report_logger.close();
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Add instr lists
// Would require c++0x for direct vector assignment
std::vector<std::string> CPU_PPC_COVERAGE::sm_instr_list = {
   "add", "add.", "addc", "addc.", "addco", "addco.", "adde", "adde.", "addeo", "addeo.",
   "addi", "addic", "addic.", "addis", "addme", "addme.", "addmeo", "addmeo.", "addo",
   "addo.", "addze", "addze.", "addzeo", "addzeo.", "and", "and.", "andc", "andc.",
   "andi.", "andis.",
   
   "b", "ba", "bbelr", "bblels", "bc", "bca", "bcctr", "bcctrl", "bcl", "bcla", "bclr",
   "bclrl", "bl", "bla", "brinc",
   
   "cmp", "cmpi", "cmpl", "cmpli", "cntlzw", "cntlzw.", "crand", "crandc", "creqv",
   "crnand", "crnor", "cror", "crorc", "crxor",
   
   "dcba", "dcbf", "dcbi", "dcblc", "dcbst", "dcbt", "dcbtls", "dcbtst", "dcbtstls",
   "dcbz", "divw", "divw.", "divwo", "divwo.", "divwu", "divwu.", "divwuo", "divwuo.",
   
   "efdabs", "efdadd", "efdcfs", "efdcfsf", "efdcfsi", "efdcfuf", "efdcfui", "efdcmpeq",
   "efdcmpgt", "efdcmplt", "efdctsf", "efdctsi", "efdctsiz", "efdctuf", "efdctui",
   "efdctuiz", "efddiv", "efdmul", "efdnabs", "efdneg", "efdsub", "efdtsteq", "efdtstgt",
   "efdtstlt", "efsabs", "efsadd", "efscfd", "efscfsf", "efscfsi", "efscfuf", "efscfui",
   "efscmpeq", "efscmpgt", "efscmplt", "efsctsf", "efsctsi", "efsctsiz", "efsctuf",
   "efsctui", "efsctuiz", "efsdiv", "efsmul", "efsnabs", "efsneg", "efssub", "efststeq",
   "efststgt", "efststlt", "eqv", "eqv.", "evabs", "evaddiw", "evaddsmiaaw", "evaddssiaaw",
   "evaddumiaaw", "evaddusiaaw", "evaddw", "evand", "evandc", "evcmpeq", "evcmpgts",
   "evcmpgtu", "evcmplts", "evcmpltu", "evcntlsw", "evcntlzw", "evdivws", "evdivwu", "eveqv",
   "evextsb", "evextsh", "evfsabs", "evfsadd", "evfscfsf", "evfscfsi", "evfscfuf", "evfscfui",
   "evfscmpeq", "evfscmpgt", "evfscmplt", "evfsctsf", "evfsctsi", "evfsctsiz", "evfsctuf",
   "evfsctui", "evfsctuiz", "evfsdiv", "evfsmul", "evfsnabs", "evfsneg", "evfssub",
   "evfststeq", "evfststgt", "evfststlt", "evldd", "evlddx", "evldh", "evldhx", "evldw",
   "evldwx", "evlhhesplat", "evlhhesplatx", "evlhhossplat", "evlhhossplatx", "evlhhousplat",
   "evlhhousplatx", "evlwhe", "evlwhex", "evlwhos", "evlwhosx", "evlwhou", "evlwhoux",
   "evlwhsplat", "evlwhsplatx", "evlwwsplat", "evlwwsplatx", "evmergehi", "evmergehilo",
   "evmergelo", "evmergelohi", "evmhegsmfaa", "evmhegsmfan", "evmhegsmiaa", "evmhegsmian",
   "evmhegumiaa", "evmhegumian", "evmhesmf", "evmhesmfa", "evmhesmfaaw", "evmhesmfanw",
   "evmhesmi", "evmhesmia", "evmhesmiaaw", "evmhesmianw", "evmhessf", "evmhessfa",
   "evmhessfaaw", "evmhessfanw", "evmhessiaaw", "evmhessianw", "evmheumi", "evmheumia",
   "evmheumiaaw", "evmheumianw", "evmheusiaaw", "evmheusianw", "evmhogsmfaa", "evmhogsmfan",
   "evmhogsmiaa", "evmhogsmian", "evmhogumiaa", "evmhogumian", "evmhosmf", "evmhosmfa",
   "evmhosmfaaw", "evmhosmfanw", "evmhosmi", "evmhosmia", "evmhosmiaaw", "evmhosmianw",
   "evmhossf", "evmhossfa", "evmhossfaaw", "evmhossfanw", "evmhossiaaw", "evmhossianw",
   "evmhoumi", "evmhoumia", "evmhoumiaaw", "evmhoumianw", "evmhousiaaw", "evmhousianw",
   "evmra", "evmwhsmf", "evmwhsmfa", "evmwhsmi", "evmwhsmia", "evmwhssf", "evmwhssfa",
   "evmwhumi", "evmwhumia", "evmwhusiaaw", "evmwhusianw", "evmwlumi", "evmwlumia", "evmwlumiaaw",
   "evmwlumianw", "evmwlusiaaw", "evmwlusianw", "evmwsmf", "evmwsmfa", "evmwsmfaa", "evmwsmfan",
   "evmwsmi", "evmwsmia", "evmwsmiaa", "evmwsmian", "evmwssf", "evmwssfa", "evmwssfaa",
   "evmwssfan", "evmwumi", "evmwumia", "evmwumiaa", "evmwumian", "evnand", "evneg", "evnor",
   "evor", "evorc", "evrlw", "evrlwi", "evrndw", "evsel", "evslw", "evslwi", "evsplatfi",
   "evsplati", "evsrwis", "evsrwiu", "evsrws", "evsrwu", "evstdd", "evstddx", "evstdh",
   "evstdhx", "evstdw", "evstdwx", "evstwhe", "evstwhex", "evstwho", "evstwhox", "evstwwe",
   "evstwwex", "evstwwo", "evstwwox", "evsubfsmiaaw", "evsubfssiaaw", "evsubfumiaaw",
   "evsubfusiaaw", "evsubfw", "evsubifw", "evxor", "extsb", "extsb.", "extsh", "extsh.",
   
   "icbi", "icblc", "icbt", "icbtls", "isel", "isync",
   
   "lbz", "lbzu", "lbzux", "lbzx", "lha", "lhau", "lhaux", "lhax", "lhbrx", "lhz", "lhzu",
   "lhzux", "lhzx", "lmw", "lwarx", "lwbrx", "lwz", "lwzu", "lwzux", "lwzx",
   
   "mbar", "mcrf", "mcrxr", "mfcr", "mfmsr", "mfpmr", "mfspr", "msync", "mtcrf", "mtmsr", "mtpmr",
   "mtspr", "mulhw", "mulhw.", "mulhwu", "mulhwu.", "mulli", "mullw", "mullw.", "mullwo", "mullwo.",
   
   "nand", "nand.", "neg", "neg.", "nego", "nego.", "nor", "nor.",
   
   "or", "or.", "orc", "orc.", "ori", "oris",
   
   "rfci", "rfi", "rfmci", "rlwimi", "rlwimi.", "rlwinm", "rlwinm.", "rlwnm", "rlwnm.",
   
   "sc", "slw", "slw.", "sraw", "sraw.", "srawi", "srawi.", "srw", "srw.", "stb", "stbu",
   "stbux", "stbx", "sth", "sthbrx", "sthu", "sthux", "sthx", "stmw", "stw", "stwbrx", "stwcx.",
   "stwu", "stwux", "stwx", "subf", "subf.", "subfc", "subfc.", "subfco", "subfco.", "subfe",
   "subfe.", "subfeo", "subfeo.", "subfic", "subfme", "subfme.", "subfmeo", "subfmeo.", "subfo",
   "subfo.", "subfze", "subfze.", "subfzeo", "subfzeo.",
   
   "tlbivax", "tlbre", "tlbsx", "tlbsync", "tlbwe", "tw", "twi",
   
   "wrtee", "wrteei",
   
   "xor", "xor.", "xori", "xoris"
};
 
#endif
