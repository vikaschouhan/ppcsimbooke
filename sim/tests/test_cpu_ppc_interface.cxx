/*
 * Test to check if cpu_ppc's interface is working fine.
 */
#include "cpu_ppc.hpp"
#include <string>

using std::string;

int main(){
    LOG_TO_FILE("test_cpu_ppc_interface.log");
    uint32_t cpuid = 0x80101234; // A unique cpu id.
    string cpuname = "e500v2";
    ppc_cpu_booke *ptr=ppc_cpu_booke::create(cpuid, cpuname);
    ptr->dump_state(4);
    std::cout << "Adding some instrs" << std::endl;
    ptr->run_instr("addi", "r0", "r0", "0x345");
    ptr->run_instr("add", "r0", "r0", "r0");
    //ptr->run_instr("add", "r0", "r0", "r0");
    ptr->run_instr("cmpi", "0", "0", "r0", "0x68a"); 
    ptr->dump_state(4);
    ppc_cpu_booke::destroy(ptr);
    return 0;
}
