/*
 * Test to check if system's interface is working fine.
 */
#include "machine.hpp"
#include <string>

using std::string;

int main(){
    LOG_TO_FILE("test_system_interface.log");
    machine m0 = machine();
    return 0;
}
