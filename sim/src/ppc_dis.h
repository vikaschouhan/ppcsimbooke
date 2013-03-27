#ifndef _PPC_DIS_HPP_
#define _PPC_DIS_HPP_

// ppc_dis.hpp ( disassembler facilities for powerPC e500v2 cpu module )
// This file contains disassembler class and corresponding member functions.
//
// Authors :
//     GNU project.
//     Vikas Chouhan ( presentisgood@gmail.com )  Copyright 2012.
//
// This file is part of ppc-sim library bundled with ppcsimbooke.
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

// Most of the functions in this module will seem to be C'ish, since these
// were taken from ppc-dis project, ( which inturn was
// taken from GNU binutils ) which is written in C.
//
// NOTE: The functions in this file are in messy state. Hope to clean it some day.
//       Also since most of the functions were originally in C, I had to make changes
//       to fit them in C++ perspective. So they are kinda in mixed state.
//
// UPDATE :
//       Nov 2 2012 : Commented all floating point code, since e500v2 doesn't have a FPU      

#include "config.h"
#include "cpu_ppc_regs.h"
#include "third_party/ppcdis/ppcdis.h"
#include "third_party/lru/lru.hpp"

#define  N_PPC_OPCODES  65

#define DIS_PPC  ppc_dis

class DIS_PPC{
    private:
    struct _dis_info{
        uint64_t  pc;        // program counter
        bool      pcr;       // is this required
        _dis_info(uint64_t _pc, bool _pcr): pc(_pc), pcr(_pcr) {}
        _dis_info(): pc(0), pcr(0) {}
    };

    public:
    typedef lru_cache<uint32_t, instr_call, _dis_info>     ppc_dis_cache;
    typedef lru_cache<std::string, instr_call, _dis_info>  ppc_dis_cache2;

    private:
    ppc_cpu_t       m_dialect;
    ppc_dis_cache   m_dis_cache;                         // Disassembler cache
    ppc_dis_cache2  m_dis_cache2;                        // Second cache for string based opcodes
    static uint16_t m_ppc_opcd_indices[N_PPC_OPCODES];
    static bool     m_opcd_indices_done;

    private:
    // Initialize opcode indices for faster look up
    static void init_opcd_indices();
    // Initialize dialect
    void init_dialect();
    // Return new dialect according to specified cpu name
    ppc_cpu_t ppc_parse_cpu (ppc_cpu_t ppc_cpu, const char *arg);
    // Extract operand value from instruction
    long operand_value_powerpc (const struct powerpc_operand *operand, unsigned long insn);

    /* Find a match for INSN in the opcode table, given machine DIALECT.
    A DIALECT of -1 is special, matching all machine opcode variations.  */
    std::pair<const struct powerpc_opcode*, uint32_t> lookup_powerpc (unsigned long insn);

    // Helper function : check if standard delimiters are present
    int if_std_delims_present(char *str);
    // Helper function : Get number of operands from powerpc_opcode struct
    int get_num_operands(const struct powerpc_opcode *opcode);
    // Show instruction format
    void print_insn_fmt (const char *insn_name);
    // Helper function : Get delimiter character for next argno argument
    char get_delim_char(const struct powerpc_opcode *opc, int argno);
    
    public:
    // constructor
    DIS_PPC(){
        init_dialect();
        if(!m_opcd_indices_done){
            init_opcd_indices();
            m_opcd_indices_done = 1;
        }
        m_dis_cache.set_size(1024);   // 256 entry cache
        m_dis_cache2.set_size(16);   // 16 entry cache
    }

    // PC is required for instrs which use relative addressing mode
    // Disassemble a 32 bit opcode into a call frame
    instr_call disasm(uint32_t opcd, uint64_t pc = 0, int endianness = EMUL_BIG_ENDIAN);
    // Second form of disassemble. Takes a string, decodes it and creates an instruction call frame
    // if valid
    instr_call disasm(std::string instr, uint64_t pc = 0);
    // Return opcode's index in LUT (used for indexing to apppropriate function ptr)
    int get_opc_index(std::string opcname);
    // Returns a hashed value of opcode
    uint64_t get_opc_hash(std::string opcname);

    // static functions
    static int get_num_opcodes(){
        return powerpc_num_opcodes;
    }

};

#endif
