#include <iostream>
#include "globals.h"

// NOTE : Only purpose of this test, is to capture syntax errors in misc.hpp 
int main(){
    std::cout << "Compiled correctly." << std::endl;
    ppcsimbooke::instr_call ic[5];
    ic[0].dump_state();
    return 0;
}
