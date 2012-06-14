#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "cpu.hpp"

//class cpu_ppc_wrap : cpu_ppc, wrapper<cpu_ppc>
//{
//    public:
//    int run_instr(instr_call *ic){
//        return this->get_override("run_instr")(ic);
//    }
//    int run_instr(std::string opcode, std::string arg0, std::string arg1, std::string arg2, std::string arg3,
//            std::string arg4, std::string arg5){
//        return this->get_override(opcode, arg0, arg1, arg2, arg3, arg4, arg5);
//    }
//}

BOOST_PYTHON_MODULE(ppcsim)
{
    using namespace boost::python;
    class_<instr_call>("instr_call")
        .def_readwrite("opcode", &instr_call::opcode)
        .add_property("arg0", &instr_call::getarg0, &instr_call::setarg0)
        .add_property("arg1", &instr_call::getarg1, &instr_call::setarg1)
        .add_property("arg2", &instr_call::getarg2, &instr_call::setarg2)
        .add_property("arg3", &instr_call::getarg3, &instr_call::setarg3)
        .add_property("arg4", &instr_call::getarg4, &instr_call::setarg4)
        .add_property("arg5", &instr_call::getarg5, &instr_call::setarg5)
        .def("dump_state", &instr_call::dump_state)
        ;
}
