#include "basic_block.h"
#include "cpu_ppc.h"

/////////////////////////////////////////////////////////////////////////////////
// basic block instruction pointer
/////////////////////////////////////////////////////////////////////////////////

ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip::basic_block_ip(){
    reset();
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip
::basic_block_ip(uint64_t ip_, uint64_t mfn_, uint32_t msr_, uint32_t pid0_, uint32_t pid1_, uint32_t pid2_, bool be_) :
    ip(ip_), mfn(mfn_), msr(msr_), pid0(pid0_), pid1(pid1_), pid2(pid2_), be(be_)
{}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip
::basic_block_ip(ppcsimbooke::ppcsimbooke_cpu::cpu& ctx){
    // translate current program counter
    ppcsimbooke::ppcsimbooke_cpu::cpu::xlated_tlb_res res = ctx.xlate(ctx.get_pc(), 0, 1);   // wr=0, ex=1

    ip   = ctx.get_pc();
    mfn  = std::get<0>(res) & MIN_PGSZ_MASK;     // mfn
    msr  = ctx.get_reg(REG_MSR);                 // msr
    pid0 = ctx.get_reg(REG_PID0);                // pid registers
    pid1 = ctx.get_reg(REG_PID1);
    pid2 = ctx.get_reg(REG_PID2);
    be   = std::get<1>(res) & 0x1;               // be
}

void ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip::reset(){
    ip   = IP_INV;
    mfn  = MFN_INV;
    msr  = 0;
    pid0 = pid1 = pid2 = 0;
}

/////////////////////////////////////////////////////////////////////////////////
// basic block chunk list manager
/////////////////////////////////////////////////////////////////////////////////

//ppcsimbooke::ppcsimbooke_basic_block::basic_block*
//ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list::xlate(ppcsimbooke::ppcsimbooke_cpu::cpu& ctx){
//    basic_block_ip bb_ip(ctx);
//
//    basic_block* bb = get(bb_ip);
//    if likely(bb) return bb;
//
//    uint8_t bb_insnbuff[MAX_BB_INS_BYTES];
//    basic_block_decoder bb_decoder(bb_ip);
//
//    bb_decoder.fillbuff(ctx, bb_insnbuff, MAX_BB_INS_BYTES);           // Assign buffer for decoder to work on
//    bb_decoder.decode();                                               // decode
//    bb = bb_decoder.clone();                                           // get basic block
//
//    bb->acquire();       // acquire basic block
//    add(bb);             // add to cache
//
//    return bb;
//}
//
//bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list::invalidate(uint64_t pc, int reason){
//    return true;
//}
//
//bool
//ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list
//::invalidate(ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip& bb_ip, int reason){
//    return true;
//}
//
//bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list
//::invalidate(ppcsimbooke::ppcsimbooke_basic_block::basic_block* bb, int reason){
//    return true;
//}
//
//size_t ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list::get_page_bb_count(uint64_t mfn){
//    return 0;
//}
//
//void ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list::flush(){
//}
//
//std::ostream& operator<<(std::ostream& ostr, ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list& cl){
//    return ostr;
//}

/////////////////////////////////////////////////////////////////////////////////
// basic block
/////////////////////////////////////////////////////////////////////////////////

void ppcsimbooke::ppcsimbooke_basic_block::basic_block::reset(){
    hashlink.reset();
    mfn_loc.reset();
    ip_taken = 0;
    ip_not_taken = 0;
    transopscount = 0;
    brtype = BB_BRTYPE_INV;
    synthops = NULL;
    refcount = 0;
    hitcount = 0;
    lastused = 0;
    lasttarget = 0;
}

void ppcsimbooke::ppcsimbooke_basic_block::basic_block::reset(const ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip& bb_ip){
    bip = bb_ip;
    reset();
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block::basic_block(){
    bip.reset();
    reset();
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block::basic_block(const ppcsimbooke::ppcsimbooke_basic_block::basic_block& bb){
    *this = bb;
    hashlink.reset();
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block::basic_block(const ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip& bb_ip){
    bip = bb_ip;
    reset();
}

// destructor
ppcsimbooke::ppcsimbooke_basic_block::basic_block::~basic_block(){
    //if likely(synthops) delete[] synthops;
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block* ppcsimbooke::ppcsimbooke_basic_block::basic_block::clone(){
    basic_block* bb_new = new basic_block;
    memcpy(bb_new, this, sizeof(basic_block));

    bb_new->hashlink.reset();
    bb_new->use(0);

    return bb_new;
}

// NOTE: This function needs to be paired with clone(), otherwise nasty things may happen.
void ppcsimbooke::ppcsimbooke_basic_block::basic_block::free(){
    if likely(synthops) delete[] synthops;
    delete this;    // commit suicide
}

// initialize opcode implementation functions' array
void ppcsimbooke::ppcsimbooke_basic_block::basic_block::init_synthops(ppcsimbooke::ppcsimbooke_cpu::cpu& ctx){
    if likely(synthops == NULL){ synthops = new opc_impl_func_t[transopscount]; }

    for(size_t i=0; i<transopscount; i++){
        synthops[i] = ctx.get_opc_impl(transops[i].hv);
    }
}


/////////////////////////////////////////////////////////////////////////////////
// basic block decoder
/////////////////////////////////////////////////////////////////////////////////

void ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::reset(){
    transbuffcount = 0;
    insnbytes_buffsize = 0;
    valid_byte_count = 0;
    ip_decoded = ip_copied = bb.bip;
    byteoffset = 0;
    invalid = false;
    outcome = 0;
    fault_addr = 0;
    fault_cause = 0;
    branch_cond = BB_BRTYPE_SPLIT;
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::basic_block_decoder
(const ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip& bip){
    bb.bip = bip;
    reset(); 
}

ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::basic_block_decoder(ppcsimbooke::ppcsimbooke_cpu::cpu& ctx){
    bb.bip = basic_block_ip(ctx);
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
        ic = decoder.disasm(insnbytes[byteoffset], ip_decoded, endianness);
        transbuff[transbuffcount++] = ic;
        // if current instruction is a control transfer instruction, end basic_block &
        // return immediately
        if(decoder.is_control_xfer(ic)){
            branch_cond = BB_BRTYPE_BRANCH;
            flush();
            return true;
        }
        byteoffset += OPCODE_SIZE;
        ip_decoded += OPCODE_SIZE;           // update decoded ip
    }
    return true;
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::flush(){
    if unlikely(!transbuffcount){ return false; }

    instr_call ic;

    for(size_t i=0; i<transbuffcount; i++){
        if unlikely(bb.transopscount >= MAX_BB_INS){
            std::cerr << "Instruction limit for basic block exceeded !!" << std::endl;
            split();
            return false;
        }
        ic = transbuff[i];
        bb.transops[bb.transopscount++] = ic;
        ip_copied += OPCODE_SIZE;                // update copied ip
    }

    transbuffcount = 0;

    return true;
}

void ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder::split(){
    bb.ip_taken = ip_copied;
    bb.ip_not_taken = ip_copied;
    bb.brtype = branch_cond;
    //end_of_block = 1;
}