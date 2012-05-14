/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include "memory.hpp"
#include <string>

using std::string;

int main(){
    LOG_TO_FILE("test_memory_interface.log");
    char arr[80];
    uint8_t data[80];
    for(int i=0; i<80; i++)
        arr[i] = static_cast<char>(i);

    std::shared_ptr<memory> mem_ptr = memory::initialize_memory(36);
    mem_ptr->register_memory_target(0xc1000000, 0x10000, "CCSR", 0, TGT_CCSR, 50); // priority of CCSR is greatest
    mem_ptr->register_memory_target(0x0, 0x100000000, "ddr0", 0, TGT_DDR);
    mem_ptr->write_from_buffer(0x10, (uint8_t *)arr, 80);
    mem_ptr->read_to_buffer(0x020, data, 80);
    mem_ptr->dump_all_memory_targets();
    std::cout << std::endl;
    mem_ptr->dump_all_page_maps();
    memory::destroy_memory();

    for(int i=0; i<80; i++)
        std::cout << (int)data[i] << " ";
    std::cout << std::endl;
    return 0;
}
