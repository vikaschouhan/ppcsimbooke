/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include "tlb_booke.hpp"
#include <string>

int main(){
    LOG_TO_FILE("test_tlb_booke_interface.log");

    typedef ppc_tlb_booke<128, 4, 16>  booke_tlb;
    booke_tlb tlbl2;

    // print alll tlb entries
    tlbl2.print_tlbs2();
    return 0;
}
