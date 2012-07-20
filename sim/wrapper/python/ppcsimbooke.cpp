
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
 
#define  SIM_REL  "current"

// Wrapping some cpu functions
 
// func ptrs for overloaded cpu::run_instrs()
int (cpu::*run_instr_ptr)(instr_call&) = &cpu::run_instr;
int (cpu::*run_instr_ptr2)(std::string) = &cpu::run_instr;

// func ptrs for overloaded cpu_ppc_booke::run_instrs()
int (cpu_ppc_booke::*run_instr_ptr_d0)(instr_call&) = &cpu_ppc_booke::run_instr;
int (cpu_ppc_booke::*run_instr_ptr2_d0)(std::string) = &cpu_ppc_booke::run_instr;

// Wrapping some ppc_dis functions
instr_call (ppc_dis_booke::*disasm_ptr)(uint32_t, int) = &ppc_dis_booke::disasm;
instr_call (ppc_dis_booke::*disasm_ptr2)(std::string)  = &ppc_dis_booke::disasm;

struct cpu_wrap : public cpu, public boost::python::wrapper<cpu>
{
    int run_instr(instr_call &ic){
        return this->get_override("run_instr")(ic);
    }
    int run_instr(std::string instr){
        return this->get_override("run_instr")(instr);
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

// Add bitmask constants
#define ADD_BMASK(mask, mask_alias)  bitmasks_py.def_readonly(mask_alias, &new_const<mask>::val)
#define ADD_BITMASKS()    \
      /* MSR */                                                                                                                                                \
      ADD_BMASK(MSR_UCLE,   "MSR_UCLE"  );  ADD_BMASK(MSR_SPE,    "MSR_SPE"   );  ADD_BMASK(MSR_WE,     "MSR_WE"    );  ADD_BMASK(MSR_CE,     "MSR_CE"    );   \
      ADD_BMASK(MSR_EE,     "MSR_EE"    );  ADD_BMASK(MSR_PR,     "MSR_PR"    );  ADD_BMASK(MSR_FP,     "MSR_FP"    );  ADD_BMASK(MSR_ME,     "MSR_ME"    );   \
      ADD_BMASK(MSR_UBLE,   "MSR_UBLE"  );  ADD_BMASK(MSR_DE,     "MSR_DE"    );  ADD_BMASK(MSR_IS,     "MSR_IS"    );  ADD_BMASK(MSR_DS,     "MSR_DS"    );   \
      ADD_BMASK(MSR_PMM,    "MSR_PMM"   );                                                                                                                     \
      /* DBSR . Many bitfields are having slightly diff names in the linux' header files */                                                                    \
      ADD_BMASK(0x80000000, "DBSR_IDE"  );  ADD_BMASK(DBSR_UDE,   "DBSR_UDE"  );  ADD_BMASK(0x30000000, "DBSR_MRR"  );  ADD_BMASK(DBSR_IC,    "DBSR_ICMP" );   \
      ADD_BMASK(DBSR_BT,    "DBSR_BRT"  );  ADD_BMASK(DBSR_IRPT,  "DBSR_IRPT" );  ADD_BMASK(DBSR_TIE,   "DBSR_TRAP" );  ADD_BMASK(DBSR_IAC1,  "DBSR_IAC1" );   \
      ADD_BMASK(DBSR_IAC2,  "DBSR_IAC2" );  ADD_BMASK(DBSR_DAC1R, "DBSR_DAC1R");  ADD_BMASK(DBSR_DAC1W, "DBSR_DAC1W");  ADD_BMASK(DBSR_DAC2R, "DBSR_DAC2R");   \
      ADD_BMASK(DBSR_DAC2W, "DBSR_DAC2W");  ADD_BMASK(DBSR_RET,   "DBSR_RET"  );                                                                               \
      /* ESR */                                                                                                                                                \
      ADD_BMASK(ESR_PIL,    "ESR_PIL"   );  ADD_BMASK(ESR_PPR,    "ESR_PPR"   );  ADD_BMASK(ESR_PTR,    "ESR_PTR"   );  ADD_BMASK(ESR_ST,     "EST_ST"    );   \
      ADD_BMASK(ESR_DLK,    "ESR_DLK"   );  ADD_BMASK(ESR_ILK,    "ESR_ILK"   );  ADD_BMASK(ESR_BO,     "ESR_BO"    );  ADD_BMASK(ESR_SPV,    "ESR_SPE"   );   \
      /* PVR */                                                                                                                                                \
      ADD_BMASK(0xffff0000, "PVR_VER"   );  ADD_BMASK(0xffff,     "PVR_REV"   );                                                                               \
      /* HID0 .  original bitmasks in linux's header files are all wrong, so specified here manually */                                                        \
      ADD_BMASK(0x80000000, "HID0_EMCP" );  ADD_BMASK(0x800000,   "HID0_DOZE" );  ADD_BMASK(0x400000,   "HID0_NAP"  );  ADD_BMASK(0x200000,   "HID0_SLEEP");   \
      ADD_BMASK(0x4000,     "HID0_TBEN" );  ADD_BMASK(0x2000, "HID0_SEL_TBCLK");  ADD_BMASK(0x80, "HID0_EN_MAS7_UPDATE");                                      \
      ADD_BMASK(0x40,       "HID0_DCFA" );  ADD_BMASK(0x1,        "HID0_NOPTI");                                                                               \
      /* MAS0 */                                                                                                                                               \
      ADD_BMASK(0x10000000, "MAS0_TLBSEL");                                       ADD_BMASK(0xf0000,    "MAS0_ESEL" );  ADD_BMASK(0x1,        "MAS0_NV"   );   \
      /* MAS1 */                                                                                                                                               \
      ADD_BMASK(0x80000000, "MAS1_V"     ); ADD_BMASK(0x40000000, "MAS1_IPROT");  ADD_BMASK(0x00ff0000, "MAS1_TID"  );  ADD_BMASK(0x1000,     "MAS1_TS"   );   \
      ADD_BMASK(0xf00,      "MAS1_TSIZE" );                                                                                                                    \
      /* MAS2 */                                                                                                                                               \
      ADD_BMASK(0xfffff000, "MAS2_EPN"   ); ADD_BMASK(0x40,       "MAS2_X0"   );  ADD_BMASK(0x20,       "MAS2_X1"   );  ADD_BMASK(0x10,       "MAS2_W"    );   \
      ADD_BMASK(0x8,        "MAS2_I"     ); ADD_BMASK(0x4,        "MAS2_M"    );  ADD_BMASK(0x2,        "MAS2_G"    );  ADD_BMASK(0x1,        "MAS2_E"    );   \
      ADD_BMASK(0x60,       "MAS2_X01"   ); ADD_BMASK(0x1F,       "MAS2_WIMGE");                                                                               \
      /* MAS3 */                                                                                                                                               \
      ADD_BMASK(0xfffff000, "MAS3_RPN"   ); ADD_BMASK(0x3c0,      "MAS3_U03"  );  ADD_BMASK(0x3f,       "MAS3_PERMIS");                                        \
      ADD_BMASK(0x20,       "MAS3_UX"    ); ADD_BMASK(0x10,       "MAS3_SX"   );  ADD_BMASK(0x8,        "MAS3_UW"    ); ADD_BMASK(0x4,        "MAS3_SW"   );   \
      ADD_BMASK(0x2,        "MAS3_UR"    ); ADD_BMASK(0x1,        "MAS3_SR"   );                                                                               \
      /* MAS4 */                                                                                                                                               \
      ADD_BMASK(0x30000000, "MAS4_TLBSELD");                                      ADD_BMASK(0x30000,    "MAS4_TIDSELD");                                       \
      ADD_BMASK(0xf00,      "MAS4_TSIZED"); ADD_BMASK(0x40,       "MAS4_X0D"  );  ADD_BMASK(0x20,       "MAS4_X1D"  );  ADD_BMASK(0x10,       "MAS4_WD"   );   \
      ADD_BMASK(0x8,        "MAS4_ID"    ); ADD_BMASK(0x4,        "MAS4_MD"   );  ADD_BMASK(0x2,        "MAS4_GD"   );  ADD_BMASK(0x1,        "MAS4_ED"   );   \
      ADD_BMASK(0x60,       "MAS4_X01D"  ); ADD_BMASK(0x1f,       "MAS4_WIMGED");                                                                              \
      /* MAS6 */                                                                                                                                               \
      ADD_BMASK(0xff0000,   "MAS6_SPID0" ); ADD_BMASK(0x1,        "MAS6_SAS"  );                                                                               \
      /* MAS7 */                                                                                                                                               \
      ADD_BMASK(0xf,        "MAS7_RPN"   )


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
        types_py.def_readonly("EMUL_LITTLE_ENDIAN",    &EMUL_LITTLE_ENDIAN)
                .def_readonly("EMUL_BIG_ENDIAN",       &EMUL_BIG_ENDIAN)
                ;

        // Disassembler class type
        class_<ppc_dis_booke>("ppc_dis")
            .def("disasm", disasm_ptr, disasm_overloads())
            .def("disasm", disasm_ptr2) 
            ;

        // instr_call type ( We will probably never use this directly )
        {
            class_<instr_call> instr_call_py("instr_call");
            scope instr_call_scope = instr_call_py;
            instr_call_py.def_readwrite("opcode", &instr_call::opcode)
                .def_readwrite("nargs",  &instr_call::nargs)
                .def("dump_state",       &instr_call::dump_state)
                .def("print_instr",      &instr_call::print_instr)
                ;

            // instr_arg type
            {
                class_<instr_call::instr_arg> instr_arg_py("instr_arg");
                instr_arg_py.def_readwrite("v", &instr_call::instr_arg::v)
                    .def_readwrite("p",   &instr_call::instr_arg::p)
                    .def_readwrite("t",   &instr_call::instr_arg::t)
                    ;
            }

            // Add arguments
            instr_call_py.add_property("arg0", make_function(&instr_call::getarg<0>, return_value_policy<reference_existing_object>()))
                .add_property("arg1",          make_function(&instr_call::getarg<1>, return_value_policy<reference_existing_object>()))
                .add_property("arg2",          make_function(&instr_call::getarg<2>, return_value_policy<reference_existing_object>()))
                .add_property("arg3",          make_function(&instr_call::getarg<3>, return_value_policy<reference_existing_object>()))
                .add_property("arg4",          make_function(&instr_call::getarg<4>, return_value_policy<reference_existing_object>()))
                .add_property("arg5",          make_function(&instr_call::getarg<5>, return_value_policy<reference_existing_object>()))
                ;
        }

        // ppc register type ( 64 bit )
        class_<ppc_reg64>("ppc_reg64")
            .def_readwrite("value", &ppc_reg64::value)
            .def_readonly("attr",   &ppc_reg64::attr)
            .def("set_bf",          &ppc_reg64::set_bf)
            .def("get_bf",          &ppc_reg64::get_bf)
            ;

        // PPC register file type ( contains all registers, GPRs, FPRs, SPRs etc. )
        {
            class_<ppc_regs> ppc_regs_py("ppc_regs");
            scope ppc_regs_scope = ppc_regs_py;
            ppc_regs_py.add_property("MSR", &ppc_regs::msr)
                .add_property("CR",  &ppc_regs::cr)
                ;
            ADD_ALL_REGS();

            // Bit mask constants defined
            {
                class_<dummy<1> > bitmasks_py("bitmasks", no_init);
                scope bitmasks_scope = bitmasks_py;
                ADD_BITMASKS();
            }
        }

        // Memory namespace
        {
            class_<memory> memory_py("memory", init<uint64_t>());
            scope memory_scope = memory_py;
            memory_py.def("reg_tgt",       &memory::register_memory_target, register_memory_target_overloads())
                .def("dump_tgts",          &memory::dump_all_memory_targets)
                .def("dump_page_maps",     &memory::dump_all_page_maps)
                .def("dump_pages",         &memory::dump_all_pages, dump_all_pages_overloads())
                //.def("write_buf",         &memory::write_from_buffer)
                //.def("read_buf",          &memory::read_to_buffer, return_value_policy<manage_new_object>())
                .def("read8",              &memory::read8)
                .def("write8",             &memory::write8)
                .def("read16",             &memory::read16, read16_overloads())
                .def("write16",            &memory::write16, write16_overloads())
                .def("read32",             &memory::read32, read32_overloads())
                .def("write32",            &memory::write32, write32_overloads())
                .def("read64",             &memory::read64, read64_overloads())
                .def("write64",            &memory::write64, write64_overloads())
                .def("write_from_file",    &memory::write_from_file)
                .def("read_to_file",       &memory::read_to_file)
                ;

            // Memory targets namespace ( defines constants for memory targets )
            {
                class_<dummy<2> > mem_tgts_py("tgts", no_init);
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
            ;

        // The derived cpu_ppc_book class ( Our main cpu class )
        class_<cpu_ppc_booke> cpu_ppc_py("cpu_ppc", init<uint64_t, std::string>());
        cpu_ppc_py.def("run_instr",   run_instr_ptr_d0)
            .def("run_instr",         run_instr_ptr2_d0)
            .def("get_reg",           &cpu_ppc_booke::get_reg)
            .def("dump_state",        &cpu_ppc_booke::dump_state, dump_state_overloads())
            .def("print_L2tlbs",      &cpu_ppc_booke::print_L2tlbs)
            .def("init_reg_attrs",    &cpu_ppc_booke::init_reg_attrs)
            .add_property("regs",     make_function(&cpu_ppc_booke::___get_regs, return_value_policy<reference_existing_object>()))
            .add_property("PC",       &cpu_ppc_booke::get_pc)
            ;


    }

    // Class machine ( Our top level machine class. This is the class we are going to use directly. )
    typedef machine<2,36>  machine_2cpus_36pl;
    class_<machine_2cpus_36pl> machine_py("machine");
    machine_py.def_readonly("ncpus", &machine_2cpus_36pl::m_ncpus)
        .def_readwrite("memory",     &machine_2cpus_36pl::m_memory)
        .add_property("cpu0", make_function(&machine_2cpus_36pl::get_cpu<0>, return_value_policy<reference_existing_object>()))
        .add_property("cpu1", make_function(&machine_2cpus_36pl::get_cpu<1>, return_value_policy<reference_existing_object>()))
        ;
  
    // Custom message after loading this module 
    std::cout << "PPCSIMBOOKE"                                                                                                     << std::endl;
    std::cout << "=============================================================================================================="  << std::endl;
    std::cout << "This a simulator for booke based powerPC cores and is intended to simulate core, and the memory subsystem."      << std::endl; 
    std::cout << "Copyright Vikas Chouhan ( presentisgood@gmail.com ) 2012"                                                        << std::endl;

    // Redirect all logs to this file.
    LOG_TO_FILE("ppcsim.log");

}
