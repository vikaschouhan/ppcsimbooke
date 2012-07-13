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

class cpu_ppc_booke_quirks {
    public:
    // Constructor
    cpu_ppc_booke_quirks(){
        // Add instruction specific quirks
        instr_quirks["addi"] = instr_arg_quirks(0x2);
    }
    // Destructor
    ~cpu_ppc_booke_quirks() {}

    // zero arg fix quirk
    void arg_fix_zero_quirk(instr_call &_call){
        int i;
        if(instr_quirks.find(_call.opcode) != instr_quirks.end()){
            for(i=0; i<_call.nargs; i++){
                // If bit for this arg is set "and" this arg is absolute zero "and" this arg is a register,
                // modify .p (pointer) value.
                if(instr_quirks[_call.opcode].zero_mask[i] && !_call.arg[i].v && _call.arg[i].t)
                    _call.arg[i].p = 0;
            }
        }
    }

    private:
    struct instr_arg_quirks {
        std::bitset<N_IC_ARGS>  zero_mask;   // Bit mask of boolean values

        instr_arg_quirks(int fix_zero_mask=0){
            for(int i=0; i<N_IC_ARGS; i++){
                zero_mask[i] = ((fix_zero_mask >> i) & 0x1);
            }
        }
        ~instr_arg_quirks() {}
    };
    // Map of instruction arguments quirks
    std::map<std::string, instr_arg_quirks> instr_quirks;
};

#endif
