/*
 * Test to check if tlb_booke's interface is working fine.
 */
#include <iostream>
#include <string>
#include "third_party/lru/lru.hpp"

int main(){
    typedef lru_cache<int, std::string> lru_cache_int_str;
    lru_cache_int_str l0 = lru_cache_int_str(2);

    std::cout << "inserting (3, \"Vikas\") in l0" << std::endl;
    l0.insert(3, "Vikas");
    std::cout << "l0[3] returned " << l0[3] << std::endl;
    std::cout << "inserting (3, \"Ashish\") in l0 ( This shouldn't work )" << std::endl;
    l0.insert(3, "Ashish");
    std::cout << "l0[3] returned " << l0[3] << std::endl;
    std::cout << "This should fail." << std::endl;
    std::cout << "l0[2] returned " << l0[2] << std::endl;
    return 0;
}
