#include "cpu_ppc_coverage.h"

// static data members --------------------------------------------------------------------

// Add instr lists
// Would require c++0x for direct vector assignment
std::vector<std::string> ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::sm_instr_list = {
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

// member functions ---------------------------------------------------------------------

// Probe instruction
void ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::probe(std::string opcd){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_cov_enabled == false){
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }

    //LASSERT_THROW(m_instr_hits.find(opcd) != m_instr_hits.end(), sim_except_fatal("Invalid opcode in cpu_ppc_coverage."), DEBUG4);
    if(m_instr_hits.find(opcd) == m_instr_hits.end()){
        std::cout << "Warning !! " << opcd << " doesn't exist in coverage logger." << std::endl;
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }
    m_instr_hits[opcd]++;
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::log_to_file(std::string filename){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_cov_enabled == false){
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }
    if(m_report_logger.is_open())
        m_report_logger.close();
    m_report_logger.open(filename);
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::enable(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_enabled = true;
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::disable(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_enabled = false;
    LOG_DEBUG4(MSG_FUNC_END);
}

bool ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::is_enabled(){
    LOG_DEBUG4(MSG_FUNC_START);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_cov_enabled;
}

void ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::generate_log(){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_cov_enabled == false){
        LOG_DEBUG4(MSG_FUNC_END);
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
    LOG_DEBUG4(MSG_FUNC_END);
}

void ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::add_ext_info(std::string info){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_cov_enabled == false){
        LOG_DEBUG4(MSG_FUNC_END);
        return;
    }

    if(m_report_logger.is_open()){
        m_report_logger << info << std::endl;
    }else{
        std::cout << "No logger specified. Did you call log_to_file()." << std::endl;
    }
    LOG_DEBUG4(MSG_FUNC_END);
}

ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::cpu_coverage(){
    LOG_DEBUG4(MSG_FUNC_START);
    for(size_t i=0; i<sm_instr_list.size(); i++){
        m_instr_hits[sm_instr_list[i]] = 0;
    }
    m_cov_enabled = true;
    LOG_DEBUG4(MSG_FUNC_END);
}

ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::cpu_coverage(const ppcsimbooke::ppcsimbooke_cpu::cpu_coverage& c){
    LOG_DEBUG4(MSG_FUNC_START);
    m_cov_enabled = c.m_cov_enabled;
    LOG_DEBUG4(MSG_FUNC_END);
}

ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::cpu_coverage(std::string filename, std::string header){
    LOG_DEBUG4(MSG_FUNC_START);
    for(size_t i=0; i<sm_instr_list.size(); i++){
        m_instr_hits[sm_instr_list[i]] = 0;
    }
    m_cov_enabled = true;
    m_report_logger.open(filename);
    // Add supplied header
    m_report_logger << header << std::endl;
    LOG_DEBUG4(MSG_FUNC_END);
}

ppcsimbooke::ppcsimbooke_cpu::cpu_coverage::~cpu_coverage(){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_report_logger.is_open())
        m_report_logger.close();
    LOG_DEBUG4(MSG_FUNC_END);
}

