#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

#include "config.h"
#include "sstl.h"

namespace ppcsimbooke_basic_block {
    //////////////////////////////////////////////////////////////////////////////
    // basic block instruction pointer
    //////////////////////////////////////////////////////////////////////////////

    // Basic Block instruction pointer.
    // Identifies a basic block uniquely
    struct basic_block_ip {
        uint64_t                ip;
        uint64_t                mfn_lo;
        uint64_t                mfn_hi;
        uint32_t                msr;
        static const uint64_t   INV = (1ULL << CPU_PHY_ADDR_SIZE) - 1;
    
        operator uint64_t() const { return ip; }
        basic_block_ip() {}
        basic_block_ip(uint64_t ip_) : ip(ip_) {}
        basic_block_ip(uint64_t ip_, uint64_t mfn_lo_, uint64_t mfn_hi_, uint64_t msr_) :
            ip(ip_), mfn_lo(mfn_lo_), mfn_hi(mfn_hi_), msr(msr_) {}
        bool operator==(const bb_ip &bip) const {
            return (bip.ip == ip) && (bip.mfn_lo == mfn_lo) && (bip.mfn_hi == mfn_hi) && (bip.msr == msr);
        }
    };
    
    inline std::ostream& operator<<(std::ostream& ostr, basic_block_ip & bip){
        ostr << std::hex << std::showbase;
        ostr << "[IP:" << bip.ip << " MFN_LO:" << bip.mfn_lo << " MFN_HI:" << bip.mfn_hi << " MSR:" << bip.msr << "]" << std::endl;
        ostr << std::dec << std::noshowbase;
        return ostr;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // basic block & basiic block chunk list manager
    ////////////////////////////////////////////////////////////////////////////////

#define BB_PTRS_PER_CHUNK 60

    struct basic_block;
    
    struct basic_block_chunk_list: public ChunkList<basic_block*, BB_PTRS_PER_CHUNK> {
      selflistlink hashlink;
      W64 mfn;
      int refcount;
    
      basic_block_chunk_list(): ChunkList<basic_block*, BB_PTRS_PER_CHUNK>() { refcount = 0; }
      basic_block_chunk_list(uint64_t mfn): ChunkList<basic_block*, BB_PTRS_PER_CHUNK>() { this->mfn = mfn; refcount = 0; }
    };
    
    // Basic Block
    struct basic_block {
        basic_block                      ip;
        selflistlink                     hashlink;
        basic_block_chunk_list::locator  mfnlo_loc;
        basic_block_chunk_list::locator  mfnhi_loc;
        uint64_t                         ip_taken;
        uint64_t                         ip_not_taken;
        uint16_t                         count;
        uint16_t                         tagcount;
        uint16_t                         memcount;
        uint16_t                         storecount;
        uint8_t                          type:4, repblock:1, invalidblock:1, call:1, ret:1;
        uint8_t                          marked:1, mfence:1, nondeterministic:1, brtype:3;
        uint64_t                         usedregs;
        opc_impl_func_t*                 synthops;
        int                              refcount;
        uint32_t                         hitcount;
        uint32_t                         predcount;
        uint32_t                         confidence;
        uint64_t                         lastused;
        uint64_t                         lasttarget;
    
        instr_call                       ops[MAX_BB_OPS*2];
    
        void acquire() {
            refcount++;
        }
    
        bool release() {
            refcount--;
            assert(refcount >= 0);
            return (!refcount);
        }
    
        void reset();
        void reset(const basic_block_ip& ip);
        basic_block* clone();
        void free();
        void use(uint64_t counter) { lastused = counter; };
    };
    
    ostream& operator <<(ostream& os, const BasicBlock& bb);
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //   basic block decoder
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    // branch cond, uncond, indirect, return from interrupt & system call
    enum { BB_TYPE_BR_COND, BB_TYPE_BR_UNCOND, BB_TYPE_BR_INDIR, BB_TYPE_RFI, BB_TYPE_SC };
    const char* bb_type_names[] = { "branch conditional", "branch unconditional", "branch indirect",
        "rfXi", "system call" };
    
    // Decode meant for basic block
    struct basic_block_decoder {
        basic_block       bb;
        DIS_PPC           dis_asm;
        instr_call        transbuf[MAX_TRANSOPS_PER_USER_INSN];
        int               transbufcount;
        bool              kernel;
        bool              dirflag;
        uint8_t           *insnbytes;
        int               insnbytes_buffsize;
        uint64_t          ip;
        uint64_t          ipstart;
        int               byteoffset;
        int               op;
        uint64_t          user_insn_count;
        bool              invalid;
        int               outcome;
        
        basic_block_decoder(const basic_block_ip& rvp);
        basic_block_decoder(CPU_PPC &ctx, Waddr rip);       // cpu maintains the context
        basic_block_decoder(uint64_t ip, bool kernel, bool df);
        
        void reset();
        void fillbuf(CPU_PPC &ctx, uint8_t* insn_buff, int insn_buffsize);

        inline uint32_t fetch() { uint32_t r = *((uint32_t*)&insnbytes[byteoffset]); ip += 4; byteoffset += 4; return r; }

        bool invalidate();
        bool decode();
        bool flush();
        void split(bool after);
        void split_before() { split(0); }
        void split_after() { split(1); }
        bool first_insn_in_bb() { return (uint64_t(ipstart) == uint64_t(bb.ip)); }
    };

}

#endif
