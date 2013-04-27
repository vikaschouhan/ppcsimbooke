#ifndef BM_H_
#define BM_H_

// bm.h (breakpoint manager for powerPC cpu module)
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

namespace ppcsimbooke {
    namespace ppcsimbooke_cpu {

        class breakpt_mngr {
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
            breakpt_mngr(): m_bkpts_enabled(true){}
        
            // Enable breakpoints
            void enable_breakpoints();
            // Disable breakpoints
            void disable_breakpoints();
            // Get last breakpoint
            std::pair<uint64_t, bool> last_breakpoint();
            // clear last breakpoint
            void clear_last_breakpoint();
            // check adddress for any hit
            bool check_pc(uint64_t pc);
            // Add breakpoint
            void add_breakpoint(uint64_t ea);
            // List all breakpoints
            void list_breakpoints();
            // Delete breakpoint by ea
            void delete_breakpoint(uint64_t ea);
            // Delete breakpoint by number
            void delete_breakpoint_num(int nbk);
            // Delete all breakpoints
            void delete_all();
        };
    }
}

#endif
