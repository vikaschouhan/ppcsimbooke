//  tlb_booke.cpp 
//  This file contains a PPC booke ( preferably Freescale e500v2 core ) tlb emulation model
//
//  Copyright 2012.
//  Author : Vikas Chouhan ( presentisgood@gmail.com )
//
//  This file is part of ppc-sim.
//
//  This library is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3, or (at your option)
//  any later version.
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
#define TLB_T                      template<int x, int y, int z>
#define TLB_PPC                    ppc_tlb
#define TLB_PPC_T                  TLB_PPC<x, y, z>

/* A booke tlb emulation */
// Template parameters
// tlb4K_ns -> number of sets in tlb4K
// tlb4K_nw -> number of ways per set in tlb4K
// tlbCam_ne -> number of entries in tlbCam
template<int tlb4K_ns, int tlb4K_nw, int tlbCam_ne> class TLB_PPC {

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
    t_tlb<tlb4K_ns,  tlb4K_nw, 0>       tlb4K;       // tlb4K_ns sets, tlb4K_nw ways
    t_tlb<       1, tlbCam_ne, 1>       tlbCam;      // 1 set,         tlbCam_nw ways

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

// All static members.

// Look up table for page masks
TLB_T const uint64_t TLB_PPC_T::sm_pgmask_list[] = {
                                                ~(0x0000000000001000ULL - 1),    // TID=1,   4K
                                                ~(0x0000000000004000ULL - 1),    // TID=2,   16K
                                                ~(0x0000000000010000ULL - 1),    // TID=3,   64K
                                                ~(0x0000000000040000ULL - 1),    // TID=4,   256K
                                                ~(0x0000000000100000ULL - 1),    // TID=5,   1M
                                                ~(0x0000000000400000ULL - 1),    // TID=6,   4M
                                                ~(0x0000000001000000ULL - 1),    // TID=7,   16M
                                                ~(0x0000000004000000ULL - 1),    // TID=8,   64M
                                                ~(0x0000000010000000ULL - 1),    // TID=9,   256M
                                                ~(0x0000000040000000ULL - 1),    // TID=10,  1G
                                                ~(0x0000000100000000ULL - 1),    // TID=11,  4G
                                            };

// Check if valid page number ( based on tsize )
#define CHK_VALID_PN(pn, tsize)  ((((pow4(tsize) * 0x400) - 1) & pn) == pn)
#define TSIZE_TO_PSIZE(tsize)    (pow4(tsize) * 0x400)


// Member functions

/* Initialize default values on per entry basis for all tlb arrays */
TLB_T void TLB_PPC_T::init_ppc_tlb_defaults(void){
    LOG("DEBUG4") << MSG_FUNC_START;

    // tlb4K
    for(size_t j=0; j<tlb4K.n_sets; j++){
        for(size_t k=0; k<tlb4K.tlb_set[j].n_ways; k++){
            tlb4K.tlb_set[j].tlb_way[k].ps = 0x1000;       /* default ps for this tlb array */
            tlb4K.tlb_set[j].tlb_way[k].tflags.valid = 0;  /* Disable all entries */
        }
    }

    // tlbCam
    for(size_t j=0; j<tlbCam.n_sets; j++){
        for(size_t k=0; k<tlbCam.tlb_set[j].n_ways; k++){
            tlbCam.tlb_set[j].tlb_way[k].tflags.valid = 0;  /* Disable all entries */
        }
    }

    /* Initialize one default tlb entry in tlb1 for the reset vector */
    t_tlb_entry &entry = tlbCam.tlb_set[0].tlb_way[0];

    /* Entry for boot page */
    entry.tflags.valid = 1;
    entry.tflags.ts    = 0;
    entry.tid          = 0;
    entry.epn          = 0xfffff;
    entry.rpn          = 0xfffff;
    entry.ea           = 0xfffff000;
    entry.ra           = 0xfffff000;
    entry.tflags.tsize = 1;                   // 4K page size
    entry.ps           = 0x1000;
    entry.permis       = 0b000111;            // sr-sw-sx = 111, ur-uw-ux = 000
    entry.wimge        = 0b01000;             // cahce inhibited
    entry.x01          = 0;
    entry.u03          = 0;
    entry.tflags.iprot = 1;

    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : print_tlb_entry
 * @args : entry -> pointer to tlb entry
 *         fmtstr -> format string ( number of spaces for formatting )
 *
 * @brief : print one tlb entry at a time.
 */ 
TLB_T void TLB_PPC_T::print_tlb_entry(t_tlb_entry &entry, std::string fmtstr){
    LOG("DEBUG4") << MSG_FUNC_START;

    std::cout << fmtstr << "tid    -> " << std::hex << entry.tid << std::endl;
    std::cout << fmtstr << "ea     -> " << std::hex << entry.ea  << std::endl;
    std::cout << fmtstr << "ra     -> " << std::hex << entry.ra  << std::endl;
    std::cout << fmtstr << "ps     -> " << std::hex << entry.ps  << std::endl;
    std::cout << fmtstr << "wimge  -> " << std::hex << entry.wimge << std::endl;
    std::cout << fmtstr << "permis -> " << std::hex << PERMIS_TO_PPCPERMIS(entry.permis) << std::endl;
    std::cout << fmtstr << "x01    -> " << std::hex << entry.x01 << std::endl;
    std::cout << fmtstr << "u03    -> " << std::hex << entry.u03 << std::endl;
    std::cout << fmtstr << "ts     -> " << std::hex << entry.tflags.ts << std::endl;
    std::cout << fmtstr << "tsize  -> " << std::hex << entry.tflags.tsize << std::endl;
    std::cout << fmtstr << "iprot  -> " << std::hex << entry.tflags.iprot << " (please ignore, if not tlbCam)" << std::endl;
    std::cout << fmtstr << "valid  -> " << std::hex << entry.tflags.valid << std::endl;

    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get tlb entry when tlbno, epn and esel are specified.
TLB_T typename TLB_PPC_T::t_tlb_entry& TLB_PPC_T::get_entry(size_t tlbno, uint64_t ea, size_t esel){
    LOG("DEBUG4") << MSG_FUNC_START;

    unsigned setno = 0;
    unsigned wayno = 0;

    /* Wrong tlbsel */
    LASSERT_THROW(tlbno == 0 || tlbno == 1, sim_except(SIM_EXCEPT_EINVAL, "invalid tlbsel ( MAS0 )"), DEBUG4);
   
    // For fully associative arrays,  esel selects the set_no and epn is not used
    // but for n way set-associative, esel selects way_no and EPN[45-51] selects the desired set_no
    if(tlbno == 0){
        setno = (ea >> MIN_PGSZ_SHIFT) & (tlb4K.assoc - 1);
    }
    wayno = esel;

    if(tlbno == 0){
        LASSERT_THROW(setno < tlb4K.n_sets, sim_except(SIM_EXCEPT_EINVAL, "Wrong set no ( MAS2 )"), DEBUG4);
        LASSERT_THROW(wayno < tlb4K.tlb_set[setno].n_ways, sim_except(SIM_EXCEPT_EINVAL, "Wrong way no ( MAS0 )"), DEBUG4);
        return tlb4K.tlb_set[setno].tlb_way[wayno];
    }else{
        LASSERT_THROW(setno < tlbCam.n_sets, sim_except(SIM_EXCEPT_EINVAL, "Wrong set no ( MAS2 )"), DEBUG4);
        LASSERT_THROW(wayno < tlbCam.tlb_set[setno].n_ways, sim_except(SIM_EXCEPT_EINVAL, "Wrong way no ( MAS0 )"), DEBUG4);
        return tlbCam.tlb_set[setno].tlb_way[wayno];
    }

    LOG("DEBUG4") << MSG_FUNC_END;
}

// Get tlb entry when tlbno, setno and wayno are specified.
TLB_T typename TLB_PPC_T::t_tlb_entry& TLB_PPC_T::get_entry2(size_t tlbno, size_t setno, size_t wayno){
    LOG("DEBUG4") << MSG_FUNC_START;

    /* Wrong tlbsel */
    LASSERT_THROW(tlbno == 0 || tlbno == 1, sim_except(SIM_EXCEPT_EINVAL, "invalid tlbsel ( MAS0 )"), DEBUG4);
   
    if(tlbno == 0){
        LASSERT_THROW(setno < tlb4K.n_sets, sim_except(SIM_EXCEPT_EINVAL, "Wrong set no ( MAS2 )"), DEBUG4);
        LASSERT_THROW(wayno < tlb4K.tlb_set[setno].n_ways, sim_except(SIM_EXCEPT_EINVAL, "Wrong way no ( MAS0 )"), DEBUG4);
        return tlb4K.tlb_set[setno].tlb_way[wayno];
    }else{
        LASSERT_THROW(setno < tlbCam.n_sets, sim_except(SIM_EXCEPT_EINVAL, "Wrong set no ( MAS2 )"), DEBUG4);
        LASSERT_THROW(wayno < tlbCam.tlb_set[setno].n_ways, sim_except(SIM_EXCEPT_EINVAL, "Wrong way no ( MAS0 )"), DEBUG4);
        return tlbCam.tlb_set[setno].tlb_way[wayno];
    }

    LOG("DEBUG4") << MSG_FUNC_END;
}

// Constructor
TLB_T TLB_PPC_T::TLB_PPC(){
    LOG("DEBUG4") << MSG_FUNC_START;
    /* Initialize default tlb parameters , based on passed information */
    init_ppc_tlb_defaults();
    m_tlb_cache.set_size(128);
    LOG("DEBUG4") << MSG_FUNC_END;
}

/* Destructor */
TLB_T TLB_PPC_T::~TLB_PPC(){
    LOG("DEBUG4") << MSG_FUNC_START;
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : print_tlbs()
 * @args : none
 *
 * @brief : print all tlb entries in a nice form
 */
TLB_T void TLB_PPC_T::print_tlbs(){
    LOG("DEBUG4") << MSG_FUNC_START;

#define PRINT_TLB(tlb_type)                                                                            \
    std::cout << #tlb_type << std::endl;                                                               \
    std::cout << BAR0 << std::endl;                                                                    \
    std::cout << "    assoc -> " << std::hex << tlb_type.assoc << std::endl;                           \
    std::cout << "    no    -> " << std::hex << tlb_type.tlbno << std::endl;                           \
    for(size_t j=0; j<tlb_type.n_sets; j++){                                                           \
        for(size_t k=0; k<tlb_type.tlb_set[j].n_ways; k++){                                            \
            t_tlb_entry &entry_this = tlb_type.tlb_set[j].tlb_way[k];                                  \
            if(entry_this.tflags.valid){           /* print only if the entry is valid */              \
                std::cout << "        set " << j << " way " << k << std::endl;                         \
                print_tlb_entry(entry_this, "            ");                                           \
            }                                                                                          \
        }                                                                                              \
    }                                                                                                  \
    std::cout << std::endl

    PRINT_TLB(tlb4K);
    PRINT_TLB(tlbCam);

    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : print_tlbs()
 * @args : none
 *
 * @brief : print all tlb entries in a nice form
 */
TLB_T void TLB_PPC_T::print_tlbs2(){
    LOG("DEBUG4") << MSG_FUNC_START;

#define PRINT_TLB_ENT(set, way, ea, ra, epn, rpn, tid, ts, wimge, permis, ps, x01, u03, iprot)                            \
    std::cout << std::showbase;                                                                                           \
    std::cout << std::setw(4) << std::right << set << std::setw(6) << std::right << way                                   \
         << std::setw(16) << std::right << std::hex << ea << std::setw(16) << std::right << std::hex << ra                \
         << std::setw(16) << std::right << std::hex << epn << std::setw(16) << std::right << std::hex << rpn              \
         << std::setw(6) << std::right << tid << std::setw(4) << std::right << ts                                         \
         << std::setw(6) << std::right << std::hex << wimge << std::setw(9) << std::right << std::hex << permis           \
         << std::setw(16) << std::right << std::hex << ps << std::setw(6) << std::right << std::hex << x01                \
         << std::setw(6) << std::right << std::hex << u03 << std::setw(6) << std::right << iprot << std::endl

#define PRINT_TLB2(tlb_type)                                                                                                            \
    std::cout << #tlb_type << std::endl;                                                                                                \
    PRINT_TLB_ENT("set", "way", "ea", "ra", "epn", "rpn", "tid", "ts", "wimge", "permis", "page size", "x01", "u03", "iprot");          \
    for(size_t j=0; j<tlb_type.n_sets; j++){                                                                                            \
        for(size_t k=0; k<tlb_type.tlb_set[j].n_ways; k++){                                                                             \
            t_tlb_entry &entry_this = tlb_type.tlb_set[j].tlb_way[k];                                                                   \
            if(entry_this.tflags.valid){           /* print only if the entry is valid */                                               \
                PRINT_TLB_ENT(j, k, entry_this.ea, entry_this.ra,                                                                       \
                        entry_this.epn,    entry_this.rpn,                                                                              \
                        entry_this.tid,    entry_this.tflags.ts,                                                                        \
                        entry_this.wimge,  PERMIS_TO_PPCPERMIS(entry_this.permis),                                                      \
                        entry_this.ps,     entry_this.x01,                                                                              \
                        entry_this.u03,    entry_this.tflags.iprot);                                                                    \
            }                                                                                                                           \
        }                                                                                                                               \
    }                                                                                                                                   \
    std::cout << std::endl

    PRINT_TLB2(tlb4K);
    PRINT_TLB2(tlbCam);

    LOG("DEBUG4") << MSG_FUNC_END;
}

/* 
 * @func : tlbre (tlb readentry)
 * @args :
 *         mas0, mas1, mas2, mas3, mas7 -> MAS registers
 *         hid0 -> HID0 register
 *
 * @brief : Reads TLB entry matching the information passed in MAS registers
 *
 */
TLB_T void TLB_PPC_T::tlbre(uint64_t &mas0, uint64_t &mas1, uint64_t &mas2, uint64_t &mas3, uint64_t &mas7, uint64_t hid0){
    LOG("DEBUG4") << MSG_FUNC_START;

    unsigned tlbsel = EBF(mas0,       MAS0_TLBSEL);
    unsigned esel   = EBF(mas0,       MAS0_ESEL);
    uint64_t epn    = EBF(mas2,       MAS2_EPN);
    //uint64_t rpn    = EBF(mas3,       MAS3_RPN);
    //unsigned tsize  = EBF(mas1,       MAS1_TSIZE);

    // check validity of page numbers
    //LASSERT_THROW(CHK_VALID_PN(epn, tsize), sim_except(SIM_EXCEPT_EINVAL, "Illegal MAS2[EPN] or MAS1[TSIZE]."), DEBUG4);
    //LASSERT_THROW(CHK_VALID_PN(rpn, tsize), sim_except(SIM_EXCEPT_EINVAL, "Illegal MAS3[RPN] or MAS1[TSIZE]."), DEBUG4); 

    t_tlb_entry &entry = get_entry(tlbsel, epn << MIN_PGSZ_SHIFT, esel);

    /* FIXME: Does the spec. says anything about nv when used with tlbre */
    mas0  = IBF(mas0, 0,                                     MAS0_NV);

    mas1  = IBF(mas1, entry.tflags.valid,                    MAS1_V);
    mas1  = IBF(mas1, entry.tflags.iprot,                    MAS1_IPROT);
    mas1  = IBF(mas1, entry.tid,                             MAS1_TID);
    mas1  = IBF(mas1, entry.tflags.ts,                       MAS1_TS);
    mas1  = IBF(mas1, entry.tflags.tsize,                    MAS1_TSIZE);
   
    mas2  = IBF(mas2, (entry.ea >> MIN_PGSZ_SHIFT),          MAS2_EPN);
    mas2  = IBF(mas2, entry.x01,                             MAS2_X01);
    mas2  = IBF(mas2, entry.wimge,                           MAS2_WIMGE);

    mas3  = IBF(mas3, (entry.ra >> MIN_PGSZ_SHIFT),          MAS3_RPN);
    mas3  = IBF(mas3, entry.u03,                             MAS3_U03);
    mas3  = IBF(mas3, PERMIS_TO_PPCPERMIS(entry.permis),     MAS3_PERMIS);

    if(EBF(hid0, HID0_EN_MAS7_UPDATE))
        mas7  = IBF(mas7, (entry.ra >> 32) & 0xf, MAS7_RPN);    /* Upper 4 bits of rpn */

    LOG("DEBUG4") << MSG_FUNC_END;
}

/* @func : tlbwe (Tlb write entry)
 * @args :
 *         mas0, mas1, mas2, mas3, mas7 -> MAS registers.
 *         hid0 -> HID0 register
 */
TLB_T void TLB_PPC_T::tlbwe(uint64_t mas0, uint64_t mas1, uint64_t mas2, uint64_t mas3, uint64_t mas7, uint64_t hid0){
    LOG("DEBUG4") << MSG_FUNC_START;

    unsigned tlbsel = EBF(mas0,    MAS0_TLBSEL);
    unsigned esel   = EBF(mas0,    MAS0_ESEL);
    uint64_t epn    = EBF(mas2,    MAS2_EPN);
    uint64_t rpn    = EBF(mas3,    MAS3_RPN);      // We only require lower 20 bits of rpn for validity check
    unsigned tsize  = EBF(mas1,    MAS1_TSIZE);
    uint64_t psize  = TSIZE_TO_PSIZE(tsize);          // Get absolute page size

    // check validity of page numbers
    //LASSERT_THROW(CHK_VALID_PN(epn, tsize), sim_except(SIM_EXCEPT_EINVAL, "Illegal MAS2[EPN] or MAS1[TSIZE]."), DEBUG4);
    //LASSERT_THROW(CHK_VALID_PN(rpn, tsize), sim_except(SIM_EXCEPT_EINVAL, "Illegal MAS3[RPN] or MAS1[TSIZE]."), DEBUG4);

    t_tlb_entry &entry = get_entry(tlbsel, epn << MIN_PGSZ_SHIFT, esel);

    /*FIXME : Need to check, if valid bit is passed as part of MAS1 or set implicitly */
    entry.tflags.valid = EBF(mas1,   MAS1_V);
    entry.tflags.iprot = EBF(mas1,   MAS1_IPROT); 
    entry.tid          = EBF(mas1,   MAS1_TID);
    entry.tflags.ts    = EBF(mas1,   MAS1_TS);
    entry.tflags.tsize = tsize;
   
    entry.ea           = epn << MIN_PGSZ_SHIFT;
    entry.epn          = entry.ea >> rshift<uint64_t>(psize);
    entry.x01          = EBF(mas2,   MAS2_X01);
    entry.wimge        = EBF(mas2,   MAS2_WIMGE);

    entry.ra           = rpn << MIN_PGSZ_SHIFT;
    entry.rpn          = entry.ra >> rshift<uint64_t>(psize);
    entry.u03          = EBF(mas3,   MAS3_U03);
    entry.permis       = PPCPERMIS_TO_PERMIS(EBF(mas3,   MAS3_PERMIS));

    if(EBF(hid0, HID0_EN_MAS7_UPDATE)){
        entry.ra |= EBF(mas7, MAS7_RPN) << 32;    /* Upper 4 bits of rpn */
        entry.rpn = entry.ra >> rshift<uint64_t>(psize);
    }

    entry.ps           = psize;

    LOG("DEBUG4") << MSG_FUNC_END;
}

/* @func : tlbse (Tlb Search entry)
 * @args :
 *         ea -> effective address to be searched for ( according to e500v2, EA[32-51] is used for the search )
 *         mas0, mas1, mas2, mas3, mas6, mas7 -> MAS registers
 *         hid0 -> hid register
 *
 * @brief : This function searches for a tlb entry matching the passed effective address , TS = MAS6[SAS] and TID = MAS6[SPID0]
 *
 */
TLB_T void TLB_PPC_T::tlbse(uint64_t ea, uint64_t &mas0, uint64_t &mas1, uint64_t &mas2, uint64_t &mas3, uint64_t &mas6, uint64_t &mas7, uint64_t hid0){
    LOG("DEBUG4") << MSG_FUNC_START;

    uint32_t  as  = EBF(mas6,  MAS6_SAS);
    uint32_t pid  = EBF(mas6,  MAS6_SPID0);
    size_t   tsel = -1;
    //size_t   ssel = -1;
    size_t   wsel = -1;
    t_tlb_entry *entry = NULL;

    /* Start searching in the tlb arrays */
#define SEARCH_TLB(tlb_type)                                                                                \
    for(size_t j=0; j<tlb_type.n_sets; j++){                                                                \
        for(size_t k=0; k<tlb_type.tlb_set[j].n_ways; k++){                                                 \
            if(((ea & ~(tlb_type.tlb_set[j].tlb_way[k].ps - 1)) == tlb_type.tlb_set[j].tlb_way[k].ea) &&    \
               (as  == tlb_type.tlb_set[j].tlb_way[k].tflags.ts)  &&                                        \
               (pid == tlb_type.tlb_set[j].tlb_way[k].tid)){                                                \
                entry = &(tlb_type.tlb_set[j].tlb_way[k]);                                                  \
                tsel = tlb_type.tlbno;                                                                      \
                /*ssel = j;*/                                                                               \
                wsel = k;                                                                                   \
                goto exit_loop_0;                                                                           \
            }                                                                                               \
        }                                                                                                   \
    }

    SEARCH_TLB(tlb4K);
    SEARCH_TLB(tlbCam);

    exit_loop_0:
    /* Valid bit set means, search was successful */
    mas1  = IBF(mas1, (entry)?1:0, MAS1_V);

    /* Now return if the search was unsuccessful */
    if(entry == NULL){
        RETURNVOID(DEBUG4);
    }

    mas0  = IBF(mas0, tsel,                                 MAS0_TLBSEL);
    mas0  = IBF(mas0, wsel,                                 MAS0_ESEL);
    /* FIXME : need to check the behaviour of MAS0[NV] on tlbsx */
    mas0  = IBF(mas0, 0,                                    MAS0_NV);

    mas1  = IBF(mas1, entry->tflags.iprot,                  MAS1_IPROT);
    mas1  = IBF(mas1, entry->tid,                           MAS1_TID);
    mas1  = IBF(mas1, entry->tflags.ts,                     MAS1_TS);
    mas1  = IBF(mas1, entry->tflags.tsize,                  MAS1_TSIZE);
   
    mas2  = IBF(mas2, entry->ea >> MIN_PGSZ_SHIFT,          MAS2_EPN);
    mas2  = IBF(mas2, entry->x01,                           MAS2_EPN);
    mas2  = IBF(mas2, entry->wimge,                         MAS2_WIMGE);

    mas3  = IBF(mas3, entry->ra >> MIN_PGSZ_SHIFT,          MAS3_RPN);
    mas3  = IBF(mas3, entry->u03,                           MAS3_U03);
    mas3  = IBF(mas3, PERMIS_TO_PPCPERMIS(entry->permis),   MAS3_PERMIS);

    if(EBF(hid0, HID0_EN_MAS7_UPDATE))
        mas7 = IBF(mas7, (entry->ra >> 32) & 0xf,  MAS7_RPN);    /* Upper 4 bits of rpn */

    LOG("DEBUG4") << MSG_FUNC_END;
}

/* @func : tlbive (TLB INValidate entry)
 * @args :
 *        ea -> effective address.
 *
 * @brief : This function will invalidate all tlb entries matching with a paricular effective address
 *        NOTE: 
 *        for e500v2 , entry to be invalidated is decided based on various fields of EA ( mentioned below )
 *          EA[32-51] -> epn for invalidation
 *          EA[60]    -> tlbsel, to select the tlb array no
 *          EA[61]    -> INV_ALL, to invalidate all entries of the tlb array selected by 'tlbsel'
 *                       ( if this entry is in tlb1 and IPROT is not set )
 *
 *        An Invalidate Instruction is broadcase throughout the coherency domain of the current processor.
 *
 * */
TLB_T void TLB_PPC_T::tlbive(uint64_t ea){
    LOG("DEBUG4") << MSG_FUNC_START;

    uint8_t  tlbsel = (ea >> 3) & 0x1;
    uint8_t  inv_all = (ea >> 2) & 0x1;
    t_tlb_entry *entry = NULL;

    /* Test if INV_ALL is set and tlbno = 1 */
    if(inv_all && (tlbsel == 1)){
        for(size_t j=0; j<tlbCam.n_sets; j++){
            for(size_t k=0; k<tlbCam.tlb_set[j].n_ways; k++){
                entry = &(tlbCam.tlb_set[j].tlb_way[k]);
                if(!entry->tflags.iprot){
                    entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
                }
            }
        }
    }else /* Test if inv_all and tlbno = 1 */
    if(inv_all && (tlbsel == 0)){
        for(size_t j=0; j<tlb4K.n_sets; j++){
            for(size_t k=0; k<tlb4K.tlb_set[j].n_ways; k++){
                entry = &(tlb4K.tlb_set[j].tlb_way[k]);
                entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
            }
        }
    }else /* Test for other cases */
    if(!inv_all)
    {
        // tlb4K
        for(size_t j=0; j<tlb4K.n_sets; j++){
            for(size_t k=0; k<tlb4K.tlb_set[j].n_ways; k++){
                entry = &(tlb4K.tlb_set[j].tlb_way[k]);
                if(entry->ea == (ea & ~(entry->ps - 1))){
                    entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
                    goto exit_loop_1;
                }
            }
        }

        // tlbCam
        for(size_t j=0; j<tlbCam.n_sets; j++){
            for(size_t k=0; k<tlbCam.tlb_set[j].n_ways; k++){
                entry = &(tlbCam.tlb_set[j].tlb_way[k]);
                if((entry->ea == (ea & ~(entry->ps - 1))) && !entry->tflags.iprot){
                    entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
                    goto exit_loop_1;
                }
            }
        }

    }else
    {
        /* Whatever ?? */
    }
    exit_loop_1:
    ;

    LOG("DEBUG4") << MSG_FUNC_END;
}

// Translate effective address into real address using as bit, an 8 bit PID value and permission attributes
TLB_T std::pair<uint64_t, uint8_t> TLB_PPC_T::xlate(uint64_t ea, bool as, uint8_t pid, uint8_t rwx, bool pr){
    LOG("DEBUG4") << MSG_FUNC_START;

    uint64_t offset;
    uint8_t wimge;
    t_tlb_entry* entry = NULL;
    uint64_t va;
    uint64_t ra;

    // Check validity of AS and PID
    LASSERT_THROW((as & 0x1)   == as,  sim_except(SIM_EXCEPT_EINVAL, "Illegal AS"), DEBUG4);
    LASSERT_THROW((pid & 0xff) == pid, sim_except(SIM_EXCEPT_EINVAL, "Illegal PID"), DEBUG4);
    LASSERT_THROW((rwx & 0x7)  == rwx, sim_except(SIM_EXCEPT_EINVAL, "Illegal permis rwx"), DEBUG4);

    uint16_t perm = (rwx << (pr*3));

    static int size_pgm = sizeof(sm_pgmask_list);

    // start searching for the tlb entry in cache first
    for(int indx=0; indx < size_pgm; indx++){
        // get va & offset
        va     = TO_VIRT(u64(pr), u64(rwx), u64(as), u64(pid), (ea & sm_pgmask_list[indx]));
        
        ra    = m_tlb_cache[va];
        offset = ea & ~sm_pgmask_list[indx];
        wimge = m_tlb_cache.info_at(va);
        
        if(m_tlb_cache.error()){
            continue;
        }

        return std::pair<uint64_t, uint8_t>((ra + offset), wimge);
    }

    // According to e500v2 CRM , multiple hits are considered programming error and the xlated
    // address may not be valid. An exception ( hardware ) is not generated in this case.
#define FIND_TLB_MATCH(tlb_type)                                                                                      \
    for(size_t j=0; j<tlb_type.n_sets; j++){                                                                          \
        for(size_t k=0; k<tlb_type.tlb_set[j].n_ways; k++){                                                           \
            entry = &(tlb_type.tlb_set[j].tlb_way[k]);                                                                \
            if(entry->tflags.valid && (entry->ea == (ea & ~(entry->ps - 1))) && entry->tid == pid                     \
                    && entry->tflags.ts == as){                                                                       \
                if((entry->permis & perm) == perm){                                                                   \
                    goto exit_loop_1;                                                                                 \
                }else{                                                                                                \
                    if(rwx & 0x1){                                                                                    \
                        std::cout << "Got ISI exception" <<std::endl;                                                 \
                        throw sim_except_ppc(PPC_EXCEPTION_ISI, PPC_EXCEPT_ISI_ACS, "ISI exception.");                \
                    }else if(rwx & 0x2){                                                                              \
                        std::cout << "Got DSI write exception" << std::endl;                                          \
                        throw sim_except_ppc(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_ACS_W, "DSI write exception.");        \
                    }else if(rwx & 0x4){                                                                              \
                        std::cout << "Got DSI read exception" << std::endl;                                           \
                        throw sim_except_ppc(PPC_EXCEPTION_DSI, PPC_EXCEPT_DSI_ACS_R, "DSI read exception.");         \
                    }                                                                                                 \
                }                                                                                                     \
            }                                                                                                         \
        }                                                                                                             \
    }

    FIND_TLB_MATCH(tlb4K)
    FIND_TLB_MATCH(tlbCam)

    LOG("DEBUG4") << MSG_FUNC_END;
    // Instead of throwing TLB miss exception from here itself we are returning a value of -1
    // The TLB miss exception will be thrown in the caller after it tries to get a hit with
    // three different PID registers ( viz. PID0, PID1 and PID2 ) and still fails.
    return std::pair<uint64_t, uint8_t>(-1, -1);

    exit_loop_1:

    // Get offset and wimge
    offset = ea & (TSIZE_TO_PSIZE(entry->tflags.tsize) - 1);
    wimge  = entry->wimge;

    // Update the cache
    m_tlb_cache.insert(TO_VIRT(u64(pr), u64(rwx), u64(as), u64(pid), (ea & ~(entry->ps - 1))), entry->ra, entry->wimge); 

    LOG("DEBUG4") << MSG_FUNC_END;
    return std::pair<uint64_t, uint8_t>((entry->ra + offset), wimge);
}

/* Initialize TLB like this
 * l2 tlb for e500v2
 * Eg:-
 * TLB_PPC_T<128,4,16>  tlbl2;
 */
