//  tlb_booke.cpp 
//  This file contains a PPC booke (based on Freescale e500v2 MMU) tlb emulation model
//
//  Copyright 2012.
//  Author : Vikas Chouhan (presentisgood@gmail.com)
//
//  This file is part of ppc-sim.
//
//  This library is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 2 as
//  published by the Free Software Foundation.
//
//  It is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this file; see the file COPYING.  If not, write to the
//  Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
//  MA 02110-1301, USA.

#ifndef TLB_BOOKE_H_
#define TLB_BOOKE_H_

#include "config.h"
#include "utils.h"
#include "third_party/lru/lru.hpp"

// Derive an interim 45 bit virtual address from MSR[PR], RWX permissions,
//                                               MSR[IS/DS], PID[0/1/2] & EA
#define  TO_VIRT(pr, rwx, as, pid, ea)    ((((pr) & 0x1) << 44)     |   \
                                           (((rwx) & 0x7)<< 41)     |   \
                                           (((as) & 0x1) << 40)     |   \
                                           (((pid) & 0xff) << 32)   |   \
                                           ((ea) & 0xffffffff))

// Macros for easy accessibility
#define TLB_T
#define TLB_PPC                    ppc_tlb
#define TLB_PPC_T                  TLB_PPC

class TLB_PPC {

    public:
    // Tlb entry
    struct t_tlb_entry {
        uint32_t  tid;     // PID
        uint64_t  epn;     // Effective page no     --------
        uint64_t  rpn;     // Real Page no          -------- Both page numbers are absolute page numbers
        uint64_t  ea;      // effective address     --------
        uint64_t  ra;      // real address          -------- Used for quick tlb search
        uint64_t  ps;      // page size
        uint32_t  wimge;   // wimge attributes
        uint32_t  x01;     // x0-x1 page attribute
        uint32_t  u03;     // u0-u3 user defined attributes
        uint32_t  permis;  // Permission . This is stored in
                           // a diff format than that specified by MAS registers
                           // ur-uw-ux-sr-sw-sx
#define PERMIS_TO_PPCPERMIS(_perm)                                                  \
        (                                                                           \
        ((_perm & 0x1) << 4) | ((_perm & 0x2) << 1)  | ((_perm & 0x4) >> 2) |       \
        ((_perm & 0x8) << 2) | ((_perm & 0x10) >> 1) | ((_perm & 0x20) >> 4)        \
        )
#define PPCPERMIS_TO_PERMIS(_perm)                                                  \
        (                                                                           \
        ((_perm & 0x1) << 2) | ((_perm & 0x2) << 4)  | ((_perm & 0x4) >> 1) |       \
        ((_perm & 0x8) << 1) | ((_perm & 0x10) >> 4) | ((_perm & 0x20) >> 2)        \
        )
        struct {
            uint64_t ts    : 1;  /* Translation space */
            uint64_t tsize : 4;  /* Logarithmic page size */
            uint64_t iprot : 1;  /* Invalidate protect */
            uint64_t valid : 1;  /* Valid ? */
        } tflags;

    };

    // Tlb set
    template<int nw> struct t_tlb_set {
        t_tlb_entry         tlb_way[nw];   /* Vector of TLB entries */
        const size_t        n_ways;    /* Number of ways */

        // Constructor
        t_tlb_set() : n_ways(nw) {}
    };

    // Tlb array
    // ns -> number of sets
    // nw -> number of ways
    // serial number of this tlb
    template<int ns, int nw, int sn> struct t_tlb {
        const uint32_t        tlbno;         /* tlb no */
        const uint32_t        assoc;         /* Associativity level */
        const size_t          n_sets;        /* Number of sets */
        t_tlb_set<nw>         tlb_set[ns];   /* Array of sets */

        // Constructor
        t_tlb() : tlbno(sn), assoc(nw), n_sets(ns) {}
    };


    private:
    t_tlb<CPU_TLB4K_N_SETS, CPU_TLB4K_N_WAYS, 0>       tlb4K;       // tlb4K_ns sets, tlb4K_nw ways
    t_tlb<1, CPU_TLBCAM_N_ENTRIES, 1>                  tlbCam;      // 1 set,         tlbCam_nw ways

    // NOTE :
    //        1. m_tlb_cache stores translation tables in cached form for faster access.
    //           Once a tlb entry is found in the main table (either in tlb4K or tlbCam),
    //           it's cached for later access.
    //        2. sm_pgmask_list is the list of all supported page masks. This is the list
    //           used to derive page va for all possible page sizes. The generated va is
    //           then used as a hash key in step 1.
    //        3. m_need_flush is flag which signifies if the cache has to be flushed. This
    //           type of need arises, when tlb array is changed (for eg. when a new tlb
    //           entry is written or some entry is invalidated etc.)
    lru_cache<uint64_t, uint64_t, uint32_t>     m_tlb_cache;          // [virtual_address] -> real_address cache
    static const uint64_t                       sm_pgmask_list[];     // page_mask list for all supported pages.
    bool                                        m_need_flush;         // m_tlb_cache needs flushing
    
    private:
    void init_ppc_tlb_defaults(void);
    void print_tlb_entry(t_tlb_entry &entry, std::string fmtstr="    ");
    t_tlb_entry& get_entry(size_t tlbsel, uint64_t ea, size_t wayno);
    t_tlb_entry& get_entry2(size_t tlbsel, size_t setno, size_t wayno);

    public:
    TLB_PPC();
    ~TLB_PPC();

    void print_tlbs();
    void print_tlbs2();
    
    void tlbre(uint64_t &mas0, uint64_t &mas1, uint64_t &mas2, uint64_t &mas3, uint64_t &mas7, uint64_t hid0);
    void tlbwe(uint64_t mas0, uint64_t mas1, uint64_t mas2, uint64_t mas3, uint64_t mas7, uint64_t hid0);
    void tlbse(uint64_t ea, uint64_t &mas0, uint64_t &mas1, uint64_t &mas2, uint64_t &mas3, uint64_t &mas6, uint64_t &mas7, uint64_t hid0);
    void tlbive(uint64_t ea);
    std::pair<uint64_t, uint8_t> xlate(uint64_t ea, bool as, uint8_t pid, uint8_t rwx, bool pr);

};

#endif

/* Initialize TLB like this
 * l2 tlb for e500v2
 * Eg:-
 * TLB_PPC_T<128,4,16>  tlbl2;
 */
