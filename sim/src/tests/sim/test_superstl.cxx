#include <iostream>
#include "superstl.h"

int main(){
    std::cout << "SUPERSTL test." << std::endl;
    typedef superstl::ChunkList<int, 200>  cl;
    cl v0;
    return 0;
}
