#include "ppc_dis.h"

int main(int argc, char* argv[])
{
    LOG_TO_FILE("test_ppcdis_interface.log");
    ppcsimbooke::ppcsimbooke_dis::ppcdis dis0 = ppcsimbooke::ppcsimbooke_dis::ppcdis();
    ppcsimbooke::instr_call c0 = dis0.disasm(0x7c4033cc, 0xffffffc, EMUL_BIG_ENDIAN);
    c0.dump_state();
    c0.print_instr();
    return 0;
}
