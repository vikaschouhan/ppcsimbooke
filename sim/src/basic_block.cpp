#include "basic_block.h"

namespace ppcsimbooke_basic_block {

    int basic_block_decode::fillbuff(CPU_PPC *ctx, uint8_t *buff, int buffsize){
        insnbytes = buff;
        insnbytes_buffsize = buffsize;
        byteoffset = 0;
        invalid = 0;
        
    }

    bool basic_block_decode::decode(){

    }
}
