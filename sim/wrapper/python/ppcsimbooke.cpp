
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
#include <boost/shared_ptr.hpp>

#include "cpu_ppc.hpp"
#include "memory.hpp"

// Used as temporary string
std::string str_ref;

// Wrapping some cpu functions

// func ptrs for overloaded cpu::run_instrs()
int (cpu::*run_instr_ptr)(instr_call*) = &cpu::run_instr;
int (cpu::*run_instr_ptr2)(std::string, std::string, std::string, std::string, std::string,
        std::string, std::string) = &cpu::run_instr;

// func ptrs for overloaded cpu_ppc_booke::run_instrs()
int (cpu_ppc_booke::*run_instr_ptr_d0)(instr_call*) = &cpu_ppc_booke::run_instr;
int (cpu_ppc_booke::*run_instr_ptr2_d0)(std::string, std::string, std::string, std::string, std::string,
        std::string, std::string) = &cpu_ppc_booke::run_instr;

struct cpu_wrap : public cpu, public boost::python::wrapper<cpu>
{
    int run_instr(instr_call *ic){
        return this->get_override("run_instr")(ic);
    }
    int run_instr(std::string opcode, std::string arg0, std::string arg1, std::string arg2, std::string arg3,
            std::string arg4, std::string arg5){
        return this->get_override("run_instr")(opcode, arg0, arg1, arg2, arg3, arg4, arg5);
    }
    int xlate_v2p(uint64_t vaddr, uint64_t *paddr, int flags){
        return this->get_override("xlate_v2p")(vaddr, paddr, flags);
    }
    cpu_wrap(uint64_t cpuid, std::string name, uint64_t pc) : cpu(cpuid, name, pc){
    }
};

// Overloads for cpu_ppc_booke::dump_state()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dump_state_overloads, dump_state, 0, 3);
// Overloads for cpu_ppc_booke::run_instr()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(run_instr_overloads, run_instr, 1, 7);

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

BOOST_PYTHON_MODULE(ppcsim)
{
    using namespace boost::python;

    // Wrap log_to_file()
    def("log_to_file", log_to_file);

    class_<instr_call>("instr_call")
        .def_readwrite("opcode", &instr_call::opcode)
        .add_property("arg0",    &instr_call::getarg0, &instr_call::setarg0)
        .add_property("arg1",    &instr_call::getarg1, &instr_call::setarg1)
        .add_property("arg2",    &instr_call::getarg2, &instr_call::setarg2)
        .add_property("arg3",    &instr_call::getarg3, &instr_call::setarg3)
        .add_property("arg4",    &instr_call::getarg4, &instr_call::setarg4)
        .add_property("arg5",    &instr_call::getarg5, &instr_call::setarg5)
        .def("dump_state",       &instr_call::dump_state)
        ;

    class_<memory, boost::shared_ptr<memory> >("memory", no_init)
        .def("create",    &memory::initialize_memory).staticmethod("create")
        .def("destroy",   &memory::destroy_memory).staticmethod("destroy")
        .def("reg_tgt",   &memory::register_memory_target, register_memory_target_overloads())
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

    class_<cpu_wrap, boost::noncopyable>("cpu", init<uint64_t, std::string, uint64_t>())
        .def("run_instr", pure_virtual(run_instr_ptr))
        .def("run_instr", pure_virtual(run_instr_ptr2))
        .def("xlate_v2p", pure_virtual(&cpu::xlate_v2p))
        ;

    class_<cpu_ppc_booke> cpu_ppc_py("cpu_ppc", no_init);
    cpu_ppc_py.def("create",      &cpu_ppc_booke::create, return_value_policy<manage_new_object>()).staticmethod("create")
        .def("destroy",     &cpu_ppc_booke::destroy).staticmethod("destroy")
        .def("run_instr",   run_instr_ptr_d0)
        .def("run_instr",   run_instr_ptr2_d0, run_instr_overloads())
        .def("get_reg",     &cpu_ppc_booke::get_reg)
        .def("dump_state",  &cpu_ppc_booke::dump_state, dump_state_overloads())
        ;

    // Add attributes for GPRs
#define ADD_REG(reg_num, reg_alias)    cpu_ppc_py.add_property(reg_alias,  &cpu_ppc_booke::___get_reg<reg_num>, &cpu_ppc_booke::___set_reg<reg_num>)
#define ADD_REGR(reg_num, reg_alias)   cpu_ppc_py.add_property(reg_alias,  &cpu_ppc_booke::___get_reg<reg_num>)

    ADD_REG(REG_GPR0,  "_R0"  ); ADD_REG(REG_GPR1,  "_R1"  ); ADD_REG(REG_GPR2,  "_R2"  ); ADD_REG(REG_GPR3,  "_R3"  );
    ADD_REG(REG_GPR4,  "_R4"  ); ADD_REG(REG_GPR5,  "_R5"  ); ADD_REG(REG_GPR6,  "_R6"  ); ADD_REG(REG_GPR7,  "_R7"  );
    ADD_REG(REG_GPR8,  "_R8"  ); ADD_REG(REG_GPR9,  "_R9"  ); ADD_REG(REG_GPR10, "_R10" ); ADD_REG(REG_GPR11, "_R11" );
    ADD_REG(REG_GPR12, "_R12" ); ADD_REG(REG_GPR13, "_R13" ); ADD_REG(REG_GPR14, "_R14" ); ADD_REG(REG_GPR15, "_R15" );
    ADD_REG(REG_GPR16, "_R16" ); ADD_REG(REG_GPR17, "_R17" ); ADD_REG(REG_GPR18, "_R18" ); ADD_REG(REG_GPR19, "_R19" );
    ADD_REG(REG_GPR20, "_R20" ); ADD_REG(REG_GPR21, "_R21" ); ADD_REG(REG_GPR22, "_R22" ); ADD_REG(REG_GPR23, "_R23" );
    ADD_REG(REG_GPR24, "_R24" ); ADD_REG(REG_GPR25, "_R25" ); ADD_REG(REG_GPR26, "_R26" ); ADD_REG(REG_GPR27, "_R27" );
    ADD_REG(REG_GPR28, "_R28" ); ADD_REG(REG_GPR29, "_R29" ); ADD_REG(REG_GPR30, "_R30" ); ADD_REG(REG_GPR31, "_R31" );

    // Add attributes for FPRS
    ADD_REG(REG_FPR0,  "_F0"  ); ADD_REG(REG_FPR1,  "_F1"  ); ADD_REG(REG_FPR2,  "_F2"  ); ADD_REG(REG_FPR3,  "_F3"  );
    ADD_REG(REG_FPR4,  "_F4"  ); ADD_REG(REG_FPR5,  "_F5"  ); ADD_REG(REG_FPR6,  "_F6"  ); ADD_REG(REG_FPR7,  "_F7"  );
    ADD_REG(REG_FPR8,  "_F8"  ); ADD_REG(REG_FPR9,  "_F9"  ); ADD_REG(REG_FPR10, "_F10" ); ADD_REG(REG_FPR11, "_F11" );
    ADD_REG(REG_FPR12, "_F12" ); ADD_REG(REG_FPR13, "_F13" ); ADD_REG(REG_FPR14, "_F14" ); ADD_REG(REG_FPR15, "_F15" );
    ADD_REG(REG_FPR16, "_F16" ); ADD_REG(REG_FPR17, "_F17" ); ADD_REG(REG_FPR18, "_F18" ); ADD_REG(REG_FPR19, "_F19" );
    ADD_REG(REG_FPR20, "_F20" ); ADD_REG(REG_FPR21, "_F21" ); ADD_REG(REG_FPR22, "_F22" ); ADD_REG(REG_FPR23, "_F23" );
    ADD_REG(REG_FPR24, "_F24" ); ADD_REG(REG_FPR25, "_F25" ); ADD_REG(REG_FPR26, "_F26" ); ADD_REG(REG_FPR27, "_F27" );
    ADD_REG(REG_FPR28, "_F28" ); ADD_REG(REG_FPR29, "_F29" ); ADD_REG(REG_FPR30, "_F30" ); ADD_REG(REG_FPR31, "_F31" );

    //// Add attributes for other registers
    ADD_REG(REG_PC,      "_PC"     ); ADD_REG(REG_MSR,     "_MSR"     ); ADD_REG(REG_CR,      "_CR"     ); ADD_REG(REG_FPSCR,    "_FPSCR"  );
    ADD_REGR(REG_ATBL,   "_ATBL"   ); ADD_REGR(REG_ATBU,   "_ATBU"    ); ADD_REG(REG_CSRR0,   "_CSRR0"  ); ADD_REG(REG_CSRR1,    "_CSRR1"  );
    ADD_REG(REG_CTR,     "_CTR"    ); ADD_REG(REG_DAC1,    "_DAC1"    ); ADD_REG(REG_DAC2,    "_DAC2"   ); ADD_REG(REG_DBCR0,    "_DBCR0"  );
    ADD_REG(REG_DBCR1,   "_DBCR1"  ); ADD_REG(REG_DBCR2,   "_DBCR2"   ); ADD_REG(REG_DBSR,    "_DBSR"   ); ADD_REG(REG_DEAR,     "_DEAR"   );
    ADD_REG(REG_DEC,     "_DEC"    ); ADD_REG(REG_DECAR,   "_DECAR"   ); ADD_REG(REG_ESR,     "_ESR"    ); ADD_REG(REG_IAC1,     "_IAC1"   ); 
    ADD_REG(REG_IAC2,    "_IAC2"   ); ADD_REG(REG_IVOR0,   "_IVOR0"   ); ADD_REG(REG_IVOR1,   "_IVOR1"  ); ADD_REG(REG_IVOR2,    "_IVOR2"  );
    ADD_REG(REG_IVOR3,   "_IVOR3"  ); ADD_REG(REG_IVOR4,   "_IVOR4"   ); ADD_REG(REG_IVOR5,   "_IVOR5"  ); ADD_REG(REG_IVOR6,    "_IVOR6"  );
    ADD_REG(REG_IVOR8,   "_IVOR8"  ); ADD_REG(REG_IVOR10,  "_IVOR10"  ); ADD_REG(REG_IVOR11,  "_IVOR11" ); ADD_REG(REG_IVOR12,   "_IVOR12" );
    ADD_REG(REG_IVOR13,  "_IVOR13" ); ADD_REG(REG_IVOR14,  "_IVOR14"  ); ADD_REG(REG_IVOR15,  "_IVOR15" ); ADD_REG(REG_IVPR,     "_IVPR"   );
    ADD_REG(REG_LR,      "_LR"     ); ADD_REG(REG_PID,     "_PID"     ); ADD_REGR(REG_PIR,    "_PIR"    ); ADD_REGR(REG_PVR,     "_PVR"    ); 
    ADD_REG(REG_SPRG0,   "_SPRG0"  ); ADD_REG(REG_SPRG1,   "_SPRG1"   ); ADD_REG(REG_SPRG2,   "_SPRG2"  ); ADD_REGR(REG_SPRG3R,  "_SPRG3R" );
    ADD_REG(REG_SPRG3,   "_SPRG3"  ); ADD_REGR(REG_SPRG4R, "_SPRG4R"  ); ADD_REG(REG_SPRG4,   "_SPRG4"  ); ADD_REGR(REG_SPRG5R,  "_SPRG5R" );
    ADD_REG(REG_SPRG5,   "_SPRG5"  ); ADD_REGR(REG_SPRG6R, "_SPRG6R"  ); ADD_REG(REG_SPRG6,   "_SPRG6"  ); ADD_REGR(REG_SPRG7R,  "_SPRG7R" );
    ADD_REG(REG_SPRG7,   "_SPRG7"  ); ADD_REG(REG_SRR0,    "_SRR0"    ); ADD_REG(REG_SRR1,    "_SRR1"   ); ADD_REGR(REG_TBRL,    "_TBRL"   );
    ADD_REG(REG_TBWL,    "_TBWL"   ); ADD_REGR(REG_TBRU,   "_TBRU"    ); ADD_REG(REG_TBWU,    "_TBWU"   ); ADD_REG(REG_TCR,      "_TCR"    );   
    ADD_REG(REG_TSR,     "_TSR"    ); ADD_REG(REG_USPRG0,  "_USPRG0"  ); ADD_REG(REG_XER,     "_XER"    ); ADD_REG(REG_BBEAR,    "_BBEAR"  );
    ADD_REG(REG_BBTAR,   "_BBTAR"  ); ADD_REG(REG_BUCSR,   "_BUCSR"   ); ADD_REG(REG_HID0,    "_HID0"   ); ADD_REG(REG_HID1,     "_HID1"   );
    ADD_REG(REG_IVOR32,  "_IVOR32" ); ADD_REG(REG_IVOR33,  "_IVOR33"  ); ADD_REG(REG_IVOR34,  "_IVOR34" ); ADD_REG(REG_IVOR35,   "_IVOR35" );
    ADD_REGR(REG_L1CFG0, "_L1CFG0" ); ADD_REGR(REG_L1CFG1, "_L1CFG1"  ); ADD_REG(REG_L1CSR0,  "_L1CSR0" ); ADD_REG(REG_L1CSR1,   "_L1CSR1" );
    ADD_REG(REG_MAS0,    "_MAS0"   ); ADD_REG(REG_MAS1,    "_MAS1"    ); ADD_REG(REG_MAS2,    "_MAS2"   ); ADD_REG(REG_MAS3,     "_MAS3"   );
    ADD_REG(REG_MAS4,    "_MAS4"   ); ADD_REG(REG_MAS5,    "_MAS5"    ); ADD_REG(REG_MAS6,    "_MAS6"   ); ADD_REG(REG_MAS7,     "_MAS7"   );
    ADD_REGR(REG_MCAR,   "_MCAR"   ); ADD_REGR(REG_MCSR,   "_MCSR"    ); ADD_REG(REG_MCSRR0,  "_MCSRR0" ); ADD_REG(REG_MCSRR1,   "_MCSRR1" );
    ADD_REGR(REG_MMUCFG, "_MMUCFG" ); ADD_REG(REG_MMUCSR0, "_MMUCSR0" ); ADD_REG(REG_PID0,    "_PID0"   ); ADD_REG(REG_PID1,     "_PID1"   ); 
    ADD_REG(REG_PID2,    "_PID2"   ); ADD_REG(REG_SPEFSCR, "_SPEFSCR" ); ADD_REG(REG_SVR,     "_SVR"    ); ADD_REGR(REG_TLB0CFG, "_TLB0CFG");
    ADD_REG(REG_TLB1CFG, "_TLB1CFG");

}
