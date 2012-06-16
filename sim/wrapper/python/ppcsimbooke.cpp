#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "cpu_ppc.hpp"

// Wrapping some cpu functions

int (cpu::*run_instr_ptr)(instr_call*) = &cpu::run_instr;
int (cpu::*run_instr_ptr2)(std::string, std::string, std::string, std::string, std::string,
        std::string, std::string) = &cpu::run_instr;

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
