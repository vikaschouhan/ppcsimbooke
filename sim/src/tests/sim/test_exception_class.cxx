/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include "exception.h"
#include <iostream>

int main(){
    std::cout << "Testing sim_except with error code only" << std::endl;
    try{
        throw sim_except(SIM_EXCEPT_EINVAL);
    }catch (sim_except &id){
        std::cout << id.what() << std::endl;
    }
    std::cout << "Testing sim_except with error code as well as with custom message" << std::endl;
    try{
        throw sim_except(SIM_EXCEPT_EINVAL, "I fucked up !!");
    }catch (sim_except &id){
        std::cout << id.what() << std::endl;
    }
    std::cout << "Testing sim_except with unknown error code " << std::endl;
    try{
        throw sim_except(-1, "I fucked up !!");
    }catch (sim_except &id){
        std::cout << id.what() << std::endl;
    }
    return 0;
}
