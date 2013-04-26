#include "basic_block.h"

namespace ppcsimbooke_basic_block {

    void basic_block_decoder::reset(){
        transbufcount = 0;
        insnbytes_buffsize = 0;
        valid_byte_count = 0;
        ipstart = ip = bip.ip;
        byteoffset = 0;
        op = 0;
        insn_count = 0;
        invalid = false;
        outcome = 0;
        fault_addr = 0;
        fault_cause = 0;
    }

    basic_block_decoder::basic_block_decoder(const basic_block_ip& bip){
        bb.bip = bip;
        reset(); 
    }

    basic_block_decoder::(CPU_PPC& ctx, uint64_t ip){
    }

    int basic_block_decode::fillbuff(CPU_PPC *ctx, uint8_t *buff, int buffsize){
        insnbytes = buff;
        insnbytes_buffsize = buffsize;
        byteoffset = 0;
        invalid = 0;
        valid_byte_count = buffsize;
        try {
            // We may encounter tlb misses when copying from memory.
            // We don't catch any other error, beacause anything else is a potential simulator error
            // & hence we need to die in those cases.
            ctx.read_buff(bb.bip.ip, buff, buffsize);
        }catch(sim_except_ppc &e){
            switch(e.err_code<0>()){
                case PPC_EXCEPTION_ISI:
                case PPC_EXCEPTION_DSI:
                    fault_addr = e.addr();
                    fault_cause = e.err_code<0>();
                    valid_byte_count = fault_addr - vv.bip.ip;
                default:
            }
        }
        return valid_byte_count;
    }

    bool basic_block_decode::decode(){

    }
}
