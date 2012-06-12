/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include "tlb_booke.hpp"
#include <string>

using std::string;

int main(){
    ppc_tlb_booke tlbl2;
    tlbl2.add_tlb(256, 4, 0, 0, "tlb0");
    tlbl2.add_tlb(16, 0, 0, 1, "tlb1");
    //ppc_tlb tlbl2(2, 256, 2, 0, 0, "tlb0", 16, 0, 0, 1, "tlb1" );
    tlbl2.init_defaults();
    // add new tlb entry
    tlbl2.tlbwe(0x10020000, 0x80000000, 0, 0, 0, 0);
    // print alll tlb entries
    tlbl2.print_tlbs2();
    return 0;
}
