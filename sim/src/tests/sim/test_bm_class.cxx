#include "bm.hpp"

int main(int argc, char* argv[])
{
    LOG_TO_FILE("test_bm_class.log");
    BM  bm0;
    bm0.add_breakpoint(0x3456);
    bm0.add_breakpoint(0xfffffffc);    
    bm0.list_breakpoints();
    bm0.delete_breakpoint(0x3456);
    bm0.list_breakpoints();
    return 0;
}
