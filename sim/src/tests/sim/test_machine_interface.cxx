/*
 * Test to check if system's interface is working fine.
 */
#include "machine.h"
#include <string>

using std::string;

int main(){
    LOG_TO_FILE("test_system_interface.log");
    ppcsimbooke::machine mc;
    return 0;
}
