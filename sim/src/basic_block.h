#ifndef BASIC_BLOCK_H_
#define BASIC_BLOCK_H_

//#include "config.h"
#include "superstl.h"
#include "ppc_dis.h"
#include "globals.h"

namespace ppcsimbooke {
    // forward declarations
    namespace ppcsimbooke_cpu {
        class cpu;
    }

    namespace ppcsimbooke_basic_block {

        static const size_t OPCODE_SIZE = 4;

        // typedefs
        typedef ppcsimbooke_cpu::cpu context;

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
            static const uint64_t   MFN_INV = (1ULL << CPU_PHY_ADDR_SIZE) - 1;
            static const uint64_t   IP_INV  = 0xffffffffffffffffULL;
        
            operator uint64_t() const { return ip; }
            basic_block_ip();
            basic_block_ip(uint64_t ip_, uint64_t mfn_, uint32_t msr_, uint32_t pid0_, uint32_t pid1_, uint32_t pid2_, bool be_);
            basic_block_ip(context& ctx);

            int get_endianness(){ return (be) ? EMUL_BIG_ENDIAN:EMUL_LITTLE_ENDIAN; }
            void reset();
            bool operator==(const basic_block_ip &bip) const {
                return !memcmp(this, &bip, sizeof(basic_block_ip));
            }
        };
       
        // basic block ip overload for std::cout 
        std::ostream& operator<<(std::ostream& ostr, const basic_block_ip& bip);
    
        ////////////////////////////////////////////////////////////////////////////////
        // basic block chunk list
        ////////////////////////////////////////////////////////////////////////////////
    
        static const size_t BB_PTRS_PER_CHUNK = 60;
    
        struct basic_block;
        
        struct basic_block_chunk_list: public superstl::ChunkList<basic_block*, BB_PTRS_PER_CHUNK> {
            superstl::selflistlink               hashlink;
            uint64_t                             mfn;
            int                                  refcount;
            
            basic_block_chunk_list(): ChunkList<basic_block*, BB_PTRS_PER_CHUNK>() { refcount = 0; }
            basic_block_chunk_list(uint64_t mfn): ChunkList<basic_block*, BB_PTRS_PER_CHUNK>() { this->mfn = mfn; refcount = 0; }
        };

        // basic block chunk list overload for std::cout
        std::ostream& operator<<(std::ostream& ostr, const basic_block_chunk_list& bb_cl);

        //////////////////////////////////////////////////////////////////////////
        // basic block
        //////////////////////////////////////////////////////////////////////////
    
        static const size_t MAX_BB_INS = 128;
        static const size_t MAX_BB_INS_BYTES = MAX_BB_INS * 4;

        enum { BB_BRTYPE_SPLIT, BB_BRTYPE_BRANCH, BB_BRTYPE_INV };

        //typedef typename ppcsimbooke::ppcsimbooke_cpu::cpu::ppc_opc_fun_ptr  opc_impl_func_t;   // powerpc opcode handler type
        typedef void (*opc_impl_func_t)(context *pcpu, ppcsimbooke::instr_call *pic);
        
        // Basic Block
        struct basic_block {
            basic_block_ip                   bip;                      // basic block ip
            superstl::selflistlink           hashlink;
            basic_block_chunk_list::Locator  mfn_loc;
            uint64_t                         ip_taken;
            uint64_t                         ip_not_taken;
            size_t                           transopscount;
            uint8_t                          brtype;
            opc_impl_func_t*                 synthops;
            int                              refcount;
            uint32_t                         hitcount;
            uint64_t                         lastused;
            uint64_t                         lasttarget;
        
            instr_call                       transops[MAX_BB_INS];
           
            basic_block();
            basic_block(const basic_block_ip& bb_ip);
            basic_block(const basic_block& bb);

            ~basic_block();

            void acquire() { refcount++; }
            bool release() { refcount--; assert(refcount >= 0); return (!refcount); }
        
            void reset();
            void reset(const basic_block_ip& ip);
            void use(uint64_t counter) { lastused = counter; }

            // basic block pointer management
            basic_block* clone();                      // clone basic block
            void         free();                       // free basic block

            // Initialize synthops from passed context
            void init_synthops(context &ctx);
            // update branch targets for next basic block
            void update_targets(context& ctx);

            // instruction objects can be inserted directly into the basic block
            basic_block& operator<<(instr_call& ic){
                if unlikely(transopscount >= MAX_BB_INS){ return *this; }
                transops[transopscount++] = ic;
                return *this;
            }
        };
        
        std::ostream& operator <<(std::ostream& ostr, const basic_block& bb);

        ///////////////////////////////////////////////////////////////////////////////////////////
        // basic block page cache / cache
        ///////////////////////////////////////////////////////////////////////////////////////////
        static const size_t BB_PAGE_CACHE_SIZE = 32*1024;  // 32K
        static const size_t BB_CACHE_SIZE      = 32*1024;  // 32 K

        // Basic Block Chunk List Hash Table Link Manager
        struct basic_block_chunk_list_hash_table_link_manager {
            static inline basic_block_chunk_list* objof(superstl::selflistlink* link) {
                return superstl_baseof(basic_block_chunk_list, hashlink, link);
            }

            static inline uint64_t& keyof(basic_block_chunk_list* obj) {
                return obj->mfn;
            }

            static inline superstl::selflistlink* linkof(basic_block_chunk_list* obj) {
                return &obj->hashlink;
            }
        };

        // Basic Block hash table link manager
        struct basic_block_hash_table_link_manager {
            static inline basic_block* objof(superstl::selflistlink* link) {
                return superstl_baseof(basic_block, hashlink, link);
            }

            static inline basic_block_ip& keyof(basic_block* obj) {
                return obj->bip;
            }

            static inline superstl::selflistlink* linkof(basic_block* obj) {
                return &obj->hashlink;
            }
        };

        // Basic Block hash table key manager (key type = basic_block_ip)
        struct basic_block_hash_table_key_manager {
            static inline int hash(const basic_block_ip& key){
                superstl::CRC32 h;
                h << key;
                return h;
            }

            static inline bool equal(const basic_block_ip& a, const basic_block_ip& b){
                return a == b;
            }

            static inline basic_block_ip dup(const basic_block_ip& key){
                return key;
            }

            static inline void free(basic_block_ip& key) {}
        };

        // basic block page cache
        typedef superstl::SelfHashtable<uint64_t, basic_block_chunk_list,
                BB_PAGE_CACHE_SIZE, basic_block_chunk_list_hash_table_link_manager> basic_block_page_cache;

        // basic block cache
        typedef superstl::SelfHashtable<basic_block_ip, basic_block,
                BB_CACHE_SIZE, basic_block_hash_table_link_manager, basic_block_hash_table_key_manager> basic_block_cache;


        ///////////////////////////////////////////////////////////////////////////////////////////
        //   basic block decoder
        ///////////////////////////////////////////////////////////////////////////////////////////
        
        // branch cond, uncond, indirect, return from interrupt & system call
        //enum { BB_TYPE_BR_COND, BB_TYPE_BR_UNCOND, BB_TYPE_BR_INDIR, BB_TYPE_RFI, BB_TYPE_SC };
        //static const char* bb_type_names[] = { "branch conditional", "branch unconditional", "branch indirect",
        //                                       "rfXi", "system call" };
    
        // Maximum instructions which can be translated before a flush is required
        static const size_t MAX_TRANS_INS = 32;
        
        // Decode meant for basic block
        struct basic_block_decoder {
            basic_block       bb;
            
            instr_call        transbuff[MAX_TRANS_INS];
            size_t            transbuffcount;
            uint8_t           *insnbytes;
            size_t            insnbytes_buffsize;
            size_t            valid_byte_count;
            uint64_t          ip_decoded;                      // ip up to which instructions are already decoded
            uint64_t          ip_copied;                       // ip up to which instructions are copied to basic block
            size_t            byteoffset;
            bool              invalid;
            uint8_t           branch_cond;                     // branch condition
            bool              outcome;
            uint64_t          fault_addr;                      // page fault address (tlb miss generating addr)
            int               fault_cause;                     // contains the exact fault error type

            ppcsimbooke::ppcsimbooke_dis::ppcdis           decoder;     // decoder module
            
            basic_block_decoder(const basic_block_ip& rvp);
            basic_block_decoder(context &ctx);       // cpu maintains the context
            basic_block_decoder(uint64_t ip, bool kernel, bool df);
            
            void reset();
            int  fillbuff(context &ctx, uint8_t* insn_buff, int insn_buffsize);
    
            bool invalidate();
            bool decode();
            bool flush();
            void split();
            bool first_insn_in_bb() { return (uint64_t(ip_decoded) == uint64_t(bb.bip.ip)); }
        };


        ////////////////////////////////////////////////////////////////////////////////////////////
        // basic block combined cache (maintains basic block cache as well as page cache)
        ////////////////////////////////////////////////////////////////////////////////////////////
        // cache invalidate reasons
        enum {
            INVALIDATE_REASON_DIRTY,
            INVALIDATE_REASON_RECLAIM,
            INVALIDATE_REASON_COUNT,
        };

        // Basic block cache unit
        class basic_block_cache_unit {
            public:
            basic_block* translate(context& ctx);
            bool         invalidate(const basic_block_ip& b_ip, int reason);
            bool         invalidate(basic_block* bb, int reason);
            bool         invalidate_page(uint64_t mfn, int reason);
            size_t       get_page_bb_count(uint64_t mfn);
            void         flush();

            private:
            basic_block_cache         m_bb_cache;
            basic_block_page_cache    m_bb_page_cache;
        };
    
    }
}

#endif
