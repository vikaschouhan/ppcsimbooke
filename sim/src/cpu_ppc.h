// cpu_ppc.h (core powerPC cpu module)
// This file contains cpu class & corresponding member functions.
// 
// Author : Vikas Chouhan (presentisgood@gmail.com)
// Copyright 2012/2013.
// 
// This file is part of ppcsimbooke library.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.
// 
// It is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file; see the file COPYING.  If not, write to the
// Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef    _CPU_PPC_HPP_
#define    _CPU_PPC_HPP_

#include "config.h"
#include "tlb_booke.h"               // BookE MMU
#include "ppc_exception.h"           // Exception tables
#include "ppc_dis.h"                 // Disassembler module
#include "memory.h"                  // Memory module
#include "bm.h"                      // Software Breakpoint manager
#include "cpu_ppc_coverage.h"        // Coverage logger
#include "fpu_emul.h"                // floating point emulation
#include "basic_block.h"             // Basic block decoder & caches module

namespace ppcsimbooke {
    namespace ppcsimbooke_cpu {

        class cpu;
        void gen_ppc_opc_func_hash(cpu *pcpu);

        // cpu run mode
        enum cpu_run_mode {
            CPU_MODE_RUNNING   = 1,
            CPU_MODE_STEPPING  = 2,
            CPU_MODE_HALTED    = 3,
            CPU_MODE_STOPPED   = 4,
        };

        enum cpu_exec_mode {
            CPU_EXEC_MODE_INTERPRETIVE = 1,
            CPU_EXEC_MODE_THREADED     = 2,
        };

        // Debug events
        static const int DBG_EVENT_IAC     = 0x00000001UL;
        static const int DBG_EVENT_DAC_LD  = 0x00000002UL;
        static const int DBG_EVENT_DAC_ST  = 0x00000004UL;
        
        /* 64 bit MSRs were used in older powerPC designs */
        /* All BookE cores have 32 bit MSRs only */
        
        class cpu {
            ////////////////////////////////////////////////////////////////////////////////
            // friends
            ///////////////////////////////////////////////////////////////////////////////

            // This function defines nested functions (using struct encapsulation technique)
            // Each function implements the pseudocode for one (exactly one) ppc opcode.
            //
            // For eg. there is one function for "add", another for "or" and likewise
            //
            // NOTE : It's the responsibility of this function to populate m_ppc_func_hash and
            //        it has to be called once in the constructor.
            friend void gen_ppc_opc_func_hash(ppcsimbooke::ppcsimbooke_cpu::cpu *pcpu);
            // Basic Block [decoder] is cpu's friend
            friend struct ppcsimbooke::ppcsimbooke_basic_block::basic_block_decoder;
            friend struct ppcsimbooke::ppcsimbooke_basic_block::basic_block;

            /////////////////////////////////////////////////////////////////////////
            // public typedefs
            /////////////////////////////////////////////////////////////////////////
            public:
            // function pointer type for opcode handlers
            typedef void (*ppc_opc_fun_ptr)(ppcsimbooke::ppcsimbooke_cpu::cpu *, ppcsimbooke::instr_call *);
            // translated result type for tlb array
            typedef std::tuple<uint64_t, uint8_t, uint64_t>  xlated_tlb_res;
       
            ////////////////////////////////////////////////////////////////////////
            // member functions
            //////////////////////////////////////////////////////////////////////// 
            public:
            cpu();
            cpu(uint64_t cpuid, std::string name);
            ~cpu();

            // cpu information
            uint64_t   get_bits();                              // Get CPU bitsize
            uint64_t   get_pc_mask();                           // Get PC bitmask
            uint64_t   get_pc();                                // Get pc
            uint64_t   get_nip();                               // Get NIP
      
            // cpu control  
            void       init(uint64_t cpuid, std::string name);     // Initialize CPU 
            void       register_mem(ppcsimbooke_memory::memory &mem);               // Register memory
            size_t     get_ninstrs();                           // Get number of instrs
           
            // Top level run/step functions 
            void       run();                                   // Non blocking run
            void       run_instr(std::string instr);
            void       run_instr(uint32_t opcd);
            void       step(size_t instr_cnt=1);      // by default step by 1 ic cnt

            // run mode functions
            void       halt();                        // halt cpu
            void       stop();                        // stop cpu
            void       run_mode();                    // prints run mode

            // execution mode functions
            void       exec_mode();                         // prints exec mode
            void       set_exec_mode_threaded();            // switch exec mode to threaded execution
            void       set_exec_mode_interpretive();        // switch exec mode to interpretive execution
           
            // Memory (EA) access functions
            //
            // All memory read/write functions (these act on effective addresses and address
            // translation is done by the tlb module)
            // These only act on data pages
            // Even if storing to an instruction page, it should
            // be treated as data only i.e a separate data tlb entry should be defined which maps
            // to the instruction physical page.
            // NOTE : store to an instruction page always goes through DTLB. ITLB is only used for
            // instruction fetches.
            uint8_t    read8(uint64_t addr);
            void       write8(uint64_t addr, uint8_t value);
            uint16_t   read16(uint64_t addr);
            void       write16(uint64_t addr, uint16_t value);
            uint32_t   read32(uint64_t addr);
            void       write32(uint64_t addr, uint32_t value);
            uint64_t   read64(uint64_t addr);
            void       write64(uint64_t addr, uint64_t value);
            
            // Read memory buffer.
            // It can read instruction as well as data pages.
            // ex=0, means data pages (with read_permission enabled),
            // ex=1, means instruction pages (with read_permission enabled)
            // returns number of bytes read
            size_t     read_buff(uint64_t addr, uint8_t *buff, size_t buffsize, bool ex=0);
       
            // Register state functions
            uint64_t   get_reg(std::string name) throw(sim_except);    // Get register by name
            uint64_t   get_reg(int regid) throw(sim_except);           // Get register by reg_id
            void       dump_state(std::ostream &ostr=std::cout);       // Dump Cpu state
            
            // TLB functions
            xlated_tlb_res  xlate(uint64_t addr, bool wr=0, bool ex=0);  // Translate EA to RA (return a tuple of <xlated addr, wimge, page_size>)
            void            print_L2tlbs();                              // Print L2 tlbs

            // get opcode implementation function pointer
            ppc_opc_fun_ptr  get_opc_impl(uint64_t opcode_hash);
        
            // Logging functions
            void       enable_cov_log();
            void       disable_cov_log();
            bool       is_cov_log_enabled();
            void       gen_cov_log();
            void       log_cov_to_file(std::string filename);
        
            // for boost::python
            ppc_regs& ___get_regs(){
                return m_cpu_regs;
            }
        
            protected:
            void       set_resv(uint64_t ea, size_t size);
            void       clear_resv(uint64_t ea);
            bool       check_resv(uint64_t ea, size_t size);
            void       notify_ctxt_switch();   // notify of context switches ( called by context syncronizing instructions  such as rfi, sc etc. )
        
            // Accessing registers using reghash interface ( for use with ppc code translation unit )
            inline ppc_reg64*      reg(int regid);
            inline ppc_reg64*      regn(std::string regname);
        
            private:
            void                  run_b();                                    // blocking run
            void                  run_basic_blocks_b();                       // basic block blocking run
            void                  clear_ctrs();                               // clear counters
            void                  ppc_exception(int exception_nr, int subtype, uint64_t ea=0xffffffffffffffffULL);
            instr_call            get_instr();                                 // Automatically tries to read instr from next NIP(PC)
            void                  check_for_dbg_events(int flags, uint64_t ea=0);   // check for debug events
            inline void           run_curr_instr();                                 // run current instr
            inline void           init_common();

            //////////////////////////////////////////////////////////////////////
            // data members
            //////////////////////////////////////////////////////////////////////
            public:
            breakpt_mngr                           m_bm;                   // breakpoint manager
            static Log<1>                          sm_instr_tracer;        // Instruction tracer module
            
            private: 
            ppcsimbooke_basic_block::basic_block_cache_unit   m_bb_cache_unit;
        
            std::string                            m_cpu_name;
            cpu_run_mode                           m_cpu_mode;            // Run mode : Running/Halted/Stopped etc.
            cpu_exec_mode                          m_cpu_exec_mode;       // Execution mode : Interpretive/Threaded (basic block)
            int                                    m_cpu_bits;            // 32 or 64
            bool                                   m_cpu_running;         // If CPU is in run mode
        
            // cache attributes
            const size_t                           m_cache_line_size;     // Cache line size 
        
            // Reservation
            // FIXME: Reservation support is not fully implemented.
            //        It may not work for multicore at all
            uint64_t                               m_resv_addr;           // This is always a physical address
            bool                                   m_resv_set;            // Flag for setting resv
            uint8_t                                m_resv_size;           // Resv. size
            static std::map<uint64_t,
                std::pair<bool, uint8_t> >         sm_resv_map;           // shared reservation map
        
            // powerPC register file
            ppc_regs                               m_cpu_regs;            // PPC register file
        #define PPCSIMBOOKE_CPU_REG(reg_id)                     (m_cpu_regs.m_ireg[reg_id]->value.u64v)
        #define PPCSIMBOOKE_CPU_REGN(reg_name)                  (m_cpu_regs.m_reg[reg_name]->value.u64v)
        #define PPCSIMBOOKE_CPU_REGATTR(reg_id)                 (m_cpu_regs.m_ireg[reg_id]->attr)
        #define PPCSIMBOOKE_CPU_REGNATTR(reg_name)              (m_cpu_regs.m_reg_attr[reg_name]->attr)
        #define PPCSIMBOOKE_CPU_REGMASK(reg_id, mask)           EBF(PPCSIMBOOKE_CPU_REG(reg_id),    mask)
        #define PPCSIMBOOKE_CPU_REGNMASK(reg_name, mask)        EBF(PPCSIMBOOKE_CPU_REGN(reg_name), mask)
        #define PPCSIMBOOKE_CPU_PC                              (m_cpu_regs.pc)
        #define PPCSIMBOOKE_CPU_NIP                             (m_cpu_regs.nip)
        
            // Book keeping
            uint64_t                               m_cpu_id;              // A unique cpu id
            uint8_t                                m_cpu_no;              // Cpu no
            static size_t                          sm_ncpus;              // Total cpus
            size_t                                 m_ninstrs;             // Number of instrs (total)
            size_t                                 m_ninstrs_last;        // Number of instrs in last run
            size_t                                 m_ncycles;             // number of cycles
            instr_call                             m_instr_this;          // Current instr
            instr_call                             m_instr_next;          // next instr
        
            // Logging facilities
            cpu_coverage                           m_cov_logger;          // coverage logger
        
            ppcsimbooke_dis::ppcdis                m_dis;                  // Disassembler module
            ppcsimbooke_tlb::tlb                   m_l2tlb;                // tlb4K_ns = 128, tlb4K_nw = 4, tlbCam_ne = 16
            ppcsimbooke_memory::memory             *m_mem_ptr;             // Memory module
            
            
            // Host flags
            x86_flags                              host_flags;            // x86 flags register
            x86_mxcsr                              m_x86_mxcsr;           // mxcsr flags register
        
            // A Cache of recently called instrs.
            lru_cache<uint64_t, instr_call>        m_instr_cache;     // Cache of recently used instrs
            bool                                   m_ctxt_switch;     // Flag indicating that a ctxt switch happened

            // opcode to function pointer map
            std::map<uint64_t, ppc_opc_fun_ptr>    m_ppc_func_hash;
        
            // timings
            boost::posix_time::ptime               m_prev_stamp;
            boost::posix_time::ptime               m_next_stamp;
        
        };
    }
}

#endif    /*  CPU_PPC_H  */
