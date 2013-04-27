#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

//#include "config.h"
#include "superstl.h"
#include "ppc_dis.h"

namespace ppcsimbooke {
    // forward declarations
    namespace ppcsimbooke_cpu {
        class cpu;
    }

    namespace ppcsimbooke_basic_block {

        //////////////////////////////////////////////////////////////////////////////
        // basic block instruction pointer
        //////////////////////////////////////////////////////////////////////////////
    
        // Basic Block instruction pointer.
        // Identifies a basic block uniquely (on basis of conext in which it was translated)
        struct basic_block_ip {
            uint64_t                ip;            // instruction pointer
            uint64_t                mfn;           // machine frame number (physical page no)
            uint32_t                msr;           // Machine state register
            uint32_t                pid0;          // PID registers
            uint32_t                pid1;
            uint32_t                pid2;
            bool                    be;            // if page is big endian
            static const uint64_t   INV = (1ULL << CPU_PHY_ADDR_SIZE) - 1;
        
            operator uint64_t() const { return ip; }
            basic_block_ip() {}
            basic_block_ip(uint64_t ip_, uint64_t mfn_, uint32_t msr_, uint32_t pid0_, uint32_t pid1_, uint32_t pid2_, bool be_) :
                ip(ip_), mfn(mfn_), msr(msr_), pid0(pid0_), pid1(pid1_), pid2(pid2_), be(be_) {}
            bool operator==(const basic_block_ip &bip) const {
                return !memcmp(this, &bip, sizeof(basic_block_ip));
            }
        };
        
        inline std::ostream& operator<<(std::ostream& ostr, basic_block_ip & bip){
            ostr << std::hex << std::showbase;
            ostr << "[IP:" << bip.ip << " MFN:" << bip.mfn << " MSR:" << bip.msr << " PIDS=["
                 << bip.pid0 << "," << bip.pid1 << "," << bip.pid2 << "]" << " BE:" << bip.be << "]" << std::endl;
            ostr << std::dec << std::noshowbase;
            return ostr;
        }
    
        ////////////////////////////////////////////////////////////////////////////////
        // basic block & basiic block chunk list manager
        ////////////////////////////////////////////////////////////////////////////////
    
        static const int BB_PTRS_PER_CHUNK = 60;
    
        struct basic_block;
        
        struct basic_block_chunk_list: public superstl::ChunkList<basic_block*, BB_PTRS_PER_CHUNK> {
            superstl::selflistlink               hashlink;
            uint64_t                             mfn;
            int                                  refcount;
            
            basic_block_chunk_list(): ChunkList<basic_block*, BB_PTRS_PER_CHUNK>() { refcount = 0; }
            basic_block_chunk_list(uint64_t mfn): ChunkList<basic_block*, BB_PTRS_PER_CHUNK>() { this->mfn = mfn; refcount = 0; }
        };
    
        static const int MAX_BB_INS = 128;

        //typedef typename ppcsimbooke::ppcsimbooke_cpu::cpu::ppc_opc_fun_ptr  opc_impl_func_t;   // powerpc opcode handler type
        typedef void (*opc_impl_func_t)(ppcsimbooke::ppcsimbooke_cpu::cpu *pcpu, ppcsimbooke::instr_call *pic);
        
        // Basic Block
        struct basic_block {
            basic_block_ip                   bip;                      // basic block ip
            superstl::selflistlink           hashlink;
            basic_block_chunk_list::Locator  mfn_loc;
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
        
            instr_call                       ops[MAX_BB_INS];
        
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
        
        inline std::ostream& operator <<(std::ostream& ostr, const basic_block& bb){
            ostr << std::showbase << std::hex;
            ostr << "PLEASE IMPLEMENT THIS." << std::endl;
            ostr << std::noshowbase << std::dec;
            return ostr;
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        //   basic block decoder
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        // branch cond, uncond, indirect, return from interrupt & system call
        //enum { BB_TYPE_BR_COND, BB_TYPE_BR_UNCOND, BB_TYPE_BR_INDIR, BB_TYPE_RFI, BB_TYPE_SC };
        //static const char* bb_type_names[] = { "branch conditional", "branch unconditional", "branch indirect",
        //                                       "rfXi", "system call" };
    
        // Maximum instructions which can be translated before a flush is required
        static const int MAX_TRANS_INS = 32;
        
        // Decode meant for basic block
        struct basic_block_decoder {
            basic_block       bb;
            
            instr_call        transbuf[MAX_TRANS_INS];
            int               transbufcount;
            uint8_t           *insnbytes;
            int               insnbytes_buffsize;
            int               valid_byte_count;
            uint64_t          ip;
            uint64_t          ipstart;
            int               byteoffset;
            int               op;
            uint64_t          insn_count;
            bool              invalid;
            int               outcome;
            uint64_t          fault_addr;                      // page fault address (tlb miss generating addr)
            int               fault_cause;                     // contains the exact fault error type

            ppcsimbooke::ppcsimbooke_dis::ppcdis           decoder;     // decoder module
            
            basic_block_decoder(const basic_block_ip& rvp);
            basic_block_decoder(ppcsimbooke::ppcsimbooke_cpu::cpu &ctx, uint64_t ip);       // cpu maintains the context
            basic_block_decoder(uint64_t ip, bool kernel, bool df);
            
            void reset();
            int  fillbuff(ppcsimbooke::ppcsimbooke_cpu::cpu &ctx, uint8_t* insn_buff, int insn_buffsize);
    
            inline uint32_t fetch() { uint32_t r = *((uint32_t*)&insnbytes[byteoffset]); ip += 4; byteoffset += 4; return r; }
    
            bool invalidate();
            bool decode();
            bool flush();
            bool first_insn_in_bb() { return (uint64_t(ipstart) == uint64_t(bb.bip.ip)); }
        };
    
    }
}

#endif
