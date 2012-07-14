/* tlb_booke.cpp 
   This file contains a PPC booke ( preferably Freescale e500v2 core ) tlb emulation model

   Copyright 2012.
   Written by Vikas Chouhan ( presentisgood@gmail.com )

   This file is part of ppc-sim.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "config.h"
#include "utils.h"

using std::vector;
using std::list;
using std::string;
using std::cout;
using std::endl;
using std::hex;
using std::showbase;
using std::setw;
using std::right;
using std::ofstream;

struct t_tlb_params {
    uint32_t  assoc;         /* Associativity level */
    size_t    nentries;      /* Number of entries */
    uint32_t  flags;         /* tlb flags */
    uint32_t  tlb_no;        /* tlb no */
    string    name;          /* name */
};


/* A booke tlb emulation */
/*    *
 * It defines all tlb arrays on a particular level
 * For eg. L1 or L2
 *
 * Suppose for eg, there are two parallel tlb arrays ( tlb0 and tlb1 ) on L2 level
 * The constructor has been overloaded for following formats :- 
 * 1. ppc_tlb_booke ( int n_tlbs, t_tlb_parms *tlbparms ) .
 *    @brief :- 
 *             n_tlbs -> number of tlbs
 *             tlbparms -> pointer to an array of t_tlb_params, wherein each entry corresponds to
 *                         parameters for that paricular tlb entry
 *
 * 2. ppc_tlb_booke ( int n_tlbs, ... )
 *    @brief :-
 *            n_tlbs -> number of tlbs
 *            variable arguments :-
 *                             varg0 -> nentries for tlb0
 *                             varg1 -> assoc for tlb0
 *                             varg2 -> flags for tlb0
 *                             varg3 -> tlbno
 *                             varg4 -> name for tlb0
 *
 *                             ( repeated for each tlb no )
 *
 *    */

class ppc_tlb_booke {

    struct t_tlb_entry {
        uint32_t  tid;     /* ~ PID */
        uint64_t  epn;     /* Effective page no */
        uint64_t  rpn;     /* Real Page no */
        uint32_t  rpb;     /* physical address bits ( as supported by implementation ) */
        uint64_t  ps;      /* page size */
        uint32_t  wimge;   /* wimge attributes */
        uint32_t  x01;     /* x0-x1 page attribute */
        uint32_t  u03;     /* u0-u3 user defined attributes */
        uint32_t  permis;  /* Permission */
        struct {
            uint64_t ts    : 1;  /* Translation space */
            uint64_t tsize : 4;  /* Logarithmic page size */
            uint64_t iprot : 1;  /* Invalidate protect */
            uint64_t valid : 1;  /* Valid ? */
        } tflags;

    };

    struct t_tlb_set {
        vector<t_tlb_entry> tlb_way;   /* Vector of TLB entries */
        size_t              n_ways;    /* Number of ways */
    };

    struct t_tlb {
        uint32_t  tlbno;         /* tlb no */
        uint32_t  assoc;         /* Associativity level */
        uint32_t ps;            /* Page size */
        uint32_t flags;         /* tlb flags */
        string   name;          /* name */
        size_t   n_sets;        /* Number of sets */
        vector<t_tlb_set> tlb_set;  /* Vector of sets */
    };

    private:
    list<t_tlb> tlb;                /* Different TLB arrays are linked in a list fashion */
    size_t n_tlbs;
    //ofstream tlb_ofstream;        /* For directing error/log messages */
    
    public:
    typedef list<t_tlb>::iterator t_tlb_iterator;        /* tlb iterator type */

    private:
    t_tlb_iterator get_tlb(int tlb_no);
    void tlb_push_back(const t_tlb& tlb_this);
    int init_ppc_tlb(size_t n_tlbs);
    int init_ppc_tlb_sets(int tlb_no, size_t n_sets);
    int init_ppc_tlb_ways(size_t tlb_no, size_t n_ways);
    int init_ppc_tlb_defaults(void);
    int print_tlb_entry(t_tlb_entry *entry, string fmtstr="    ");

    public:
    ppc_tlb_booke(size_t n_tlbs, struct t_tlb_params *tlbparms);
    ppc_tlb_booke(size_t n_tlbs, ...);
    ppc_tlb_booke();
    ~ppc_tlb_booke();

    int add_tlb(size_t nentries, int assoc, uint32_t flags, int tlb_no, string name);
    int init_defaults();
    int print_tlbs();
    int print_tlbs2();
    
    int tlbre(uint32_t &mas0, uint32_t &mas1, uint32_t &mas2, uint32_t &mas3, uint32_t &mas7, uint32_t hid0);
    int tlbwe(uint32_t mas0, uint32_t mas1, uint32_t mas2, uint32_t mas3, uint32_t mas7, uint32_t hid0);
    int tlbse(uint64_t ea, uint32_t &mas0, uint32_t &mas1, uint32_t &mas2, uint32_t &mas3, uint32_t &mas6, uint32_t &mas7, uint32_t hid0);
    int tlbive(uint64_t ea);

};

// Member functions

/* Get tlb entry corresponding to a tlbno */
ppc_tlb_booke::t_tlb_iterator ppc_tlb_booke::get_tlb(int tlb_no){
    t_tlb_iterator i;
    for(i=tlb.begin(); i!= tlb.end(); i++){
        if(static_cast<int>(i->tlbno) == tlb_no)
            return i;
    }
    return tlb.end();
}

/* Push this tlb to the global list */
void ppc_tlb_booke::tlb_push_back(const t_tlb& tlb_this){
    n_tlbs++;
    tlb.push_back(tlb_this);
}

/* Initialize no of tlb arrays */
int ppc_tlb_booke::init_ppc_tlb(size_t n_tlbs){
    this->n_tlbs = n_tlbs;
    tlb.resize(n_tlbs);
    return 0;
}

/* Initialize no of sets for a paricular tlb array */
int ppc_tlb_booke::init_ppc_tlb_sets(int tlb_no, size_t n_sets){
    assert_ret(IF_POWER_OF_2(n_sets), 1);

    t_tlb_iterator tlb_this = get_tlb(tlb_no);
    assert_ret(tlb_this != tlb.end(), -1); 

    tlb_this->n_sets = n_sets;
    tlb_this->tlb_set.resize(n_sets);

    if(n_sets == 1){  /* If number of sets = 1, then associativity of this tlb array is 0 */
        tlb_this->assoc = 0;
    }
    return 0;
}

/* Initialize no of ways for a particular tlb array */
int ppc_tlb_booke::init_ppc_tlb_ways(size_t tlb_no, size_t n_ways){
    assert_ret(n_ways >= 1, 1);
    assert_ret(IF_POWER_OF_2(n_ways), 1);

    t_tlb_iterator tlb_this = get_tlb(tlb_no);
    assert_ret(tlb_this != tlb.end(), -1);

    for(size_t i=0; i<tlb_this->n_sets; i++){
        tlb_this->tlb_set[i].n_ways = n_ways;
        tlb_this->tlb_set[i].tlb_way.resize(n_ways);
    }
    return 0;
}

/* Initialize default values on per entry basis for all tlb arrays */
int ppc_tlb_booke::init_ppc_tlb_defaults(void){
    for(t_tlb_iterator tlb_this=tlb.begin(); tlb_this != tlb.end(); tlb_this++){       /* Iterate over all tlb arrays */
        if(tlb_this->assoc != 0){     /* If associativity of this tlb array is not zero */
            tlb_this->ps = 0x1000;    /* Default ps */
        }
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                if(tlb_this->assoc != 0){
                    tlb_this->tlb_set[j].tlb_way[k].ps = 0x1000;  /* default ps for this tlb array */
                }
                tlb_this->tlb_set[j].tlb_way[k].tflags.valid = 0;  /* Disable all entries */
            }
        }

    }
    /*
     * NOTE : As long as we don't resize any of the containers , or they don't get reallocated,
     *        we can safely use pointers to access individual elements.
     */
    t_tlb_iterator tlb_this = get_tlb(1);       /* Get tlb array with tlbno = 1 */
    assert_ret(tlb_this != tlb.end(), -1);

    /* Initialize one default tlb entry in tlb1 for the reset vector */
    t_tlb_entry *entry = &(tlb_this->tlb_set[0].tlb_way[0]);

    /* Entry for boot page */
    entry->tflags.valid = 1;
    entry->tflags.ts    = 0;
    entry->tid          = 0;
    entry->epn          = 0xfffff;
    entry->rpn          = 0xfffff;
    entry->tflags.tsize = 1;                  /* 4K page size */
    entry->ps           = 0x1000;
    entry->permis       = 0b10101;            /* sx-sr-sw = 111, ux-ur-uw = 000 */
    entry->wimge        = 0b1000;
    entry->x01          = 0;
    entry->u03          = 0;
    entry->tflags.iprot = 1;

    return 0;

}

/*
 * @func : print_tlb_entry
 * @args : entry -> pointer to tlb entry
 *         fmtstr -> format string ( number of spaces for formatting )
 *
 * @brief : print one tlb entry at a time.
 */ 
int ppc_tlb_booke::print_tlb_entry(t_tlb_entry *entry, string fmtstr){
    assert_ret(entry, -1);

    cout << fmtstr << "tid    -> " << hex << entry->tid << endl;
    cout << fmtstr << "epn    -> " << hex << entry->epn << endl;
    cout << fmtstr << "rpn    -> " << hex << entry->rpn << endl;
    cout << fmtstr << "ps     -> " << hex << entry->ps  << endl;
    cout << fmtstr << "wimge  -> " << hex << entry->wimge << endl;
    cout << fmtstr << "permis -> " << hex << entry->permis << endl;
    cout << fmtstr << "x01    -> " << hex << entry->x01 << endl;
    cout << fmtstr << "u03    -> " << hex << entry->u03 << endl;
    cout << fmtstr << "ts     -> " << hex << entry->tflags.ts << endl;
    cout << fmtstr << "tsize  -> " << hex << entry->tflags.tsize << endl;
    cout << fmtstr << "iprot  -> " << hex << entry->tflags.iprot << " (please ignore, if not tlbCam)" << endl;
    cout << fmtstr << "valid  -> " << hex << entry->tflags.valid << endl;

    return 0;
}

/* Tlb constructor */
/* NOTE: This contructor hasn't been tested and might not work at all */
ppc_tlb_booke::ppc_tlb_booke(size_t n_tlbs, struct t_tlb_params *tlbparms){
    init_ppc_tlb(n_tlbs);

    for(size_t i=0; i<n_tlbs; i++){

        t_tlb tlb_this;

        size_t nentries = tlbparms[i].nentries;
        unsigned assoc  = tlbparms[i].assoc;

        tlb_this.assoc = tlbparms[i].assoc;
        tlb_this.name  = tlbparms[i].name;
        tlb_this.flags = tlbparms[i].flags;
        tlb_this.tlbno = tlbparms[i].tlb_no;

        /* Check if associativity is >=0 and power of 2 */
        assert((assoc >= 0) && IF_POWER_OF_2(assoc));

        /* Push tlb array to the list */
        tlb_push_back(tlb_this);

        if(tlb_this.assoc == 0 ){            /* Associativity is zero */
            init_ppc_tlb_sets(tlb_this.tlbno, 1);
            init_ppc_tlb_ways(tlb_this.tlbno, nentries);
        }else{
            init_ppc_tlb_sets(tlb_this.tlbno, int(nentries/assoc));
            init_ppc_tlb_ways(tlb_this.tlbno, assoc);
        }
    }

    /* Initialize default tlb parameters , based on passed information */
    init_ppc_tlb_defaults();
}

/* Direct arguments form */
/*
 * n_tlbs -> no of tlbs
 *
 * varg0 -> nentries for this tlb
 * varg1 -> assoc for this tlb
 * varg2 -> flags for this tlb
 * varg3 -> tlbno for this tlb
 * varg4 -> name for this tlb
 *
 * varg5 -> nentries for this tlb 
 * varg6 -> assoc for this tlb
 * varg7 -> flags for this tlb
 * varg8 -> tlbno for this tlb
 * varg9 -> name for this tlb
 *
 * and so on...
 */
ppc_tlb_booke::ppc_tlb_booke(size_t n_tlbs, ...){

    /* We can't use assert_ret in the constructor, since they don't have return values */

    /* No of tlb arrays should be atleast 2 */
    assert(n_tlbs >= 2);

    init_ppc_tlb(n_tlbs);

    va_list tlb_args;
    va_start(tlb_args, n_tlbs);

    for(unsigned i=0; i<n_tlbs; i++){
        t_tlb tlb_this;

        int nentries   = va_arg(tlb_args, int);
        int assoc      = va_arg(tlb_args, int);
        int flags      = va_arg(tlb_args, int);
        int tlb_no     = va_arg(tlb_args, int);
        char* name     = va_arg(tlb_args, char *);

        assert(nentries >= 0);
        assert(assoc >= 0 && IF_POWER_OF_2(assoc));

        /* Check for valid configurations */
        assert((tlb_no != 0)?(assoc == 0):(assoc != 0));

        tlb_this.assoc = assoc;
        tlb_this.name = name;
        tlb_this.flags = flags;
        tlb_this.tlbno = tlb_no;

        tlb_push_back(tlb_this);

        if(tlb_this.assoc == 0 ){            /* Associativity is zero */
            init_ppc_tlb_sets(i, 1);
            init_ppc_tlb_ways(i, nentries);
        }else{
            init_ppc_tlb_sets(i, int(nentries/assoc));
            init_ppc_tlb_ways(i, assoc);
        }
    }

    /* Initialize default tlb parameters , based on passed information */
    init_ppc_tlb_defaults();

    va_end(tlb_args);

}

/*
 * dummy tlb ( In case the object is defined inside another class )
 */
ppc_tlb_booke::ppc_tlb_booke(){ }

/* Destructor */
ppc_tlb_booke::~ppc_tlb_booke(){ }

/*
 * @func : add_tlb (add a new tlb)
 * @args : assoc, nentries, flags, name
 *         assoc -> associativity
 *         nentries -> total number of entries in this tlb
 *         flags -> flags for this tlb
 *         name -> name of this tlb ( for eg. tlb0, tlb1 etc. )
 *
 * @brief : Adds a new tlb according to the information passed.
 */
int ppc_tlb_booke::add_tlb(size_t nentries, int assoc, uint32_t flags, int tlb_no, string name){

    assert_ret(nentries >= 0, 1);
    assert_ret(assoc >= 0 && IF_POWER_OF_2(assoc), 1);

    /* Check for valid configurations */
    assert_ret((tlb_no != 0)?(assoc == 0):(assoc != 0), 1);

    t_tlb tlb_this;

    tlb_this.assoc = assoc;
    tlb_this.name  = name;
    tlb_this.flags = flags;
    tlb_this.tlbno = tlb_no;

    tlb_push_back(tlb_this);

    if(assoc == 0 ){             /* Associativity is zero */
        init_ppc_tlb_sets(tlb_no, 1);
        init_ppc_tlb_ways(tlb_no, nentries);
    }else{
        init_ppc_tlb_sets(tlb_no, int(nentries/assoc));
        init_ppc_tlb_ways(tlb_no, assoc);
    }

    return 0;
}

/*
 * @func : init_defaults ( initialize default setup )
 * @args : none
 *
 * @brief : initialize defualt tlb entries
 */
int ppc_tlb_booke::init_defaults(){
    return init_ppc_tlb_defaults();
}

/*
 * @func : print_tlbs()
 * @args : none
 *
 * @brief : print all tlb entries in a nice form
 */
int ppc_tlb_booke::print_tlbs(){
    for(t_tlb_iterator tlb_this=tlb.begin(); tlb_this != tlb.end(); tlb_this++){
        cout << tlb_this->name << endl;
        cout << "    assoc -> " << hex << tlb_this->assoc << endl;
        cout << "    ps    -> " << hex << tlb_this->ps << " (please ignore, if tlbCam)" << endl;
        cout << "    no    -> " << hex << tlb_this->tlbno << endl;
        cout << "    flags -> " << hex << tlb_this->flags << endl;
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                t_tlb_entry *entry_this = &(tlb_this->tlb_set[j].tlb_way[k]);
                if(entry_this->tflags.valid){           /* print only if the entry is valid */
                    cout << "        set_no " << j << " way_no " << k << endl;
                    assert_ret(print_tlb_entry(entry_this, "            ") == 0, -1);
                }
            }
        }
    }
    return 0;
}

/*
 * @func : print_tlbs()
 * @args : none
 *
 * @brief : print all tlb entries in a nice form
 */
int ppc_tlb_booke::print_tlbs2(){
#define PRINT_TLB_ENT(set, way, epn, rpn, tid, ts, wimge, permis, ps, x01, u03, iprot)          \
    cout << showbase;                                                                       \
    cout << setw(4) << right << set << setw(6) << right << way                              \
         << setw(16) << right << hex << epn << setw(16) << right << hex << rpn              \
         << setw(6) << right << tid << setw(4) << right << ts                               \
         << setw(6) << right << hex << wimge << setw(9) << right << hex << permis           \
         << setw(16) << right << hex << ps << setw(6) << right << hex << x01                \
         << setw(6) << right << hex << u03 << setw(6) << right << iprot << endl

    for(t_tlb_iterator tlb_this=tlb.begin(); tlb_this != tlb.end(); tlb_this++){
        cout << tlb_this->name << endl;
        PRINT_TLB_ENT("set", "way", "epn", "rpn", "tid", "ts", "wimge", "permis", "page size", "x01", "u03", "iprot");
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                t_tlb_entry *entry_this = &(tlb_this->tlb_set[j].tlb_way[k]);
                if(entry_this->tflags.valid){           /* print only if the entry is valid */
                    PRINT_TLB_ENT(j, k, entry_this->epn, entry_this->rpn,
                            entry_this->tid, entry_this->tflags.ts,
                            entry_this->wimge, entry_this->permis,
                            entry_this->ps, entry_this->x01,
                            entry_this->u03, entry_this->tflags.iprot);
                }
            }
        }
        cout << endl;
    }
    return 0;
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
int ppc_tlb_booke::tlbre(uint32_t &mas0, uint32_t &mas1, uint32_t &mas2, uint32_t &mas3, uint32_t &mas7, uint32_t hid0){

    unsigned tlbsel = EBMASK(mas0,       MAS0_TLBSEL);
    unsigned esel   = EBMASK(mas0,       MAS0_ESEL);
    uint64_t epn    = EBMASK(mas2,       MAS2_EPN);
    t_tlb_entry *entry = NULL;

    unsigned setn, wayn;

    /* Wrong tlbsel */
    assert_ret(tlbsel < n_tlbs, 1);
   
    t_tlb_iterator tlb_this = get_tlb(tlbsel);
    assert_ret(tlb_this != tlb.end(), -1);

    /* For fully associative arrays,  esel selects the set_no and epn is not used
     *
     * but for n way set-associative, esel selects way_no and EPN[45-51] selects the desired set_no
     */
    setn = (tlb_this->assoc)?(epn & (tlb_this->assoc - 1)):0;
    wayn = esel;

    assert_ret(setn < tlb_this->n_sets, 1);
    assert_ret(wayn < tlb_this->tlb_set[setn].n_ways, 1);

    /* Get tlb entry */
    entry = &(tlb_this->tlb_set[setn].tlb_way[wayn]);

    /* FIXME: Does the spec. says anything about nv when used with tlbre */
    mas0  |= IBMASK(0,                   MAS0_NV);

    mas1  |= IBMASK(entry->tflags.valid, MAS1_V);
    mas1  |= IBMASK(entry->tflags.iprot, MAS1_IPROT);
    mas1  |= IBMASK(entry->tid,          MAS1_TID);
    mas1  |= IBMASK(entry->tflags.ts,    MAS1_TS);
    mas1  |= IBMASK(entry->tflags.tsize, MAS1_TSIZE);
   
    mas2  |= IBMASK(entry->epn,          MAS2_EPN);
    mas2  |= IBMASK(entry->x01,          MAS2_X01);
    mas2  |= IBMASK(entry->wimge,        MAS2_WIMGE);

    mas3  |= IBMASK(entry->rpn,          MAS3_RPN);
    mas3  |= IBMASK(entry->u03,          MAS3_U03);
    mas3  |= IBMASK(entry->permis,       MAS3_PERMIS);

    if(EBMASK(hid0, HID0_EN_MAS7_UPDATE))
        mas7  |= IBMASK((entry->rpn >> 20) & 0xf, MAS7_RPN);    /* Upper 4 bits of rpn */

    return 0;

}
/* @func : tlbwe (Tlb write entry)
 * @args :
 *         mas0, mas1, mas2, mas3, mas7 -> MAS registers.
 *         hid0 -> HID0 register
 */
int ppc_tlb_booke::tlbwe(uint32_t mas0, uint32_t mas1, uint32_t mas2, uint32_t mas3, uint32_t mas7, uint32_t hid0){
    unsigned tlbsel = EBMASK(mas0,    MAS0_TLBSEL);
    unsigned esel   = EBMASK(mas0,    MAS0_ESEL);
    uint64_t epn    = EBMASK(mas2,    MAS2_EPN);
    t_tlb_entry *entry = NULL;

    unsigned setn, wayn;

    /* Wrong tlbsel */
    assert_ret(tlbsel < n_tlbs, 1);

    t_tlb_iterator tlb_this = get_tlb(tlbsel);
    assert_ret(tlb_this != tlb.end(), -1);

    /* For fully associative arrays,  esel selects the set_no and epn is not used
     *
     * but for n way set-associative, esel selects way_no and EPN[45-51] selects the desired set_no
     */
    setn = (tlb_this->assoc)?(epn & (tlb_this->assoc - 1)):0;
    wayn = esel;

    assert_ret(setn < tlb_this->n_sets, 1);
    assert_ret(wayn < tlb_this->tlb_set[setn].n_ways, 1);

    /* Get tlb entry */
    entry = &(tlb_this->tlb_set[setn].tlb_way[wayn]);

    /*FIXME : Need to check, if valid bit is passed as part of MAS1 or set implicitly */
    entry->tflags.valid = EBMASK(mas1,   MAS1_V);
    entry->tflags.iprot = EBMASK(mas1,   MAS1_IPROT); 
    entry->tid          = EBMASK(mas1,   MAS1_TID);
    entry->tflags.ts    = EBMASK(mas1,   MAS1_TS);
    entry->tflags.tsize = EBMASK(mas1,   MAS1_TSIZE);
   
    entry->epn          = EBMASK(mas2,   MAS2_EPN);
    entry->x01          = EBMASK(mas2,   MAS2_X01);
    entry->wimge        = EBMASK(mas2,   MAS2_WIMGE);

    entry->rpn          = EBMASK(mas3,   MAS3_RPN);
    entry->u03          = EBMASK(mas3,   MAS3_U03);
    entry->permis       = EBMASK(mas3,   MAS3_PERMIS);

    if(EBMASK(hid0, HID0_EN_MAS7_UPDATE))
        entry->rpn |= EBMASK(mas7, MAS7_RPN) << 20;    /* Upper 4 bits of rpn */

    return 0;
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
int ppc_tlb_booke::tlbse(uint64_t ea, uint32_t &mas0, uint32_t &mas1, uint32_t &mas2, uint32_t &mas3, uint32_t &mas6, uint32_t &mas7, uint32_t hid0){
    uint64_t epn  = (ea >> 12);
    uint32_t  as  = EBMASK(mas6,  MAS6_SAS);
    uint32_t pid  = EBMASK(mas6,  MAS6_SPID0);
    size_t   tsel = -1;
    size_t   ssel = -1;
    size_t   wsel = -1;
    t_tlb_entry *entry = NULL;

    /* Start searching in the tlb arrays */
    for(t_tlb_iterator tlb_this=tlb.begin(); tlb_this != tlb.end(); tlb_this++){
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                if((epn == tlb_this->tlb_set[j].tlb_way[k].epn)        &&
                   (as  == tlb_this->tlb_set[j].tlb_way[k].tflags.ts)  &&
                   (pid == tlb_this->tlb_set[j].tlb_way[k].tid)){
                    entry = &(tlb_this->tlb_set[j].tlb_way[k]);
                    tsel = tlb_this->tlbno;
                    ssel = j;
                    wsel = k;
                    goto exit_loop_0;
                }
            }
        }
    }

    exit_loop_0:
    /* Valid bit set means, search was successful */
    mas1  |= IBMASK((entry)?1:0, MAS1_V);
    /* Now return if the search was unsuccessful */
    assert_ret(entry != NULL, 1);

    mas0  |= IBMASK(tsel,  MAS0_TLBSEL);
    mas0  |= IBMASK(wsel,  MAS0_ESEL);
    /* FIXME : need to check the behaviour of MAS0[NV] on tlbsx */
    mas0  |= IBMASK(0,     MAS0_NV);

    mas1  |= IBMASK(entry->tflags.iprot, MAS1_IPROT);
    mas1  |= IBMASK(entry->tid,          MAS1_TID);
    mas1  |= IBMASK(entry->tflags.ts,    MAS1_TS);
    mas1  |= IBMASK(entry->tflags.tsize, MAS1_TSIZE);
   
    mas2  |= IBMASK(entry->epn,          MAS2_EPN);
    mas2  |= IBMASK(entry->x01,          MAS2_EPN);
    mas2  |= IBMASK(entry->wimge,        MAS2_WIMGE);

    mas3  |= IBMASK(entry->rpn,          MAS3_RPN);
    mas3  |= IBMASK(entry->u03,          MAS3_U03);
    mas3  |= IBMASK(entry->permis,       MAS3_PERMIS);

    if(EBMASK(hid0, HID0_EN_MAS7_UPDATE))
        mas7 |= IBMASK((entry->rpn >> 20) & 0xf,  MAS7_RPN);    /* Upper 4 bits of rpn */

    return 0;
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
int ppc_tlb_booke::tlbive(uint64_t ea){
    //uint64_t epn = (ea >> 12);
    uint8_t  tlbsel = (ea >> 3) & 0x1;
    uint8_t  inv_all = (ea >> 2) & 0x1;
    t_tlb_entry *entry = NULL;

    t_tlb_iterator tlb_this = get_tlb(tlbsel);
    assert_ret(tlb_this != tlb.end(), -1);

    assert_ret((tlbsel == 1) && (tlb_this->assoc == 0), 1);
    assert_ret((tlbsel == 0) && (tlb_this->assoc != 0), 1);

    /* Test if INV_ALL is set and tlbno = 1 */
    if(inv_all && (tlbsel == 1)){
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                entry = &(tlb_this->tlb_set[j].tlb_way[k]);
                if(!entry->tflags.iprot){
                    entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
                }
            }
        }
    }else /* Test if inv_all and tlbno = 1 */
    if(inv_all && (tlbsel == 0)){
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                entry = &(tlb_this->tlb_set[j].tlb_way[k]);
                entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
            }
        }
    }else /* Test for other cases */
    if(!inv_all)
    {
        for(size_t j=0; j<tlb_this->n_sets; j++){
            for(size_t k=0; k<tlb_this->tlb_set[j].n_ways; k++){
                entry = &(tlb_this->tlb_set[j].tlb_way[k]);
                if(tlbsel == 0){
                    entry->tflags.valid = 0;      /* If IPROT is not set, invalidate this entry */ 
                    goto exit_loop_1;
                }else if(tlbsel == 1){
                    if(entry->tflags.iprot){
                        entry->tflags.valid = 0;
                        goto exit_loop_1;
                    }
                }
            }
        }
    }else
    {
        /* Whatever ?? */
    }
    exit_loop_1:
    return 0;
}

/* Initialize TLB like this
 * l2 tlb for e500v2
 * Eg:-
 * ppc_tlb_booke  tlb_l2(2, 256, 2, 0, 0, "tlb0", 16, 0, 0, 1, "tlb1" );
 */
