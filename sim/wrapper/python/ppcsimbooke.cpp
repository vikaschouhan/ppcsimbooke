#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/shared_ptr.hpp>

#include "cpu_ppc.hpp"
#include "memory.hpp"

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

    class_<cpu_ppc_booke>("cpu_ppc", no_init)
        .def("create",      &cpu_ppc_booke::create, return_value_policy<manage_new_object>()).staticmethod("create")
        .def("destroy",     &cpu_ppc_booke::destroy).staticmethod("destroy")
        .def("run_instr",   run_instr_ptr_d0)
        .def("run_instr",   run_instr_ptr2_d0, run_instr_overloads())
        .def("msr",     &cpu_ppc_booke::get_msr)
        .def("gpr",     &cpu_ppc_booke::get_gpr)
        .def("spr",     &cpu_ppc_booke::get_spr)
        .def("pmr",     &cpu_ppc_booke::get_pmr)
        .def("fpr",     &cpu_ppc_booke::get_fpr)
        .def("cr",      &cpu_ppc_booke::get_cr)
        .def("fpscr",   &cpu_ppc_booke::get_fpscr)
        .def("reg",     &cpu_ppc_booke::get_reg)
        .def("dump_state",  &cpu_ppc_booke::dump_state, dump_state_overloads())
        .def("msr",     &cpu_ppc_booke::set_msr)
        .def("gpr",     &cpu_ppc_booke::set_gpr)
        .def("spr",     &cpu_ppc_booke::set_spr)
        .def("pmr",     &cpu_ppc_booke::set_pmr)
        .def("fpr",     &cpu_ppc_booke::set_fpr)
        .def("cr",      &cpu_ppc_booke::set_spr)
        .def("fpscr",   &cpu_ppc_booke::set_fpscr)
        ;
}
