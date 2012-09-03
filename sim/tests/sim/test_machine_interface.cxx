/*
 * Test to check if system's interface is working fine.
 */
#include "machine.hpp"
#include <string>

using std::string;

int main(){
    LOG_TO_FILE("test_system_interface.log");
    typedef machine<2, 36, 32, 128, 4, 64>  mc_2cpus;
    mc_2cpus m0 = mc_2cpus();
    return 0;
}
