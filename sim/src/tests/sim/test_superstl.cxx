#include <iostream>
#include "superstl.h"

int main(){
    std::cout << "SUPERSTL test." << std::endl;

    typedef superstl::ChunkList<int, 200>  cl;
    cl v0;

    typedef superstl::SelfHashtable<int, int> sht;
    sht sht_v0;

    return 0;
}
