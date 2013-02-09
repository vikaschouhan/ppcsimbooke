
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
#include <boost/python/exception_translator.hpp>

#include "cpu_ppc.hpp"
#include "memory.hpp"
#include "machine.hpp"
#include "ppc_dis.hpp"
 
#define  SIM_REL                     "current"
// CPU parameters
#define  N_CPUS                      2     // Number of cpus
#define  CPU_CACHE_LINE_SIZE         32    // cache line size
#define  CPU_PHY_ADDR_SIZE           36    // physical address line size
#define  CPU_TLB4K_N_SETS            128   // number of sets in tlb4K
#define  CPU_TLB4K_N_WAYS            4     // number of ways/set in tlb4K
#define  CPU_TLBCAM_N_ENTRIES        16    // Number of entries in tlbCam

typedef CPU_PPC<CPU_CACHE_LINE_SIZE, CPU_PHY_ADDR_SIZE, CPU_TLB4K_N_SETS, CPU_TLB4K_N_WAYS, CPU_TLBCAM_N_ENTRIES>   cpu_e500v2_t;
typedef MEM_PPC<CPU_PHY_ADDR_SIZE>                                                                                  memory_e500v2_t;
typedef machine<N_CPUS, CPU_PHY_ADDR_SIZE, CPU_CACHE_LINE_SIZE,
        CPU_TLB4K_N_SETS, CPU_TLB4K_N_WAYS, CPU_TLBCAM_N_ENTRIES>                                                   machine_e500v2_t;

// Wrapping some cpu functions

// func ptrs for overloaded CPU_PPC::run_instrs()
void (cpu_e500v2_t::*run_instr_ptr_d0)(uint32_t)     = &cpu_e500v2_t::run_instr;
void (cpu_e500v2_t::*run_instr_ptr2_d0)(std::string) = &cpu_e500v2_t::run_instr;

// Wrapping some ppc_dis functions
instr_call (DIS_PPC::*disasm_ptr)(uint32_t, uint64_t, int)  = &DIS_PPC::disasm;
instr_call (DIS_PPC::*disasm_ptr2)(std::string, uint64_t)   = &DIS_PPC::disasm;

// Overloads for CPU_PPC::step()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(step_overloads, step, 0, 1);

// Overloads for CPU_PPC::dump_state()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dump_state_overloads, dump_state, 0, 3);

// Overloads for memory::register_memory_target()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(register_memory_target_overloads, register_memory_target, 5, 6);
// Overloads for memory::dump_all_pages()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dump_all_pages_overloads, dump_all_pages, 0, 1);
// Overloads for memory::dump_page()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dump_page_overloads, dump_page, 1, 2);
// Overloads for memory::read16()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(read8_overloads, read8, 1, 2);
// Overloads for memory::write16()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(write8_overloads, write8, 2, 3);
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
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(disasm_overloads, disasm, 1, 3);
// Overloads for ppc_dis::disasm() ( string version )
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(disasm2_overloads, disasm, 1, 2);

// Overloads for machine::run()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(mc_run_overloads, run, 0, 1);
// Overloads for machine::stop()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(mc_stop_overloads, stop, 0, 1);
// Overloads for machine::run_mode()
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(mc_run_mode_overloads, run_mode, 0, 1);

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

// Exception translation mechanism
void translate_sim_ex(const sim_except& e){
    PyErr_SetString(PyExc_RuntimeError, e.desc());
}
void translate_sim_ex_fatal(const sim_except_fatal& e){
    PyErr_SetString(PyExc_RuntimeError, e.desc());
}
void translate_sim_ex_ppc(const sim_except_ppc& e){
    PyErr_SetString(PyExc_RuntimeError, e.desc());
}
void translate_sim_ex_ppc_halt(const sim_except_ppc_halt& e){
   PyErr_SetString(PyExc_RuntimeError, e.desc());
}

// python signal checker
int py_sig_callback(){
    PyErr_CheckSignals();
    if(PyErr_Occurred()){
        std::cerr << "Signal recieved." << std::endl;
        return 1;
    }
    return 0;
}

// Add attributes for GPRs
#define ADD_REG(reg_v, reg_alias) \
    ppc_regs_py.def_readwrite(reg_alias, &ppc_regs::reg_v)

// Add all registers
#define ADD_ALL_REGS() \
       ADD_REG(gpr0,  "R0"  ); ADD_REG(gpr1,  "R1"  ); ADD_REG(gpr2,  "R2"  ); ADD_REG(gpr3,  "R3"  );  \
       ADD_REG(gpr4,  "R4"  ); ADD_REG(gpr5,  "R5"  ); ADD_REG(gpr6,  "R6"  ); ADD_REG(gpr7,  "R7"  );  \
       ADD_REG(gpr8,  "R8"  ); ADD_REG(gpr9,  "R9"  ); ADD_REG(gpr10, "R10" ); ADD_REG(gpr11, "R11" );  \
       ADD_REG(gpr12, "R12" ); ADD_REG(gpr13, "R13" ); ADD_REG(gpr14, "R14" ); ADD_REG(gpr15, "R15" );  \
       ADD_REG(gpr16, "R16" ); ADD_REG(gpr17, "R17" ); ADD_REG(gpr18, "R18" ); ADD_REG(gpr19, "R19" );  \
       ADD_REG(gpr20, "R20" ); ADD_REG(gpr21, "R21" ); ADD_REG(gpr22, "R22" ); ADD_REG(gpr23, "R23" );  \
       ADD_REG(gpr24, "R24" ); ADD_REG(gpr25, "R25" ); ADD_REG(gpr26, "R26" ); ADD_REG(gpr27, "R27" );  \
       ADD_REG(gpr28, "R28" ); ADD_REG(gpr29, "R29" ); ADD_REG(gpr30, "R30" ); ADD_REG(gpr31, "R31" );  \
                                                                                            \
      /* Add attributes for other registers */                                              \
      ADD_REG(atbl,    "ATBL"   ); ADD_REG(atbu,    "ATBU"    ); ADD_REG(csrr0,   "CSRR0"  ); ADD_REG(csrr1,    "CSRR1"  );    \
      ADD_REG(ctr,     "CTR"    ); ADD_REG(dac1,    "DAC1"    ); ADD_REG(dac2,    "DAC2"   ); ADD_REG(dbcr0,    "DBCR0"  );    \
      ADD_REG(dbcr1,   "DBCR1"  ); ADD_REG(dbcr2,   "DBCR2"   ); ADD_REG(dbsr,    "DBSR"   ); ADD_REG(dear,     "DEAR"   );    \
      ADD_REG(dec,     "DEC"    ); ADD_REG(decar,   "DECAR"   ); ADD_REG(esr,     "ESR"    ); ADD_REG(iac1,     "IAC1"   );    \
      ADD_REG(iac2,    "IAC2"   ); ADD_REG(ivor0,   "IVOR0"   ); ADD_REG(ivor1,   "IVOR1"  ); ADD_REG(ivor2,    "IVOR2"  );    \
      ADD_REG(ivor3,   "IVOR3"  ); ADD_REG(ivor4,   "IVOR4"   ); ADD_REG(ivor5,   "IVOR5"  ); ADD_REG(ivor6,    "IVOR6"  );    \
      ADD_REG(ivor8,   "IVOR8"  ); ADD_REG(ivor10,  "IVOR10"  ); ADD_REG(ivor11,  "IVOR11" ); ADD_REG(ivor12,   "IVOR12" );    \
      ADD_REG(ivor13,  "IVOR13" ); ADD_REG(ivor14,  "IVOR14"  ); ADD_REG(ivor15,  "IVOR15" ); ADD_REG(ivpr,     "IVPR"   );    \
      ADD_REG(lr,      "LR"     ); ADD_REG(pid0,    "PID"     ); ADD_REG(pir,     "PIR"    ); ADD_REG(pvr,      "PVR"    );    \
      ADD_REG(sprg0,   "SPRG0"  ); ADD_REG(sprg1,   "SPRG1"   ); ADD_REG(sprg2,   "SPRG2"  ); ADD_REG(sprg3,    "SPRG3R" );    \
      ADD_REG(sprg3,   "SPRG3"  ); ADD_REG(sprg4,   "SPRG4R"  ); ADD_REG(sprg4,   "SPRG4"  ); ADD_REG(sprg5,    "SPRG5R" );    \
      ADD_REG(sprg5,   "SPRG5"  ); ADD_REG(sprg6,   "SPRG6R"  ); ADD_REG(sprg6,   "SPRG6"  ); ADD_REG(sprg7,    "SPRG7R" );    \
      ADD_REG(sprg7,   "SPRG7"  ); ADD_REG(srr0,    "SRR0"    ); ADD_REG(srr1,    "SRR1"   ); ADD_REG(tbl,      "TBRL"   );    \
      ADD_REG(tbl,     "TBWL"   ); ADD_REG(tbu,     "TBRU"    ); ADD_REG(tbu,     "TBWU"   ); ADD_REG(tcr,      "TCR"    );    \
      ADD_REG(tsr,     "TSR"    ); ADD_REG(usprg0,  "USPRG0"  ); ADD_REG(xer,     "XER"    ); ADD_REG(bbear,    "BBEAR"  );    \
      ADD_REG(bbtar,   "BBTAR"  ); ADD_REG(bucsr,   "BUCSR"   ); ADD_REG(hid0,    "HID0"   ); ADD_REG(hid1,     "HID1"   );    \
      ADD_REG(ivor32,  "IVOR32" ); ADD_REG(ivor33,  "IVOR33"  ); ADD_REG(ivor34,  "IVOR34" ); ADD_REG(ivor35,   "IVOR35" );    \
      ADD_REG(l1cfg0,  "L1CFG0" ); ADD_REG(l1cfg1,  "L1CFG1"  ); ADD_REG(l1csr0,  "L1CSR0" ); ADD_REG(l1csr1,   "L1CSR1" );    \
      ADD_REG(mas0,    "MAS0"   ); ADD_REG(mas1,    "MAS1"    ); ADD_REG(mas2,    "MAS2"   ); ADD_REG(mas3,     "MAS3"   );    \
      ADD_REG(mas4,    "MAS4"   ); ADD_REG(mas5,    "MAS5"    ); ADD_REG(mas6,    "MAS6"   ); ADD_REG(mas7,     "MAS7"   );    \
      ADD_REG(mcar,    "MCAR"   ); ADD_REG(mcsr,    "MCSR"    ); ADD_REG(mcsrr0,  "MCSRR0" ); ADD_REG(mcsrr1,   "MCSRR1" );    \
      ADD_REG(mmucfg,  "MMUCFG" ); ADD_REG(mmucsr0, "MMUCSR0" ); ADD_REG(pid0,    "PID0"   ); ADD_REG(pid1,     "PID1"   );    \
      ADD_REG(pid2,    "PID2"   ); ADD_REG(spefscr, "SPEFSCR" ); ADD_REG(svr,     "SVR"    ); ADD_REG(tlb0cfg,  "TLB0CFG");    \
      ADD_REG(tlb1cfg, "TLB1CFG")

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

    // Install the python signal checker
    py_signal_callback::callback = py_sig_callback;

    // Register exception convertors
    register_exception_translator<sim_except>(&translate_sim_ex);
    register_exception_translator<sim_except_fatal>(&translate_sim_ex_fatal);
    register_exception_translator<sim_except_ppc>(&translate_sim_ex_ppc);
    register_exception_translator<sim_except_ppc_halt>(&translate_sim_ex_ppc_halt);

    // Types namespace ( defines all class types being used in our module, one way of other )
    {
        class_<dummy<0> > types_py("types", no_init);
        scope types_scope = types_py;

        // Endianness attributes
        types_py.def_readonly("EMUL_LITTLE_ENDIAN",    &EMUL_LITTLE_ENDIAN)
                .def_readonly("EMUL_BIG_ENDIAN",       &EMUL_BIG_ENDIAN)
                ;

        // Disassembler class type
        class_<DIS_PPC>("ppc_dis")
            .def("disasm", disasm_ptr,  disasm_overloads())
            .def("disasm", disasm_ptr2, disasm2_overloads()) 
            ;

        // instr_call type ( We will probably never use this directly )
        {
            class_<instr_call> instr_call_py("instr_call");
            scope instr_call_scope = instr_call_py;
            instr_call_py.def_readwrite("opcode",   &instr_call::opc)
                .def_readwrite("opcode_name",       &instr_call::opcname)
                .def_readwrite("nargs",             &instr_call::nargs)
                .def("dump_state",                  &instr_call::dump_state)
                .def("print_instr",                 &instr_call::print_instr)
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
        class_<ppc_reg64>("ppc_reg64", init<uint64_t, uint64_t, int, int>())
            .add_property("value",  &ppc_reg64::__get_v, &ppc_reg64::__set_v)
            .def_readonly("fvalue", &ppc_reg64::fvalue)
            .def_readonly("attr",   &ppc_reg64::attr)
            .def_readonly("regno",  &ppc_reg64::indx)
            .def("set_bf",          &ppc_reg64::set_bf)
            .def("get_bf",          &ppc_reg64::get_bf)
            .def("refresh_fval",    &ppc_reg64::refresh_fval)
            ;

        // PPC register file type ( contains all registers, GPRs, SPRs etc. )
        {
            class_<ppc_regs> ppc_regs_py("ppc_regs");
            scope ppc_regs_scope = ppc_regs_py;
            ppc_regs_py.add_property("MSR", &ppc_regs::msr)
                .add_property("CR",  &ppc_regs::cr)
                .add_property("ACC", &ppc_regs::acc)
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
            class_<memory_e500v2_t> memory_py("memory");
            scope memory_scope = memory_py;
            memory_py.def("reg_tgt",       &memory_e500v2_t::register_memory_target, register_memory_target_overloads())
                .def("dump_tgts",          &memory_e500v2_t::dump_all_memory_targets)
                .def("dump_page_maps",     &memory_e500v2_t::dump_all_page_maps)
                .def("dump_pages",         &memory_e500v2_t::dump_all_pages, dump_all_pages_overloads())
                .def("dump_page",          &memory_e500v2_t::dump_page, dump_page_overloads())
                //.def("write_buf",         &memory::write_from_buffer)
                //.def("read_buf",          &memory::read_to_buffer, return_value_policy<manage_new_object>())
                .def("read8",              &memory_e500v2_t::read8,  read8_overloads())
                .def("write8",             &memory_e500v2_t::write8, write8_overloads())
                .def("read16",             &memory_e500v2_t::read16, read16_overloads())
                .def("write16",            &memory_e500v2_t::write16, write16_overloads())
                .def("read32",             &memory_e500v2_t::read32, read32_overloads())
                .def("write32",            &memory_e500v2_t::write32, write32_overloads())
                .def("read64",             &memory_e500v2_t::read64, read64_overloads())
                .def("write64",            &memory_e500v2_t::write64, write64_overloads())
                .def("write_from_file",    &memory_e500v2_t::write_from_file)
                .def("read_to_file",       &memory_e500v2_t::read_to_file)
                .def("read_to_ascii_file", &memory_e500v2_t::read_to_ascii_file)
                .def("load_elf",           &memory_e500v2_t::load_elf)
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

        // The breakpoint manager class
        class_<BM> bm_py("bm");
            bm_py.def("enable",            &BM::enable_breakpoints)
            .def("disable",                &BM::disable_breakpoints)
            .def("add",                    &BM::add_breakpoint)
            .def("delete",                 &BM::delete_breakpoint)
            .def("delete_by_num",          &BM::delete_breakpoint_num)
            .def("delete_all",             &BM::delete_all)
            .def("list_all",               &BM::list_breakpoints)
            ;

        // The derived cpu_ppc_book class ( Our main cpu class )
        class_<cpu_e500v2_t> cpu_ppc_py("cpu_ppc", init<uint64_t, std::string>());
        cpu_ppc_py.def("run_instr",   run_instr_ptr_d0)
            .def("run_instr",         run_instr_ptr2_d0)
            .def("run",               &cpu_e500v2_t::run)
            .def("step",              &cpu_e500v2_t::step, step_overloads())
            .def("halt",              &cpu_e500v2_t::halt)
            .def("stop",              &cpu_e500v2_t::stop)
            .def("run_mode",          &cpu_e500v2_t::run_mode)
            .def("get_reg",           &cpu_e500v2_t::get_reg)
            .def("dump_state",        &cpu_e500v2_t::dump_state, dump_state_overloads())
            .def("print_L2tlbs",      &cpu_e500v2_t::print_L2tlbs)
            .def("en_cov",            &cpu_e500v2_t::enable_cov_log)
            .def("dis_cov",           &cpu_e500v2_t::disable_cov_log)
            .def("is_cov_enabled",    &cpu_e500v2_t::is_cov_log_enabled)
            .def("log_cov_to_file",   &cpu_e500v2_t::log_cov_to_file)
            .def("read8",             &cpu_e500v2_t::read8)
            .def("write8",            &cpu_e500v2_t::write8)
            .def("read16",            &cpu_e500v2_t::read16)
            .def("write16",           &cpu_e500v2_t::write16)
            .def("read32",            &cpu_e500v2_t::read32)
            .def("write32",           &cpu_e500v2_t::write32)
            .def("read64",            &cpu_e500v2_t::read64)
            .def("write64",           &cpu_e500v2_t::write64)
            .add_property("regs",     make_function(&cpu_e500v2_t::___get_regs, return_value_policy<reference_existing_object>()))
            .def_readonly("PC",       &cpu_e500v2_t::get_pc)
            .def_readonly("ninstrs",  &cpu_e500v2_t::get_ninstrs)
            .def_readonly("bm",       &cpu_e500v2_t::m_bm)
            ;


    }

    // Class machine ( Our top level machine class. This is the class we are going to use directly. )
    class_<machine_e500v2_t> machine_py("machine");
    machine_py.def_readonly("ncpus", &machine_e500v2_t::m_ncpus)
        .def_readwrite("memory",     &machine_e500v2_t::m_memory)
        .add_property("cpu0",        make_function(&machine_e500v2_t::get_cpu<0>, return_value_policy<reference_existing_object>()))
        .add_property("cpu1",        make_function(&machine_e500v2_t::get_cpu<1>, return_value_policy<reference_existing_object>()))
        .def("load_elf",             &machine_e500v2_t::load_elf)
        .def("run",                  &machine_e500v2_t::run, mc_run_overloads())
        .def("stop",                 &machine_e500v2_t::stop, mc_stop_overloads())
        .def("run_mode",             &machine_e500v2_t::run_mode, mc_run_mode_overloads())
        .def("trace_to_file",        &machine_e500v2_t::trace_to_file)
        .def("en_trace",             &machine_e500v2_t::trace_enable)
        .def("dis_trace",            &machine_e500v2_t::trace_disable)
        .def("is_trace_en",          &machine_e500v2_t::is_trace_enabled)
        .def("gen_cov_logs",         &machine_e500v2_t::gen_cov_logs)
        ;
  
    // Custom message after loading this module 
    std::cout << "PPCSIMBOOKE"                                                                                                     << std::endl;
    std::cout << "=============================================================================================================="  << std::endl;
    std::cout << "This a simulator for booke based powerPC cores and is intended to simulate core, and the memory subsystem."      << std::endl; 
    std::cout << "Copyright Vikas Chouhan (presentisgood@gmail.com) 2012"                                                          << std::endl;
    std::cout << "=============================================================================================================="  << std::endl;
    std::cout << std::endl;

    // Redirect all logs to this file.
    LOG_TO_FILE("ppcsim.log");

}
