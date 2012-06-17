
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
        .def("reg",     &cpu_ppc_booke::get_reg)
        .def("dump_state",  &cpu_ppc_booke::dump_state, dump_state_overloads())
        ;

    // Add attributes for GPRs
    str_ref = "r0";  cpu_ppc_py.add_property("R0",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r1";  cpu_ppc_py.add_property("R1",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r2";  cpu_ppc_py.add_property("R2",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r3";  cpu_ppc_py.add_property("R3",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r4";  cpu_ppc_py.add_property("R4",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r5";  cpu_ppc_py.add_property("R5",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r6";  cpu_ppc_py.add_property("R6",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r7";  cpu_ppc_py.add_property("R7",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r8";  cpu_ppc_py.add_property("R8",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r9";  cpu_ppc_py.add_property("R9",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r10"; cpu_ppc_py.add_property("R10", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r11"; cpu_ppc_py.add_property("R11", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r12"; cpu_ppc_py.add_property("R12", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r13"; cpu_ppc_py.add_property("R13", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r14"; cpu_ppc_py.add_property("R14", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r15"; cpu_ppc_py.add_property("R15", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r16"; cpu_ppc_py.add_property("R16", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r17"; cpu_ppc_py.add_property("R17", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r18"; cpu_ppc_py.add_property("R18", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r19"; cpu_ppc_py.add_property("R19", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r20"; cpu_ppc_py.add_property("R20", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r21"; cpu_ppc_py.add_property("R21", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r22"; cpu_ppc_py.add_property("R22", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r23"; cpu_ppc_py.add_property("R23", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r24"; cpu_ppc_py.add_property("R24", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r25"; cpu_ppc_py.add_property("R25", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r26"; cpu_ppc_py.add_property("R26", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r27"; cpu_ppc_py.add_property("R27", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r28"; cpu_ppc_py.add_property("R28", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r29"; cpu_ppc_py.add_property("R29", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r30"; cpu_ppc_py.add_property("R30", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "r31"; cpu_ppc_py.add_property("R31", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);

    // Add attributes for FPRS
    str_ref = "f0";  cpu_ppc_py.add_property("F0",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f1";  cpu_ppc_py.add_property("F1",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f2";  cpu_ppc_py.add_property("F2",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f3";  cpu_ppc_py.add_property("F3",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f4";  cpu_ppc_py.add_property("F4",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f5";  cpu_ppc_py.add_property("F5",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f6";  cpu_ppc_py.add_property("F6",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f7";  cpu_ppc_py.add_property("F7",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f8";  cpu_ppc_py.add_property("F8",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f9";  cpu_ppc_py.add_property("F9",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f10"; cpu_ppc_py.add_property("F10", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f11"; cpu_ppc_py.add_property("F11", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f12"; cpu_ppc_py.add_property("F12", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f13"; cpu_ppc_py.add_property("F13", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f14"; cpu_ppc_py.add_property("F14", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f15"; cpu_ppc_py.add_property("F15", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f16"; cpu_ppc_py.add_property("F16", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f17"; cpu_ppc_py.add_property("F17", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f18"; cpu_ppc_py.add_property("F18", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f19"; cpu_ppc_py.add_property("F19", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f20"; cpu_ppc_py.add_property("F20", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f21"; cpu_ppc_py.add_property("F21", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f22"; cpu_ppc_py.add_property("F22", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f23"; cpu_ppc_py.add_property("F23", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f24"; cpu_ppc_py.add_property("F24", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f25"; cpu_ppc_py.add_property("F25", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f26"; cpu_ppc_py.add_property("F26", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f27"; cpu_ppc_py.add_property("F27", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f28"; cpu_ppc_py.add_property("F28", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f29"; cpu_ppc_py.add_property("F29", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f30"; cpu_ppc_py.add_property("F30", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "f31"; cpu_ppc_py.add_property("F31", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);

    // Add attributes for other registers
    str_ref = "pc"      ; cpu_ppc_py.add_property("PC",      &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "msr"     ; cpu_ppc_py.add_property("MSR",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "cr"      ; cpu_ppc_py.add_property("CR",      &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "fpscr"   ; cpu_ppc_py.add_property("FPSCR",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "atbl"    ; cpu_ppc_py.add_property("ATBL",    &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "atbu"    ; cpu_ppc_py.add_property("ATBU",    &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "csrr0"   ; cpu_ppc_py.add_property("CSRR0",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "csrr1"   ; cpu_ppc_py.add_property("CSRR1",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ctr"     ; cpu_ppc_py.add_property("CTR",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dac1"    ; cpu_ppc_py.add_property("DAC1",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dac2"    ; cpu_ppc_py.add_property("DAC2",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dbcr0"   ; cpu_ppc_py.add_property("DBCR0",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dbcr1"   ; cpu_ppc_py.add_property("DBCR1",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dbcr2"   ; cpu_ppc_py.add_property("DBCR2",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dbsr"    ; cpu_ppc_py.add_property("DBSR",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dear"    ; cpu_ppc_py.add_property("DEAR",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "dec"     ; cpu_ppc_py.add_property("DEC",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "decar"   ; cpu_ppc_py.add_property("DECAR",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "esr"     ; cpu_ppc_py.add_property("ESR",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "iac1"    ; cpu_ppc_py.add_property("IAC1",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "iac2"    ; cpu_ppc_py.add_property("IAC2",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor0"   ; cpu_ppc_py.add_property("IVOR0",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor1"   ; cpu_ppc_py.add_property("IVOR1",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor2"   ; cpu_ppc_py.add_property("IVOR2",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor3"   ; cpu_ppc_py.add_property("IVOR3",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor4"   ; cpu_ppc_py.add_property("IVOR4",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor5"   ; cpu_ppc_py.add_property("IVOR5",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor6"   ; cpu_ppc_py.add_property("IVOR6",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor8"   ; cpu_ppc_py.add_property("IVOR8",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor10"  ; cpu_ppc_py.add_property("IVOR10",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor11"  ; cpu_ppc_py.add_property("IVOR11",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor12"  ; cpu_ppc_py.add_property("IVOR12",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor13"  ; cpu_ppc_py.add_property("IVOR13",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor14"  ; cpu_ppc_py.add_property("IVOR14",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor15"  ; cpu_ppc_py.add_property("IVOR15",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivpr"    ; cpu_ppc_py.add_property("IVPR",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "lr"      ; cpu_ppc_py.add_property("LR",      &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "pid"     ; cpu_ppc_py.add_property("PID",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "pir"     ; cpu_ppc_py.add_property("PIR",     &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "pvr"     ; cpu_ppc_py.add_property("PVR",     &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "sprg0"   ; cpu_ppc_py.add_property("SPRG0",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg1"   ; cpu_ppc_py.add_property("SPRG1",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg2"   ; cpu_ppc_py.add_property("SPRG2",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg3r"  ; cpu_ppc_py.add_property("SPRG3R",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "sprg3"   ; cpu_ppc_py.add_property("SPRG3",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg4r"  ; cpu_ppc_py.add_property("SPRG4R",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "sprg4"   ; cpu_ppc_py.add_property("SPRG4",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg5r"  ; cpu_ppc_py.add_property("SPRG5R",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "sprg5"   ; cpu_ppc_py.add_property("SPRG5",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg6r"  ; cpu_ppc_py.add_property("SPRG6R",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "sprg6"   ; cpu_ppc_py.add_property("SPRG6",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "sprg7r"  ; cpu_ppc_py.add_property("SPRG7R",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "sprg7"   ; cpu_ppc_py.add_property("SPRG7",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "srr0"    ; cpu_ppc_py.add_property("SRR0",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "srr1"    ; cpu_ppc_py.add_property("SRR1",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "tbrl"    ; cpu_ppc_py.add_property("TBRL",    &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "tbwl"    ; cpu_ppc_py.add_property("TBWL",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "tbru"    ; cpu_ppc_py.add_property("TBRU",    &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "tbwu"    ; cpu_ppc_py.add_property("TBWU",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "tcr"     ; cpu_ppc_py.add_property("TCR",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "tsr"     ; cpu_ppc_py.add_property("TSR",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "usprg0"  ; cpu_ppc_py.add_property("USPRG0",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "xer"     ; cpu_ppc_py.add_property("XER",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "bbear"   ; cpu_ppc_py.add_property("BBEAR",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "bbtar"   ; cpu_ppc_py.add_property("BBTAR",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "bucsr"   ; cpu_ppc_py.add_property("BUCSR",   &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "hid0"    ; cpu_ppc_py.add_property("HID0",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "hid1"    ; cpu_ppc_py.add_property("HID1",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor32"  ; cpu_ppc_py.add_property("IVOR32",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor33"  ; cpu_ppc_py.add_property("IVOR33",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor34"  ; cpu_ppc_py.add_property("IVOR34",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "ivor35"  ; cpu_ppc_py.add_property("IVOR35",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "l1cfg0"  ; cpu_ppc_py.add_property("L1CFG0",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "l1cfg1"  ; cpu_ppc_py.add_property("L1CFG1",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "l1csr0"  ; cpu_ppc_py.add_property("L1CSR0",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "l1csr1"  ; cpu_ppc_py.add_property("L1CSR1",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas0"    ; cpu_ppc_py.add_property("MAS0",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas1"    ; cpu_ppc_py.add_property("MAS1",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas2"    ; cpu_ppc_py.add_property("MAS2",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas3"    ; cpu_ppc_py.add_property("MAS3",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas4"    ; cpu_ppc_py.add_property("MAS4",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas5"    ; cpu_ppc_py.add_property("MAS5",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas6"    ; cpu_ppc_py.add_property("MAS6",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mas7"    ; cpu_ppc_py.add_property("MAS7",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mcar"    ; cpu_ppc_py.add_property("MCAR",    &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "mcsr"    ; cpu_ppc_py.add_property("MCSR",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mcsrr0"  ; cpu_ppc_py.add_property("MCSRR0",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mcsrr1"  ; cpu_ppc_py.add_property("MCSRR1",  &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "mmucfg"  ; cpu_ppc_py.add_property("MMUCFG",  &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "mmucsr0" ; cpu_ppc_py.add_property("MMUCSR0", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "pid0"    ; cpu_ppc_py.add_property("PID0",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "pid1"    ; cpu_ppc_py.add_property("PID1",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "pid2"    ; cpu_ppc_py.add_property("PID2",    &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "spefscr" ; cpu_ppc_py.add_property("SPEFSCR", &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "svr"     ; cpu_ppc_py.add_property("SVR",     &cpu_ppc_booke::___get_reg<str_ref>, &cpu_ppc_booke::___set_reg<str_ref>);
    str_ref = "tlb0cfg" ; cpu_ppc_py.add_property("TLB0CFG", &cpu_ppc_booke::___get_reg<str_ref>);
    str_ref = "tlb1cfg" ; cpu_ppc_py.add_property("TLB1CFG", &cpu_ppc_booke::___get_reg<str_ref>);

}
