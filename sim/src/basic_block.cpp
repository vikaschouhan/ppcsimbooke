#include "basic_block.h"
#include "cpu_ppc.h"

/////////////////////////////////////////////////////////////////////////////////
// basic block
/////////////////////////////////////////////////////////////////////////////////

void ppcsimbooke::ppcsimbooke_basic_block::basic_block::reset(){
}

void ppcsimbooke::ppcsimbooke_basic_block::basic_block::reset(const ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip &bip){
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block* ppcsimbooke::ppcsimbooke_basic_block::basic_block::clone(){
    return NULL;
}

void ppcsimbooke::ppcsimbooke_basic_block::basic_block::free(){
}


/////////////////////////////////////////////////////////////////////////////////
// basic block decoder
/////////////////////////////////////////////////////////////////////////////////

void ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::reset(){
    transbufcount = 0;
    insnbytes_buffsize = 0;
    valid_byte_count = 0;
    ipstart = ip = bb.bip;
    byteoffset = 0;
    op = 0;
    insn_count = 0;
    invalid = false;
    outcome = 0;
    fault_addr = 0;
    fault_cause = 0;
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::basic_block_decoder(const basic_block_ip& bip){
    bb.bip = bip;
    reset(); 
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::basic_block_decoder(ppcsimbooke::ppcsimbooke_cpu::cpu& ctx){
}

int ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::fillbuff(ppcsimbooke::ppcsimbooke_cpu::cpu &ctx, uint8_t *buff, int buffsize){
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
                valid_byte_count = fault_addr - bb.bip;
            default:
                throw(sim_except_fatal("Unknown exception in basic_block_decoder::fillbuff."));
        }
    }
    return valid_byte_count;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::invalidate(){
    return false;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::decode(){
    return false;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::flush(){
    return false;
}
