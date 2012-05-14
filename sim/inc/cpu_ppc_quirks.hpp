// This file defines any cpu specific quirks/fixes/erratas etc.
//
// Author : Vikas Chouhan ( presentisgood@gmail.com )
// Copyright 2012.
// 
// This file is part of ppc-sim library bundled with test_gen_ppc.
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
#ifndef _CPU_PPC_QUIRKS_HPP_
#define _CPU_PPC_QUIRKS_HPP_
#include "config.h"

class ppc_cpu_booke_quirks {
    public:
    // Constructor
    ppc_cpu_booke_quirks(){
        // Add instruction specific quirks
        instr_quirks["addi"] = instr_arg_quirks(0x2);
    }
    // Destructor
    ~ppc_cpu_booke_quirks() {}

    // zero arg fix quirk
    std::string arg_fix_zero_quirk(std::string opc, std::string arg, int argno){
        boost::algorithm::to_lower(arg);
        if(instr_quirks.find(opc) == instr_quirks.end()){
            return arg;
        }else if(instr_quirks[opc].set_to_zero_if_zero[argno]){
            return "0";
        }
        return arg;
    }

    private:
    struct instr_arg_quirks {
        std::bitset<N_IC_ARGS> set_to_zero_if_zero;   // Bit mask of boolean values

        instr_arg_quirks(int fix_zero_mask=0){
            for(int i=0; i<N_IC_ARGS; i++){
                set_to_zero_if_zero[i] = ((fix_zero_mask >> i) & 0x1);
            }
        }
        ~instr_arg_quirks() {}
    };
    // Map of instruction arguments quirks
    std::map<std::string, instr_arg_quirks> instr_quirks;
};

#endif
