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
    CPU_PPC cpu0(cpuid, cpuname);
    cpu0.dump_state(4);
    std::cout << "Adding some instrs" << std::endl;
    cpu0.run_instr("addi r0, r0, 0x345");
    cpu0.run_instr("add r0, r0, r0");
    //ptr->run_instr("add", "r0", "r0", "r0");
    cpu0.run_instr("mtmsr r0, 0x0");
    cpu0.run_instr("cmpi 0x0, 0x0, r0, 0x68a");
    cpu0.dump_state(4);
    cpu0.get_reg("r0");
    return 0;
}
