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
    transbuffcount = 0;
    insnbytes_buffsize = 0;
    valid_byte_count = 0;
    ip = bb.bip;
    byteoffset = 0;
    invalid = false;
    outcome = 0;
    fault_addr = 0;
    fault_cause = 0;
    branch_cond = false;
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::basic_block_decoder(const basic_block_ip& bip){
    bb.bip = bip;
    reset(); 
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::basic_block_decoder(ppcsimbooke::ppcsimbooke_cpu::cpu& ctx){
    ppcsimbooke::ppcsimbooke_cpu::cpu::xlated_tlb_res res = ctx.xlate(ctx.get_pc(), 0, 1);                 // wr=0, ex=1
    bb.bip = basic_block_ip(ctx.get_pc(), std::get<0>(res) & MIN_PGSZ_MASK, ctx.get_reg(REG_MSR),          // ea, mfn, msr
                            ctx.get_reg(REG_PID0), ctx.get_reg(REG_PID1), ctx.get_reg(REG_PID2),           // pid0, pid1, pid2
                            std::get<1>(res) & 0x1);                                                       // be
    reset();
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
                fault_addr = e.addr();
                fault_cause = e.err_code<0>();
                valid_byte_count = fault_addr - bb.bip;
            default:
                throw(sim_except_fatal("Unknown exception in basic_block_decoder::fillbuff."));
        }
    }
    // instruction buffer should be multiple of 4 bytes
    LASSERT_THROW_UNLIKELY(!(valid_byte_count % OPCODE_SIZE),
            sim_except(SIM_EXCEPT_EINVAL, "valid_byte_count should be multiple of " + OPCODE_SIZE), DEBUG4);
    return valid_byte_count;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::invalidate(){
    return false;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::decode(){
    int endianness = bb.bip.get_endianness();
    instr_call ic;

    // keep on decoding until either all instructions in instruction buffer are decoded, decoder
    // buffer is full or a control transfer instruction is encountered.
    while(byteoffset < valid_byte_count){
        if unlikely(transbuffcount == MAX_TRANS_INS){
            flush();         // flush instructions in temporary buffer when it's full
        }
        ic = decoder.disasm(insnbytes[byteoffset], ip, endianness);
        transbuff[transbuffcount++] = ic;
        // if current instruction is a control transfer instruction, end basic_block &
        // return immediately
        if(decoder.is_control_xfer(ic)){
            branch_cond = true;
            flush();
            return true;
        }
        byteoffset += OPCODE_SIZE;
        ip += OPCODE_SIZE;
    }
    return true;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::flush(){
    if unlikely(!transbuffcount){ return false; }

    bool overflow = (transbuffcount - bb.transopscount) >= MAX_BB_INS;
    instr_call ic;

    if unlikely(overflow){
        assert(!first_insn_in_bb());
        split();
    }

    for(size_t i=0; i<transbuffcount; i++){
        if unlikely(bb.transopscount >= MAX_BB_INS){
            std::cerr << "Instruction limit for basic block exceeded !!" << std::endl;
        }
        ic = transbuff[i];
        bb.transops[bb.transopscount++] = ic;
    }

    transbuffcount = 0;

    return true;
}

void ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::split(){
    bb.ip_taken = ip;
    bb.ip_not_taken = ip;
    bb.brtype = BB_BRTYPE_SPLIT;
    //end_of_block = 1;
}
