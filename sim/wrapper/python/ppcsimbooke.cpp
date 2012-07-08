
// ppcsimbooke.cpp ( python wrapper for our simulator )
// This file contains wrappping macros and classes for our simulator modules
//
// Author : Vikas Chouhan ( presentisgood@gmail.com )
// Copyright 2012.
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

#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>
#include <boost/shared_ptr.hpp>


#include "cpu_ppc.hpp"
#include "memory.hpp"
#include "machine.hpp"
#include "ppc_dis.hpp"
 
 
// Wrapping some cpu functions
 
// func ptrs for overloaded cpu::run_instrs()
int (cpu::*run_instr_ptr)(instr_call*) = &cpu::run_instr;
int (cpu::*run_instr_ptr2)(std::string) = &cpu::run_instr;

// func ptrs for overloaded cpu_ppc_booke::run_instrs()
int (cpu_ppc_booke::*run_instr_ptr_d0)(instr_call*) = &cpu_ppc_booke::run_instr;
int (cpu_ppc_booke::*run_instr_ptr2_d0)(std::string) = &cpu_ppc_booke::run_instr;

// Wrapping some ppc_dis functions
instr_call (ppc_dis_booke::*disasm_ptr)(uint32_t, int) = &ppc_dis_booke::disasm;
instr_call (ppc_dis_booke::*disasm_ptr2)(std::string)  = &ppc_dis_booke::disasm;

struct cpu_wrap : public cpu, public boost::python::wrapper<cpu>
{
    int run_instr(instr_call *ic){
        return this->get_override("run_instr")(ic);
    }
    int run_instr(std::string instr){
        return this->get_override("run_instr")(instr);
    }
    int xlate_v2p(uint64_t vaddr, uint64_t *paddr, int flags){
        return this->get_override("xlate_v2p")(vaddr, paddr, flags);
    }
    cpu_wrap(uint64_t cpuid, std::string name, uint64_t pc) : cpu(cpuid, name, pc){
    }
};

// Overloads for cpu_ppc_booke::dump_state()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dump_state_overloads, dump_state, 0, 3);

// Overloads for memory::register_memory_target()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(register_memory_target_overloads, register_memory_target, 5, 6);
// Overloads for memory::dump_all_pages()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dump_all_pages_overloads, dump_all_pages, 0, 1);
// Overloads for memory::read16()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(read16_overloads, read16, 1, 2);
// Overloads for memory::write16()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(write16_overloads, write16, 2, 3);
// Overloads for memory::read32()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(read32_overloads, read32, 1, 2);
// Overloads for memory::write32()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(write32_overloads, write32, 2, 3);
// Overloads for memory::read64()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(read64_overloads, read64, 1, 2);
// Overloads for memory::write64()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(write64_overloads, write64, 2, 3);

// Overloads for ppc_dis::disasm()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(disasm_overloads, disasm, 1, 2);

// Dummy class to be used for adding extra classes in python proxies
template <int x> class dummy{
    private:
    dummy(){}
};

// A template class which instantiates a new uint64_t static const variable based on a constant
template <uint64_t x> struct new_const{
    static const uint64_t val;
};
template<uint64_t x> const uint64_t new_const<x>::val = x;


// Add attributes for GPRs
#define ADD_GPR(reg_num, reg_alias) \
    ppc_regs_py.add_property(reg_alias, make_function(&ppc_regs::get_gpr<reg_num>, return_value_policy<reference_existing_object>()))
#define ADD_FPR(reg_num, reg_alias) \
    ppc_regs_py.add_property(reg_alias, make_function(&ppc_regs::get_fpr<reg_num>, return_value_policy<reference_existing_object>()))
#define ADD_SPR(reg_num, reg_alias) \
    ppc_regs_py.add_property(reg_alias, make_function(&ppc_regs::get_spr<reg_num>, return_value_policy<reference_existing_object>()))
#define ADD_PMR(reg_num, reg_alias) \
    ppc_regs_py.add_property(reg_alias, make_function(&ppc_regs::get_pmr<reg_num>, return_value_policy<reference_existing_object>()))

// Add all registers
#define ADD_ALL_REGS() \
       ADD_GPR(0,  "R0"  ); ADD_GPR(1,  "R1"  ); ADD_GPR(2,  "R2"  ); ADD_GPR(3,  "R3"  );  \
       ADD_GPR(4,  "R4"  ); ADD_GPR(5,  "R5"  ); ADD_GPR(6,  "R6"  ); ADD_GPR(7,  "R7"  );  \
       ADD_GPR(8,  "R8"  ); ADD_GPR(9,  "R9"  ); ADD_GPR(10, "R10" ); ADD_GPR(11, "R11" );  \
       ADD_GPR(12, "R12" ); ADD_GPR(13, "R13" ); ADD_GPR(14, "R14" ); ADD_GPR(15, "R15" );  \
       ADD_GPR(16, "R16" ); ADD_GPR(17, "R17" ); ADD_GPR(18, "R18" ); ADD_GPR(19, "R19" );  \
       ADD_GPR(20, "R20" ); ADD_GPR(21, "R21" ); ADD_GPR(22, "R22" ); ADD_GPR(23, "R23" );  \
       ADD_GPR(24, "R24" ); ADD_GPR(25, "R25" ); ADD_GPR(26, "R26" ); ADD_GPR(27, "R27" );  \
       ADD_GPR(28, "R28" ); ADD_GPR(29, "R29" ); ADD_GPR(30, "R30" ); ADD_GPR(31, "R31" );  \
                                                                                            \
      /* Add attributes for FPRS */                                                         \
      ADD_FPR(0,  "F0"  ); ADD_FPR(1,  "F1"  ); ADD_FPR(2,  "F2"  ); ADD_FPR(3,  "F3"  );   \
      ADD_FPR(4,  "F4"  ); ADD_FPR(5,  "F5"  ); ADD_FPR(6,  "F6"  ); ADD_FPR(7,  "F7"  );   \
      ADD_FPR(8,  "F8"  ); ADD_FPR(9,  "F9"  ); ADD_FPR(10, "F10" ); ADD_FPR(11, "F11" );   \
      ADD_FPR(12, "F12" ); ADD_FPR(13, "F13" ); ADD_FPR(14, "F14" ); ADD_FPR(15, "F15" );   \
      ADD_FPR(16, "F16" ); ADD_FPR(17, "F17" ); ADD_FPR(18, "F18" ); ADD_FPR(19, "F19" );   \
      ADD_FPR(20, "F20" ); ADD_FPR(21, "F21" ); ADD_FPR(22, "F22" ); ADD_FPR(23, "F23" );   \
      ADD_FPR(24, "F24" ); ADD_FPR(25, "F25" ); ADD_FPR(26, "F26" ); ADD_FPR(27, "F27" );   \
      ADD_FPR(28, "F28" ); ADD_FPR(29, "F29" ); ADD_FPR(30, "F30" ); ADD_FPR(31, "F31" );   \
                                                                                            \
      /* Add attributes for other registers */                                              \
      ADD_SPR(SPRN_ATBL,    "ATBL"   ); ADD_SPR(SPRN_ATBU,    "ATBU"    ); ADD_SPR(SPRN_CSRR0,   "CSRR0"  ); ADD_SPR(SPRN_CSRR1,    "CSRR1"  );    \
      ADD_SPR(SPRN_CTR,     "CTR"    ); ADD_SPR(SPRN_DAC1,    "DAC1"    ); ADD_SPR(SPRN_DAC2,    "DAC2"   ); ADD_SPR(SPRN_DBCR0,    "DBCR0"  );    \
      ADD_SPR(SPRN_DBCR1,   "DBCR1"  ); ADD_SPR(SPRN_DBCR2,   "DBCR2"   ); ADD_SPR(SPRN_DBSR,    "DBSR"   ); ADD_SPR(SPRN_DEAR,     "DEAR"   );    \
      ADD_SPR(SPRN_DEC,     "DEC"    ); ADD_SPR(SPRN_DECAR,   "DECAR"   ); ADD_SPR(SPRN_ESR,     "ESR"    ); ADD_SPR(SPRN_IAC1,     "IAC1"   );    \
      ADD_SPR(SPRN_IAC2,    "IAC2"   ); ADD_SPR(SPRN_IVOR0,   "IVOR0"   ); ADD_SPR(SPRN_IVOR1,   "IVOR1"  ); ADD_SPR(SPRN_IVOR2,    "IVOR2"  );    \
      ADD_SPR(SPRN_IVOR3,   "IVOR3"  ); ADD_SPR(SPRN_IVOR4,   "IVOR4"   ); ADD_SPR(SPRN_IVOR5,   "IVOR5"  ); ADD_SPR(SPRN_IVOR6,    "IVOR6"  );    \
      ADD_SPR(SPRN_IVOR8,   "IVOR8"  ); ADD_SPR(SPRN_IVOR10,  "IVOR10"  ); ADD_SPR(SPRN_IVOR11,  "IVOR11" ); ADD_SPR(SPRN_IVOR12,   "IVOR12" );    \
      ADD_SPR(SPRN_IVOR13,  "IVOR13" ); ADD_SPR(SPRN_IVOR14,  "IVOR14"  ); ADD_SPR(SPRN_IVOR15,  "IVOR15" ); ADD_SPR(SPRN_IVPR,     "IVPR"   );    \
      ADD_SPR(SPRN_LR,      "LR"     ); ADD_SPR(SPRN_PID,     "PID"     ); ADD_SPR(SPRN_PIR,     "PIR"    ); ADD_SPR(SPRN_PVR,      "PVR"    );    \
      ADD_SPR(SPRN_SPRG0,   "SPRG0"  ); ADD_SPR(SPRN_SPRG1,   "SPRG1"   ); ADD_SPR(SPRN_SPRG2,   "SPRG2"  ); ADD_SPR(SPRN_SPRG3R,   "SPRG3R" );    \
      ADD_SPR(SPRN_SPRG3,   "SPRG3"  ); ADD_SPR(SPRN_SPRG4R,  "SPRG4R"  ); ADD_SPR(SPRN_SPRG4,   "SPRG4"  ); ADD_SPR(SPRN_SPRG5R,   "SPRG5R" );    \
      ADD_SPR(SPRN_SPRG5,   "SPRG5"  ); ADD_SPR(SPRN_SPRG6R,  "SPRG6R"  ); ADD_SPR(SPRN_SPRG6,   "SPRG6"  ); ADD_SPR(SPRN_SPRG7R,   "SPRG7R" );    \
      ADD_SPR(SPRN_SPRG7,   "SPRG7"  ); ADD_SPR(SPRN_SRR0,    "SRR0"    ); ADD_SPR(SPRN_SRR1,    "SRR1"   ); ADD_SPR(SPRN_TBRL,     "TBRL"   );    \
      ADD_SPR(SPRN_TBWL,    "TBWL"   ); ADD_SPR(SPRN_TBRU,    "TBRU"    ); ADD_SPR(SPRN_TBWU,    "TBWU"   ); ADD_SPR(SPRN_TCR,      "TCR"    );    \
      ADD_SPR(SPRN_TSR,     "TSR"    ); ADD_SPR(SPRN_USPRG0,  "USPRG0"  ); ADD_SPR(SPRN_XER,     "XER"    ); ADD_SPR(SPRN_BBEAR,    "BBEAR"  );    \
      ADD_SPR(SPRN_BBTAR,   "BBTAR"  ); ADD_SPR(SPRN_BUCSR,   "BUCSR"   ); ADD_SPR(SPRN_HID0,    "HID0"   ); ADD_SPR(SPRN_HID1,     "HID1"   );    \
      ADD_SPR(SPRN_IVOR32,  "IVOR32" ); ADD_SPR(SPRN_IVOR33,  "IVOR33"  ); ADD_SPR(SPRN_IVOR34,  "IVOR34" ); ADD_SPR(SPRN_IVOR35,   "IVOR35" );    \
      ADD_SPR(SPRN_L1CFG0,  "L1CFG0" ); ADD_SPR(SPRN_L1CFG1,  "L1CFG1"  ); ADD_SPR(SPRN_L1CSR0,  "L1CSR0" ); ADD_SPR(SPRN_L1CSR1,   "L1CSR1" );    \
      ADD_SPR(SPRN_MAS0,    "MAS0"   ); ADD_SPR(SPRN_MAS1,    "MAS1"    ); ADD_SPR(SPRN_MAS2,    "MAS2"   ); ADD_SPR(SPRN_MAS3,     "MAS3"   );    \
      ADD_SPR(SPRN_MAS4,    "MAS4"   ); ADD_SPR(SPRN_MAS5,    "MAS5"    ); ADD_SPR(SPRN_MAS6,    "MAS6"   ); ADD_SPR(SPRN_MAS7,     "MAS7"   );    \
      ADD_SPR(SPRN_MCAR,    "MCAR"   ); ADD_SPR(SPRN_MCSR,    "MCSR"    ); ADD_SPR(SPRN_MCSRR0,  "MCSRR0" ); ADD_SPR(SPRN_MCSRR1,   "MCSRR1" );    \
      ADD_SPR(SPRN_MMUCFG,  "MMUCFG" ); ADD_SPR(SPRN_MMUCSR0, "MMUCSR0" ); ADD_SPR(SPRN_PID0,    "PID0"   ); ADD_SPR(SPRN_PID1,     "PID1"   );    \
      ADD_SPR(SPRN_PID2,    "PID2"   ); ADD_SPR(SPRN_SPEFSCR, "SPEFSCR" ); ADD_SPR(SPRN_SVR,     "SVR"    ); ADD_SPR(SPRN_TLB0CFG,  "TLB0CFG");    \
      ADD_SPR(SPRN_TLB1CFG, "TLB1CFG")


// Main module
BOOST_PYTHON_MODULE(ppcsim)
{
    using namespace boost::python;

    // Wrap log_to_file()
    def("log_to_file", log_to_file);

    // Types namespace ( defines all class types being used in our module, one way of other )
    {
        class_<dummy<0> > types_py("types", no_init);
        scope types_scope = types_py;

        // Endianness attributes
        types_py.def_readonly("EMUL_UNDEFINED_ENDIAN", &EMUL_UNDEFINED_ENDIAN)
                .def_readonly("EMUL_LITTLE_ENDIAN",    &EMUL_LITTLE_ENDIAN)
                .def_readonly("EMUL_BIG_ENDIAN",       &EMUL_BIG_ENDIAN)
                ;

        // Disassembler class type
        class_<ppc_dis_booke>("ppc_dis")
            .def("disasm", disasm_ptr, disasm_overloads())
            .def("disasm", disasm_ptr2) 
            ;

        // instr_call type ( We will probably never use this directly )
        class_<instr_call>("instr_call")
            .def_readwrite("opcode", &instr_call::opcode)
            .add_property("arg0",    &instr_call::getarg<0>, &instr_call::setarg<0>)
            .add_property("arg1",    &instr_call::getarg<1>, &instr_call::setarg<1>)
            .add_property("arg2",    &instr_call::getarg<2>, &instr_call::setarg<2>)
            .add_property("arg3",    &instr_call::getarg<3>, &instr_call::setarg<3>)
            .add_property("arg4",    &instr_call::getarg<4>, &instr_call::setarg<4>)
            .add_property("arg5",    &instr_call::getarg<5>, &instr_call::setarg<5>)
            .def("dump_state",       &instr_call::dump_state)
            .def("print_instr",      &instr_call::print_instr)
            ;

        // ppc register type ( 64 bit )
        class_<ppc_reg64>("ppc_reg64")
            .def_readwrite("value", &ppc_reg64::value)
            .def_readonly("attr",   &ppc_reg64::attr)
            .def("set_bf",          &ppc_reg64::set_bf)
            .def("get_bf",          &ppc_reg64::get_bf)
            ;

        // PPC register file type ( contains all registers, GPRs, FPRs, SPRs etc. )
        class_<ppc_regs> ppc_regs_py("ppc_regs");
        ppc_regs_py.add_property("MSR", &ppc_regs::msr)
            .add_property("CR",  &ppc_regs::cr)
            ;
        ADD_ALL_REGS();

        // Memory namespace
        {
            class_<memory> memory_py("memory", init<uint64_t>());
            scope memory_scope = memory_py;
            memory_py.def("reg_tgt",   &memory::register_memory_target, register_memory_target_overloads())
                .def("dump_tgts", &memory::dump_all_memory_targets)
                .def("dump_page_maps", &memory::dump_all_page_maps)
                .def("dump_pages", &memory::dump_all_pages, dump_all_pages_overloads())
                //.def("write_buf",  &memory::write_from_buffer)
                //.def("read_buf",   &memory::read_to_buffer, return_value_policy<manage_new_object>())
                .def("read8",        &memory::read8)
                .def("write8",       &memory::write8)
                .def("read16",       &memory::read16, read16_overloads())
                .def("write16",      &memory::write16, write16_overloads())
                .def("read32",       &memory::read32, read32_overloads())
                .def("write32",      &memory::write32, write32_overloads())
                .def("read64",       &memory::read64, read64_overloads())
                .def("write64",      &memory::write64, write64_overloads())
                ;

            // Memory targets namespace ( defines constants for memory targets )
            {
                class_<dummy<1> > mem_tgts_py("tgts", no_init);
                scope mem_tgts_scope = mem_tgts_py;
                mem_tgts_py.def_readonly("TGT_DDR", &TGT_DDR)
                    .def_readonly("TGT_CCSR", &TGT_CCSR)
                    .def_readonly("TGT_IFC", &TGT_IFC)
                    ;
            }
        }

        // The abstract cpu class
        class_<cpu_wrap, boost::noncopyable>("cpu", init<uint64_t, std::string, uint64_t>())
            .def("run_instr", pure_virtual(run_instr_ptr))
            .def("run_instr", pure_virtual(run_instr_ptr2))
            .def("xlate_v2p", pure_virtual(&cpu::xlate_v2p))
            ;

        // The derived cpu_ppc_book class ( Our main cpu class )
        class_<cpu_ppc_booke> cpu_ppc_py("cpu_ppc", init<uint64_t, std::string>());
        cpu_ppc_py.def("run_instr",   run_instr_ptr_d0)
            .def("run_instr",         run_instr_ptr2_d0)
            .def("get_reg",           &cpu_ppc_booke::get_reg)
            .def("dump_state",        &cpu_ppc_booke::dump_state, dump_state_overloads())
            .def("init_reg_attrs",    &cpu_ppc_booke::init_reg_attrs)
            .add_property("regs",     make_function(&cpu_ppc_booke::___get_regs, return_value_policy<reference_existing_object>()))
            .add_property("PC",       &cpu_ppc_booke::get_pc)
            ;


    }

    // Class machine ( Our top level machine class. This is the class we are going to use directly. )
    class_<machine<2> > machine_py("machine");
    machine_py.def_readonly("ncpus", &machine<2>::m_ncpus)
        .def_readwrite("memory", &machine<2>::m_memory)
        .add_property("cpu0", make_function(&machine<2>::get_cpu<0>, return_value_policy<reference_existing_object>()))
        .add_property("cpu1", make_function(&machine<2>::get_cpu<1>, return_value_policy<reference_existing_object>()))
        ;
    
}
