/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include "tlb_booke.h"
#include <string>

int main(){
    LOG_TO_FILE("test_tlb_booke_interface.log");

    TLB_PPC tlbl2;

    // print alll tlb entries
    tlbl2.print_tlbs2();
    return 0;
}
