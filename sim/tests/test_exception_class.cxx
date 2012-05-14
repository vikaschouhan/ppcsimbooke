/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include "exception.hpp"
#include <iostream>

int main(){
    std::cout << "Testing sim_exception with error code only" << std::endl;
    try{
        throw sim_exception(SIM_EXCEPT_ILLEGAL_OP);
    }catch (sim_exception &id){
        std::cout << id.what() << std::endl;
    }
    std::cout << "Testing sim_exception with error code as well as with custom message" << std::endl;
    try{
        throw sim_exception(SIM_EXCEPT_ILLEGAL_OP, "I fucked up !!");
    }catch (sim_exception &id){
        std::cout << id.what() << std::endl;
    }
    std::cout << "Testing sim_exception with unknown error code " << std::endl;
    try{
        throw sim_exception(-2, "I fucked up !!");
    }catch (sim_exception &id){
        std::cout << id.what() << std::endl;
    }
    return 0;
}
