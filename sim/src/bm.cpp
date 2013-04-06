#include "bm.h"

// static data member functions
int BM::m_bk_num = 0;

// breakpoint manager member functions -----------------------------------------------
// Enable breakpoints
void BM::enable_breakpoints(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_bkpts_enabled = true;
    LOG_DEBUG4(MSG_FUNC_END);
}

// Disable breakpoints
void BM::disable_breakpoints(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_bkpts_enabled = false;
    LOG_DEBUG4(MSG_FUNC_END);
}

// Get last breakpoint
std::pair<uint64_t, bool> BM::last_breakpoint(){
    LOG_DEBUG4(MSG_FUNC_START);
    LOG_DEBUG4(MSG_FUNC_END);
    return m_last_bkpt;
}

// clear last breakpoint
void BM::clear_last_breakpoint(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_last_bkpt.first = 0;
    m_last_bkpt.second = false;
    LOG_DEBUG4(MSG_FUNC_END);
}

// check adddress for any hit
bool BM::check_pc(uint64_t pc){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_bk_map.find(pc) == m_bk_map.end() or m_bkpts_enabled == false){
        LOG_DEBUG4(MSG_FUNC_END);
        return false;
    }
    m_bk_map[pc].second ++;
    m_last_bkpt.first  = pc;
    m_last_bkpt.second = true;
    LOG_DEBUG4(MSG_FUNC_END);
    return true;
}

// Add breakpoint
void BM::add_breakpoint(uint64_t ea){
    LOG_DEBUG4(MSG_FUNC_START);
    LOG_DEBUG4(std::hex, "Inserting breakpoint at ea=", std::showbase, ea, std::endl);
    m_bk_map[ea] = std::make_pair(m_bk_num, 0);
    m_bk_num++;
    LOG_DEBUG4(MSG_FUNC_START);
}

// List all breakpoints
void BM::list_breakpoints(){
    LOG_DEBUG4(MSG_FUNC_START);
    for(bkpt_map_iter iter0 = m_bk_map.begin(); iter0 != m_bk_map.end(); iter0++){
        std::cout << iter0->second.first << "    " << std::hex << std::showbase <<
            std::setw(16) << std::setfill(' ') << iter0->first << "    hits:" << std::dec << iter0->second.second << std::endl;
    }
    LOG_DEBUG4(MSG_FUNC_END);
}

// Delete breakpoint by ea
void BM::delete_breakpoint(uint64_t ea){
    LOG_DEBUG4(MSG_FUNC_START);
    if(m_bk_map.find(ea) != m_bk_map.end()){
        m_bk_map.erase(ea);
    }else{
        LOG_DEBUG4("No breakpoint found on ea=0x", std::hex, ea, std::endl);
    }
    LOG_DEBUG4(MSG_FUNC_END);
}

// Delete breakpoint by number
void BM::delete_breakpoint_num(int nbk){
    LOG_DEBUG4(MSG_FUNC_START);
    for(bkpt_map_iter iter0 = m_bk_map.begin(); iter0 != m_bk_map.end(); iter0++){
        if(iter0->second.first == nbk)
            m_bk_map.erase(iter0);
    }
    LOG_DEBUG4(MSG_FUNC_END);
}

// Delete all breakpoints
void BM::delete_all(){
    LOG_DEBUG4(MSG_FUNC_START);
    m_bk_map.clear();
    m_bk_num = 0;
    LOG_DEBUG4(MSG_FUNC_END);
}
