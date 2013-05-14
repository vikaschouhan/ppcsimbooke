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

std::ostream& ppcsimbooke::ppcsimbooke_basic_block::operator<<(std::ostream& ostr, const ppcsimbooke::ppcsimbooke_basic_block::basic_block_ip& bip){
    ostr << std::hex << std::showbase;
    ostr << "[IP:" << bip.ip << " MFN:" << bip.mfn << " MSR:" << bip.msr << " PIDS=["
         << bip.pid0 << "," << bip.pid1 << "," << bip.pid2 << "]" << " BE:" << bip.be << "]" << std::endl;
    ostr << std::dec << std::noshowbase;
    return ostr;
}

/////////////////////////////////////////////////////////////////////////////////
// basic block chunk list
/////////////////////////////////////////////////////////////////////////////////

std::ostream& ppcsimbooke::ppcsimbooke_basic_block::operator<<(std::ostream& ostr, const ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list& bb_cl){
    ppcsimbooke::ppcsimbooke_basic_block::basic_block** bb_ptr = NULL;
    ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list::Iterator
        iter(const_cast<ppcsimbooke::ppcsimbooke_basic_block::basic_block_chunk_list*>(&bb_cl));

    ostr << "[basic_block_chunk_list:" << &bb_cl << "]" << std::endl;
    while((bb_ptr = iter.next())){
        ostr << (**bb_ptr);
    }

    return ostr;
}


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

std::ostream& ppcsimbooke::ppcsimbooke_basic_block::operator<<(std::ostream& ostr, const ppcsimbooke::ppcsimbooke_basic_block::basic_block& bb){
    ostr << std::showbase << std::hex;
    ostr << "[BB:" << &bb << "]" << std::endl;
    ostr << "    " << "bip : " << bb.bip << std::endl;
    ostr << "    " << "ip_taken : " << bb.ip_taken << std::endl;
    ostr << "    " << "ip_not_taken : " << bb.ip_not_taken << std::endl;
    ostr << "    " << "transopscount : " << bb.transopscount << std::endl;
    ostr << "    " << "brtype : " << bb.brtype << std::endl;
    ostr << "    " << "refcount : " << bb.refcount << std::endl;
    ostr << "    " << "hitcount : " << bb.hitcount << std::endl;
    ostr << "    " << "lastused : " << bb.lastused << std::endl;
    ostr << "    " << "lasttarget : " << bb.lasttarget << std::endl;
    ostr << "    " << "TRANSOPS : " << std::endl;
    ostr << "    " << "[";

    for(size_t i=0; i<(bb.transopscount-1); i++){
        ostr << bb.transops[i] << ",";
    }
    ostr <<  bb.transops[bb.transopscount-1] << "]" << std::endl;
    ostr << std::endl;
    ostr << std::noshowbase << std::dec;
    return ostr;
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

//////////////////////////////////////////////////////////////////////////////////////////////
// basic block cache unit
//////////////////////////////////////////////////////////////////////////////////////////////

// translate current context into a single basic block & insert it into basic block cache
ppcsimbooke::ppcsimbooke_basic_block::basic_block*
ppcsimbooke::ppcsimbooke_basic_block::basic_block_cache_unit::translate(ppcsimbooke::ppcsimbooke_basic_block::context& ctx){
    basic_block_ip bb_ip(ctx);
    basic_block_chunk_list* page_list = NULL;

    basic_block* bb = m_bb_cache.get(bb_ip);
    if likely(bb) return bb;

    uint8_t bb_insnbuff[MAX_BB_INS_BYTES];
    basic_block_decoder bb_decoder(bb_ip);

    bb_decoder.fillbuff(ctx, bb_insnbuff, MAX_BB_INS_BYTES);           // Assign buffer for decoder to work on
    bb_decoder.decode();                                               // decode
    bb = bb_decoder.bb.clone();                                        // get basic block

    // Add to basic block cache
    bb->acquire();                  // acquire basic block
    m_bb_cache.add(bb);             // add to cache
    bb->release();

    // Add to page list cache
    page_list = m_bb_page_cache.get(bb->bip.mfn);
    if(!page_list){
        page_list = new basic_block_chunk_list(bb->bip.mfn);
        //page_list->refcount++;
        m_bb_page_cache.add(page_list);
    }
    page_list->refcount++;
    page_list->add(bb, bb->mfn_loc);
    page_list->refcount--;

    return bb;
}

// invalidate basic block match current context
bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_cache_unit::invalidate(const basic_block_ip& bb_ip, int reason){
    basic_block* bb = m_bb_cache.get(bb_ip);
    if(!bb) return true;
    return invalidate(bb, reason);
}

bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_cache_unit::invalidate(ppcsimbooke::ppcsimbooke_basic_block::basic_block* bb, int reason){
    basic_block_chunk_list* page_list = NULL;

    if unlikely(bb->refcount){
        LOG_DEBUG4("basic_block ", bb, " is still in use somewhere. RefCount = ", bb->refcount);
        return false;
    }

    page_list = m_bb_page_cache.get(bb->bip.mfn);
    page_list->remove(bb->mfn_loc);         // Remove this basic block from page list cache 
    m_bb_cache.remove(bb);                  // Remove this from basic block cache

    bb->free();
    return true;
}

// invalidate all basic blocks belonging to a physical page no
bool ppcsimbooke::ppcsimbooke_basic_block::basic_block_cache_unit::invalidate_page(uint64_t mfn, int reason){
    if unlikely(mfn == basic_block_ip::MFN_INV){ return false; }

    basic_block_chunk_list* page_list = m_bb_page_cache.get(mfn);
    if unlikely(!page_list){ return false; }

    basic_block_chunk_list::Iterator iter(page_list);
    basic_block*  bb = NULL;
    basic_block** bb_ptr = NULL;

    while((bb_ptr = iter.next())){
        bb = *bb_ptr;
        if unlikely(!invalidate(bb, reason)){
            LOG_DEBUG4("Couldn't invalidate ", *bb, " std::endl");
            return false;
        }
    }

    page_list->clear();
    return true;
}

// Return total number of basic blocks belonging to a physical page no
size_t ppcsimbooke::ppcsimbooke_basic_block::basic_block_cache_unit::get_page_bb_count(uint64_t mfn){
    if unlikely(mfn == basic_block_ip::MFN_INV){ return 0; }

    basic_block_chunk_list* page_list = m_bb_page_cache.get(mfn);
    if unlikely(page_list){ return 0; }

    return page_list->count();
}

// flush all caches
void ppcsimbooke::ppcsimbooke_basic_block::basic_block_cache_unit::flush(){
    basic_block_cache::Iterator iter_bc(m_bb_cache);
    basic_block* bptr;
    while((bptr = iter_bc.next())){ invalidate(bptr, INVALIDATE_REASON_RECLAIM); }

    basic_block_page_cache::Iterator iter_pc(m_bb_page_cache);
    basic_block_chunk_list *page;
    while((page = iter_pc.next())){
        m_bb_page_cache.remove(page);
        delete page;
    }
}
