#include "ppc_dis.hpp"

int main(int argc, char* argv[])
{
    LOG_TO_FILE("test_ppcdis_interface.log");
    ppc_dis_booke dis0 = ppc_dis_booke();
    return 0;
}
