#ifndef _BM_HPP
#define _BM_HPP

// bm.hpp ( breakpoint manager for powerPC cpu module )
// This file contains breakpoint manager ( bm ) class and corresponding member functions.
//
// Author :
//     Vikas Chouhan ( presentisgood@gmail.com )  Copyright 2012.
//
// This file is part of ppc-sim library bundled with ppc_sim_boooke.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
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

#include "config.h"

#define BM     breakpt_mngr
class BM {
    // key = ea, value = pairof<breakpoint no, no_of_hits>
    typedef std::map<uint64_t, std::pair<int, uint64_t> > bkpt_map;
    typedef bkpt_map::iterator                            bkpt_map_iter;

    // Private Data
    bkpt_map                                              m_bk_map;              // Hash of address breakpoints
    bool                                                  m_bkpts_enabled;       // Global enable/disable flag
    std::pair<uint64_t, bool>                             m_last_bkpt;           // Last breakpoint information
    static int                                            m_bk_num;              // Counter keeping track of breakpoint number

    public:
    // constructor
    BM(): m_bkpts_enabled(true){}

    // Enable breakpoints
    void enable_breakpoints(){
        LOG("DEBUG4") << MSG_FUNC_START;
        m_bkpts_enabled = true;
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // Disable breakpoints
    void disable_breakpoints(){
        LOG("DEBUG4") << MSG_FUNC_START;
        m_bkpts_enabled = false;
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // Get last breakpoint
    std::pair<uint64_t, bool> last_breakpoint(){
        LOG("DEBUG4") << MSG_FUNC_START;
        LOG("DEBUG4") << MSG_FUNC_END;
        return m_last_bkpt;
    }

    // clear last breakpoint
    void clear_last_breakpoint(){
        LOG("DEBUG4") << MSG_FUNC_START;
        m_last_bkpt.first = 0;
        m_last_bkpt.second = false;
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // check adddress for any hit
    bool check_pc(uint64_t pc){
        LOG("DEBUG4") << MSG_FUNC_START;
        if(m_bk_map.find(pc) == m_bk_map.end() or m_bkpts_enabled == false){
            LOG("DEBUG4") << MSG_FUNC_END;
            return false;
        }
        m_bk_map[pc].second ++;
        m_last_bkpt.first  = pc;
        m_last_bkpt.second = true;
        LOG("DEBUG4") << MSG_FUNC_END;
        return true;
    }

    // Add breakpoint
    void add_breakpoint(uint64_t ea){
        LOG("DEBUG4") << MSG_FUNC_START;
        LOG("DEBUG4") << std::hex << "Inserting breakpoint at ea=" << std::showbase << ea << std::endl;
        m_bk_map[ea] = std::make_pair(m_bk_num, 0);
        m_bk_num++;
        LOG("DEBUG4") << MSG_FUNC_START;
    }

    // List all breakpoints
    void list_breakpoints(){
        LOG("DEBUG4") << MSG_FUNC_START;
        for(bkpt_map_iter iter0 = m_bk_map.begin(); iter0 != m_bk_map.end(); iter0++){
            std::cout << iter0->second.first << "    " << std::hex << std::showbase <<
                std::setw(16) << std::setfill(' ') << iter0->first << "    hits:" << std::dec << iter0->second.second << std::endl;
        }
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // Delete breakpoint by ea
    void delete_breakpoint(uint64_t ea){
        LOG("DEBUG4") << MSG_FUNC_START;
        if(m_bk_map.find(ea) != m_bk_map.end()){
            m_bk_map.erase(ea);
        }else{
            LOG("DEBUG4") << "No breakpoint found on ea=0x" << std::hex << ea << std::endl;
        }
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // Delete breakpoint by number
    void delete_breakpoint_num(int nbk){
        LOG("DEBUG4") << MSG_FUNC_START;
        for(bkpt_map_iter iter0 = m_bk_map.begin(); iter0 != m_bk_map.end(); iter0++){
            if(iter0->second.first == nbk)
                m_bk_map.erase(iter0);
        }
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    // Delete all breakpoints
    void delete_all(){
        LOG("DEBUG4") << MSG_FUNC_START;
        m_bk_map.clear();
        m_bk_num = 0;
        LOG("DEBUG4") << MSG_FUNC_END;
    }
};

int BM::m_bk_num = 0;

#endif
