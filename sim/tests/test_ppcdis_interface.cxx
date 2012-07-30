#include "ppc_dis.hpp"

int main(int argc, char* argv[])
{
    LOG_TO_FILE("test_ppcdis_interface.log");
    DIS_PPC dis0 = DIS_PPC();
    instr_call c0 = dis0.disasm(0x7c4033cc, EMUL_BIG_ENDIAN);
    c0.dump_state();
    c0.print_instr();
    return 0;
}
