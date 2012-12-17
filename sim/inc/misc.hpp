#ifndef _MISC_HPP
#define _MISC_HPP

#include "cpu_ppc_regs.h"            // PPC register table
#include "utils.h"

//------------------------------------ instruction call frame ---------------------------------------------

// instruction call frame
#define N_IC_ARGS 6           // Max arguments supported
struct instr_call {
    struct instr_arg {
        size_t  v;         // Actual argument
        size_t  p;         // Indirect Pointer to the m_ireghash entry
        bool    t;         // Argument type ( 1=register, 0=value )
    };
    std::string   opcname;              // Opcode name
    std::string   fmt;                  // Display format
    uint64_t      opc;                  // opcode ( First 32 bit is opcode and next 32 bit is the entry no in opcode table )
    int           nargs;                // Number of arguments
    instr_arg     arg[N_IC_ARGS];       // Argument array
    void          *fptr;                // Function pointer

    instr_call(){
        opcname = "";
        fmt     = "";
        opc     = 0;
        nargs   = 0;
        for(int i=0; i<N_IC_ARGS; i++){
            arg[i].v = arg[i].p = arg[i].t = 0;
        }
        fptr = NULL;
    }
    // Dump state
    void dump_state(){
        std::cout << "name   : " << opcname << std::endl;
        std::cout << "opcode : " << std::hex << std::showbase << opc << std::endl;
        std::cout << "args   : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i].v << " ";
        }
        std::cout << std::endl;
        std::cout << "pargs  : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i].p << " ";
        }
        std::cout << std::endl;
        std::cout << "targs  : ";
        for(int i=0; i<nargs; i++){
            std::cout << std::hex << std::showbase << arg[i].t << " ";
        }
        std::cout << std::endl;
        std::cout << "fmt    : " << fmt << std::endl;
        std::cout << "nargs  : " << nargs << std::endl << std::endl;
    }
    // print instruction
    void print_instr(){
        std::string lfmt = fmt;
        for(int i=nargs; i<N_IC_ARGS; i++){
            lfmt += "%c";
        }
        fmt += "\n";
        // We use printf for printing , since we have format of arguments in a string.
        if(opcname == "")
            printf(".long 0x%lx\n", (arg[0].v & 0xffffffff));
        else
            printf(lfmt.c_str(), opcname.c_str(), arg[0].v, arg[1].v, arg[2].v, arg[3].v, arg[4].v, arg[5].v);
    }
    // Get instruction representation in string form ( Used for DEBUG logs )
    char* get_instr_str(){
        static char instr_str[100];
        std::string lfmt = fmt;
        for(int i=nargs; i<N_IC_ARGS; i++){
            lfmt += "%c";
        }
        fmt += " ";
        // We use printf for printing , since we have format of arguments in a string.
        if(opcname == "")
            sprintf(instr_str, ".long 0x%lx ", (arg[0].v & 0xffffffff));
        else
            sprintf(instr_str, lfmt.c_str(), opcname.c_str(), arg[0].v, arg[1].v, arg[2].v, arg[3].v, arg[4].v, arg[5].v);
        return instr_str;
    }

    // Get function for boost::python. We don't really need these in our c++ library
    template <int ARG_NUM> instr_arg& getarg(){
        return arg[ARG_NUM];
    }
};

//------------------ register file definitions --------------------------------------------
//
// powerPC register attribute flags
// if attr=0, this means the register is illegal
#define REG_READ_SUP        0x00000001UL                         // Register has read access in supervisor mode
#define REG_WRITE_SUP       0x00000002UL                         // Register has write access in supervisor mode
#define REG_READ_USR        0x00000004UL                         // Register has read access in user mode
#define REG_WRITE_USR       0x00000008UL                         // Register has write access in user mode
#define REG_CLEAR_W1C       0x00000010UL                         // Register can be cleared by writing 1 to the bits
#define REG_REQ_SYNC        0x00000020UL                         // Register write requires synchronization


// Some constants
#define    PPC_NGPRS        32

// PPC SPR (64 bit)
struct ppc_spr64 {
    uint64_t          value;     // value
    const uint64_t    fvalue;    // fvalue is to be used in case register is read only
    const uint64_t    attr;      // attribute ( permissions etc. )
    const int         indx;      // register index no. (sprno)

    // Constructor
    ppc_spr64(uint64_t val, uint64_t attr_, int regno): 
        value(val),
        fvalue(val),
        attr(attr_),
        indx(regno)
    { }

    // Getter/Setter functions for bit fields. ( Mainly for boost::python )
    uint64_t get_bf(uint64_t mask){
        return ((value & mask) >> rshift(mask));
    }
    void set_bf(uint64_t bf, uint64_t mask){
        value &= ~mask;
        value |= ((bf << rshift(mask)) & mask);
    }
};

typedef ppc_spr64  ppc_pmr64;

union ppc_msr {
    struct {
        uint64_t  resv0 : 32;
        uint64_t  resv1 : 5;
        uint64_t  ucle  : 1;    // MSR[37]
        uint64_t  spe   : 1;    // MSR[38]
        uint64_t  resv2 : 6;
        uint64_t  we    : 1;    // MSR[45]
        uint64_t  ce    : 1;    // MSR[46]
        uint64_t  resv3 : 1;
        uint64_t  ee    : 1;    // MSR[48]
        uint64_t  pr    : 1;    // MSR[49]
        uint64_t  fp    : 1;    // MSR[50]
        uint64_t  me    : 1;    // MSR[51]
        uint64_t  resv4 : 1;
        uint64_t  uble  : 1;    // MSR[53]
        uint64_t  de    : 1;    // MSR[54]
        uint64_t  resv5 : 2;
        uint64_t  is    : 1;    // MSR[58]
        uint64_t  ds    : 1;    // MSR[59]
        uint64_t  resv6 : 1;
        uint64_t  pmm   : 1;    // MSR[60]
        uint64_t  resv7 : 2;
    } bits;
    uint64_t value;

    ppc_msr(uint64_t msr_): value(msr_) {}
    ppc_msr(): value(0) {}
};

union ppc_cr {
    struct {
        uint64_t  resv0 : 32;

        uint64_t  lt0   : 1;  // ---------/ 
        uint64_t  gt0   : 1;  //         /_____ CR0
        uint64_t  eq0   : 1;  //        /
        uint64_t  so0   : 1;  // ------/

        uint64_t  lt1   : 1;
        uint64_t  gt1   : 1;
        uint64_t  eq1   : 1;
        uint64_t  so1   : 1;

        uint64_t  lt2   : 1;
        uint64_t  gt2   : 1;
        uint64_t  eq2   : 1;
        uint64_t  so2   : 1;

        uint64_t  lt3   : 1;
        uint64_t  gt3   : 1;
        uint64_t  eq3   : 1;
        uint64_t  so3   : 1;

        uint64_t  lt4   : 1;
        uint64_t  gt4   : 1;
        uint64_t  eq4   : 1;
        uint64_t  so4   : 1;

        uint64_t  lt5   : 1;
        uint64_t  gt5   : 1;
        uint64_t  eq5   : 1;
        uint64_t  so5   : 1;

        uint64_t  lt6   : 1;
        uint64_t  gt6   : 1;
        uint64_t  eq6   : 1;
        uint64_t  so6   : 1;

        uint64_t  lt7   : 1;
        uint64_t  gt7   : 1;
        uint64_t  eq7   : 1;
        uint64_t  so7   : 1;

    } bits;
    struct {
        uint64_t  resv0 : 32;
        uint64_t  cr0   : 4;
        uint64_t  cr1   : 4;
        uint64_t  cr2   : 4;
        uint64_t  cr3   : 4;
        uint64_t  cr4   : 4;
        uint64_t  cr5   : 4;
        uint64_t  cr6   : 4;
        uint64_t  cr7   : 4;
    } bfs;
    uint64_t value;
};

union ppc_xer {
    struct {
        uint64_t  resv0 : 32;
        uint64_t  so    : 1;
        uint64_t  ov    : 1;
        uint64_t  ca    : 1;
        uint64_t  resv1 : 29;
    } bits;
    uint64_t value;
};

// e500v2 PPC register file ( this is the file we are gonna use in our cpu )
struct ppc_regs {
    ppc_cr            cr;
    ppc_msr           msr;
    uint64_t          acc;

    // GPRs
    uint64_t          gpr[PPC_NGPRS];

    // SPRs
    ppc_spr64         atbl;
    ppc_spr64         atbu;
    ppc_spr64         csrr0;
    ppc_spr64         csrr1;
    ppc_spr64         ctr;
    ppc_spr64         dac1;
    ppc_spr64         dac2;
    ppc_spr64         dbcr0;
    ppc_spr64         dbcr1;
    ppc_spr64         dbcr2;
    ppc_spr64         dbsr;
    ppc_spr64         dear;
    ppc_spr64         dec;
    ppc_spr64         decar;
    ppc_spr64         esr;
    ppc_spr64         iac1;
    ppc_spr64         iac2;
    ppc_spr64         ivor0;
    ppc_spr64         ivor1;
    ppc_spr64         ivor2;
    ppc_spr64         ivor3;
    ppc_spr64         ivor4;
    ppc_spr64         ivor5;
    ppc_spr64         ivor6;
    ppc_spr64         ivor7;
    ppc_spr64         ivor8;
    ppc_spr64         ivor9;
    ppc_spr64         ivor10;
    ppc_spr64         ivor11;
    ppc_spr64         ivor12;
    ppc_spr64         ivor13;
    ppc_spr64         ivor14;
    ppc_spr64         ivor15;
    ppc_spr64         ivpr;
    ppc_spr64         lr;
    ppc_spr64         pid0;
    ppc_spr64         pid1;
    ppc_spr64         pid2;
    ppc_spr64         pir;
    ppc_spr64         pvr;
    ppc_spr64         sprg0;
    ppc_spr64         sprg1;
    ppc_spr64         sprg2;
    ppc_spr64         sprg3;
    ppc_spr64         sprg4;
    ppc_spr64         sprg5;
    ppc_spr64         sprg6;
    ppc_spr64         sprg7;
    ppc_spr64         srr0;
    ppc_spr64         srr1;
    ppc_spr64         tbl;
    ppc_spr64         tbu;
    ppc_spr64         tcr;
    ppc_spr64         tsr;
    ppc_spr64         usprg0;
    ppc_spr64         xer;

    ppc_spr64         bbear;
    ppc_spr64         bbtar;
    ppc_spr64         bucsr;
    ppc_spr64         hid0;
    ppc_spr64         hid1;
    ppc_spr64         ivor32;
    ppc_spr64         ivor33;
    ppc_spr64         ivor34;
    ppc_spr64         ivor35;
    ppc_spr64         l1cfg0; 
    ppc_spr64         l1cfg1;
    ppc_spr64         l1csr0;
    ppc_spr64         l1csr1;
    ppc_spr64         mas0;
    ppc_spr64         mas1;
    ppc_spr64         mas2;
    ppc_spr64         mas3;
    ppc_spr64         mas4;
    ppc_spr64         mas5;
    ppc_spr64         mas6;
    ppc_spr64         mas7;
    ppc_spr64         mcar;
    ppc_spr64         mcsr;
    ppc_spr64         mcsrr0;
    ppc_spr64         mcsrr1;
    ppc_spr64         mmucfg;
    ppc_spr64         mmucsr0;
    ppc_spr64         spefscr;
    ppc_spr64         svr;
    ppc_spr64         tlb0cfg;
    ppc_spr64         tlb1cfg;

    // PMRs
    ppc_pmr64         pmgc0;
    ppc_pmr64         pmlca0;
    ppc_pmr64         pmlca1;
    ppc_pmr64         pmlca2;
    ppc_pmr64         pmlca3;
    ppc_pmr64         pmlcb0;
    ppc_pmr64         pmlcb1;
    ppc_pmr64         pmlcb2;
    ppc_pmr64         pmlcb3;
    ppc_pmr64         pmc0;
    ppc_pmr64         pmc1;
    ppc_pmr64         pmc2;
    ppc_pmr64         pmc3;

    ppc_pmr64         upmgc0;
    ppc_pmr64         upmlca0;
    ppc_pmr64         upmlca1;
    ppc_pmr64         upmlca2;
    ppc_pmr64         upmlca3;
    ppc_pmr64         upmlcb0;
    ppc_pmr64         upmlcb1;
    ppc_pmr64         upmlcb2;
    ppc_pmr64         upmlcb3;
    ppc_pmr64         upmc0;
    ppc_pmr64         upmc1;
    ppc_pmr64         upmc2;
    ppc_pmr64         upmc3;

    // register ptrs
    std::map<std::string, uint64_t*>  m_reg;
    std::map<int,         uint64_t*>  m_ireg;

    // register attribute ptrs
    std::map<std::string, const uint64_t*>  m_reg_attr;
    std::map<int,         const uint64_t*>  m_ireg_attr;

    
    // Constructor
    ppc_regs(uint64_t msr_=0):
        msr     (msr_),

        atbl    (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR                                  ), SPRN_ATBL    ),
        atbu    (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR                                  ), SPRN_ATBU    ),
        csrr0   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_CSRR0   ),
        csrr1   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_CSRR1   ),
        ctr     (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR  | REG_WRITE_USR                 ), SPRN_CTR     ),
        dac1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DAC1    ),
        dac2    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DAC2    ),
        dbcr0   (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                                  ), SPRN_DBCR0   ),
        dbcr1   (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                                  ), SPRN_DBCR1   ),
        dbcr2   (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                                  ), SPRN_DBCR2   ),
        dbsr    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DBSR    ),
        dear    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DEAR    ),
        dec     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DEC     ),
        decar   (0, (REG_WRITE_SUP                                                                   ), SPRN_DECAR   ),
        esr     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_ESR     ),
        iac1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IAC1    ),
        iac2    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IAC2    ),
        ivor0   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR0   ),
        ivor1   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR1   ),
        ivor2   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR2   ),
        ivor3   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR3   ),
        ivor4   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR4   ),
        ivor5   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR5   ),
        ivor6   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR6   ),
        ivor7   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR7   ),
        ivor8   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR8   ),
        ivor9   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR9   ),
        ivor10  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR10  ),
        ivor11  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR11  ),
        ivor12  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR12  ),
        ivor13  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR13  ),
        ivor14  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR14  ),
        ivor15  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR15  ),
        ivpr    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVPR    ),
        lr      (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR | REG_WRITE_USR                   ), SPRN_LR      ),
        pid0    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_PID0    ),
        pid1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_PID1    ),
        pid2    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_PID2    ),
        pir     (0, (REG_READ_SUP                                                                    ), SPRN_PIR     ),
        pvr     (0, (REG_READ_SUP                                                                    ), SPRN_PVR     ),
        sprg0   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG0   ),
        sprg1   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG1   ),
        sprg2   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG2   ),
        sprg3   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG3   ),
        sprg4   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG4   ),
        sprg5   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG5   ),
        sprg6   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG6   ),
        sprg7   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG7   ),
        srr0    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SRR0    ),
        srr1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SRR1    ),
        tbl     (0, (REG_READ_USR                                                                    ), SPRN_TBWL    ),
        tbu     (0, (REG_WRITE_SUP                                                                   ), SPRN_TBWU    ),
        tcr     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_TCR     ),
        tsr     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_TSR     ),
        usprg0  (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR                  ), SPRN_USPRG0  ),
        xer     (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR                  ), SPRN_XER     ),

        bbear   (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR   | REG_REQ_SYNC ), SPRN_BBEAR   ),
        bbtar   (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR   | REG_REQ_SYNC ), SPRN_BBTAR   ),
        bucsr   (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_BUCSR   ),
        hid0    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_HID0    ),
        hid1    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_HID1    ),
        ivor32  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR32  ),
        ivor33  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR33  ),
        ivor34  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR34  ),
        ivor35  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR35  ),
        l1cfg0  (0, (REG_READ_SUP                                                                    ), SPRN_L1CFG0  ),
        l1cfg1  (0, (REG_READ_SUP                                                                    ), SPRN_L1CFG1  ),
        l1csr0  (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_L1CSR0  ),
        l1csr1  (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_L1CSR1  ),
        mas0    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS0    ),
        mas1    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS1    ),
        mas2    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS2    ),
        mas3    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS3    ),
        mas4    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS4    ),
        mas5    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS5    ),
        mas6    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS6    ),
        mas7    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS7    ),
        mcar    (0, (REG_READ_SUP                                                                    ), SPRN_MCAR    ),
        mcsr    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_MCSR    ),
        mcsrr0  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_MCSRR0  ),
        mcsrr1  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_MCSRR1  ),
        mmucfg  (0, (REG_READ_SUP                                                                    ), SPRN_MMUCFG  ),
        mmucsr0 (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MMUCSR0 ),
        spefscr (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_SPEFSCR ),
        svr     (0, (REG_READ_SUP                                                                    ), SPRN_SVR     ),
        tlb0cfg (0, (REG_READ_SUP                                                                    ), SPRN_TLB0CFG ),
        tlb1cfg (0, (REG_READ_SUP                                                                    ), SPRN_TLB1CFG ),

        pmgc0   (0, 0, PMRN_PMGC0   ),
        pmlca0  (0, 0, PMRN_PMLCA0  ),
        pmlca1  (0, 0, PMRN_PMLCA1  ),
        pmlca2  (0, 0, PMRN_PMLCA2  ),
        pmlca3  (0, 0, PMRN_PMLCA3  ),
        pmlcb0  (0, 0, PMRN_PMLCB0  ),
        pmlcb1  (0, 0, PMRN_PMLCB1  ),
        pmlcb2  (0, 0, PMRN_PMLCB2  ),
        pmlcb3  (0, 0, PMRN_PMLCB3  ),
        pmc0    (0, 0, PMRN_PMC0    ),
        pmc1    (0, 0, PMRN_PMC1    ),
        pmc2    (0, 0, PMRN_PMC2    ),
        pmc3    (0, 0, PMRN_PMC3    ),
        upmgc0  (0, 0, PMRN_UPMGC0  ),
        upmlca0 (0, 0, PMRN_UPMLCA0 ),
        upmlca1 (0, 0, PMRN_UPMLCA1 ),
        upmlca2 (0, 0, PMRN_UPMLCA2 ),
        upmlca3 (0, 0, PMRN_UPMLCA3 ),
        upmlcb0 (0, 0, PMRN_UPMLCB0 ),
        upmlcb1 (0, 0, PMRN_UPMLCB1 ),
        upmlcb2 (0, 0, PMRN_UPMLCB2 ),
        upmlcb3 (0, 0, PMRN_UPMLCB3 ),
        upmc0   (0, 0, PMRN_UPMC0   ),
        upmc1   (0, 0, PMRN_UPMC1   ),
        upmc2   (0, 0, PMRN_UPMC2   ),
        upmc3   (0, 0, PMRN_UPMC2   )
    {
        // Initialize value ptrs
        m_reg["msr"]        = &(msr.value                  );      m_ireg[REG_MSR]         = m_reg["msr"];
        m_reg["cr"]         = &(cr.value                   );      m_ireg[REG_CR]          = m_reg["cr"];
        m_reg["acc"]        = &(acc                        );      m_ireg[REG_ACC]         = m_reg["acc"];

        m_reg["atbl"]       = &(atbl.value                 );      m_ireg[REG_ATBL]        = m_reg["atbl"];
        m_reg["atbu"]       = &(atbu.value                 );      m_ireg[REG_ATBU]        = m_reg["atbu"];
        m_reg["csrr0"]      = &(csrr0.value                );      m_ireg[REG_CSRR0]       = m_reg["csrr0"];
        m_reg["csrr1"]      = &(csrr1.value                );      m_ireg[REG_CSRR1]       = m_reg["csrr1"];
        m_reg["ctr"]        = &(ctr.value                  );      m_ireg[REG_CTR]         = m_reg["ctr"];
        m_reg["dac1"]       = &(dac1.value                 );      m_ireg[REG_DAC1]        = m_reg["dac1"];
        m_reg["dac2"]       = &(dac2.value                 );      m_ireg[REG_DAC2]        = m_reg["dac2"];
        m_reg["dbcr0"]      = &(dbcr0.value                );      m_ireg[REG_DBCR0]       = m_reg["dbcr0"];
        m_reg["dbcr1"]      = &(dbcr1.value                );      m_ireg[REG_DBCR1]       = m_reg["dbcr1"];
        m_reg["dbcr2"]      = &(dbcr2.value                );      m_ireg[REG_DBCR2]       = m_reg["dbcr2"];
        m_reg["dbsr"]       = &(dbsr.value                 );      m_ireg[REG_DBSR]        = m_reg["dbsr"];
        m_reg["dear"]       = &(dear.value                 );      m_ireg[REG_DEAR]        = m_reg["dear"];
        m_reg["dec"]        = &(dec.value                  );      m_ireg[REG_DEC]         = m_reg["dec"];
        m_reg["decar"]      = &(decar.value                );      m_ireg[REG_DECAR]       = m_reg["decar"];
        m_reg["esr"]        = &(esr.value                  );      m_ireg[REG_ESR]         = m_reg["esr"];
        m_reg["iac1"]       = &(iac1.value                 );      m_ireg[REG_IAC1]        = m_reg["iac1"];
        m_reg["iac2"]       = &(iac2.value                 );      m_ireg[REG_IAC2]        = m_reg["iac2"];
        m_reg["ivor0"]      = &(ivor0.value                );      m_ireg[REG_IVOR0]       = m_reg["ivor0"];
        m_reg["ivor1"]      = &(ivor1.value                );      m_ireg[REG_IVOR1]       = m_reg["ivor1"]; 
        m_reg["ivor2"]      = &(ivor2.value                );      m_ireg[REG_IVOR2]       = m_reg["ivor2"];
        m_reg["ivor3"]      = &(ivor3.value                );      m_ireg[REG_IVOR3]       = m_reg["ivor3"];
        m_reg["ivor4"]      = &(ivor4.value                );      m_ireg[REG_IVOR4]       = m_reg["ivor4"];
        m_reg["ivor5"]      = &(ivor5.value                );      m_ireg[REG_IVOR5]       = m_reg["ivor5"];
        m_reg["ivor6"]      = &(ivor6.value                );      m_ireg[REG_IVOR6]       = m_reg["ivor6"];
        m_reg["ivor7"]      = &(ivor7.value                );      m_ireg[REG_IVOR7]       = m_reg["ivor7"];
        m_reg["ivor8"]      = &(ivor8.value                );      m_ireg[REG_IVOR8]       = m_reg["ivor8"];
        m_reg["ivor9"]      = &(ivor9.value                );      m_ireg[REG_IVOR9]       = m_reg["ivor9"];
        m_reg["ivor10"]     = &(ivor10.value               );      m_ireg[REG_IVOR10]      = m_reg["ivor10"];
        m_reg["ivor11"]     = &(ivor11.value               );      m_ireg[REG_IVOR11]      = m_reg["ivor11"];
        m_reg["ivor12"]     = &(ivor12.value               );      m_ireg[REG_IVOR12]      = m_reg["ivor12"];
        m_reg["ivor13"]     = &(ivor13.value               );      m_ireg[REG_IVOR13]      = m_reg["ivor13"];
        m_reg["ivor14"]     = &(ivor14.value               );      m_ireg[REG_IVOR14]      = m_reg["ivor14"];
        m_reg["ivor15"]     = &(ivor15.value               );      m_ireg[REG_IVOR15]      = m_reg["ivor15"];
        m_reg["ivpr"]       = &(ivpr.value                 );      m_ireg[REG_IVPR]        = m_reg["ivpr"];
        m_reg["lr"]         = &(lr.value                   );      m_ireg[REG_LR]          = m_reg["lr"];
        m_reg["pid"]        = &(pid0.value                 );      m_ireg[REG_PID]         = m_reg["pid"];
        m_reg["pir"]        = &(pir.value                  );      m_ireg[REG_PIR]         = m_reg["pir"];
        m_reg["pvr"]        = &(pvr.value                  );      m_ireg[REG_PVR]         = m_reg["pvr"];
        m_reg["sprg0"]      = &(sprg0.value                );      m_ireg[REG_SPRG0]       = m_reg["sprg0"];
        m_reg["sprg1"]      = &(sprg1.value                );      m_ireg[REG_SPRG1]       = m_reg["sprg1"];
        m_reg["sprg2"]      = &(sprg2.value                );      m_ireg[REG_SPRG2]       = m_reg["sprg2"];
        m_reg["sprg3r"]     = &(sprg3.value                );      m_ireg[REG_SPRG3R]      = m_reg["sprg3r"];
        m_reg["sprg3"]      = &(sprg3.value                );      m_ireg[REG_SPRG3]       = m_reg["sprg3"];
        m_reg["sprg4r"]     = &(sprg4.value                );      m_ireg[REG_SPRG4R]      = m_reg["sprg4r"];
        m_reg["sprg4"]      = &(sprg4.value                );      m_ireg[REG_SPRG4]       = m_reg["sprg4"];
        m_reg["sprg5r"]     = &(sprg5.value                );      m_ireg[REG_SPRG5R]      = m_reg["sprg5r"];
        m_reg["sprg5"]      = &(sprg5.value                );      m_ireg[REG_SPRG5]       = m_reg["sprg5"];
        m_reg["sprg6r"]     = &(sprg6.value                );      m_ireg[REG_SPRG6R]      = m_reg["sprg6r"];
        m_reg["sprg6"]      = &(sprg6.value                );      m_ireg[REG_SPRG6]       = m_reg["sprg6"];
        m_reg["sprg7r"]     = &(sprg7.value                );      m_ireg[REG_SPRG7R]      = m_reg["sprg7r"];
        m_reg["sprg7"]      = &(sprg7.value                );      m_ireg[REG_SPRG7]       = m_reg["sprg7"];
        m_reg["srr0"]       = &(srr0.value                 );      m_ireg[REG_SRR0]        = m_reg["srr0"];
        m_reg["srr1"]       = &(srr1.value                 );      m_ireg[REG_SRR1]        = m_reg["srr1"];
        m_reg["tbrl"]       = &(tbl.value                  );      m_ireg[REG_TBRL]        = m_reg["tbrl"];
        m_reg["tbwl"]       = &(tbl.value                  );      m_ireg[REG_TBWL]        = m_reg["tbwl"];
        m_reg["tbru"]       = &(tbu.value                  );      m_ireg[REG_TBRU]        = m_reg["tbru"];
        m_reg["tbwu"]       = &(tbu.value                  );      m_ireg[REG_TBWU]        = m_reg["tbwu"];
        m_reg["tcr"]        = &(tcr.value                  );      m_ireg[REG_TCR]         = m_reg["tcr"];
        m_reg["tsr"]        = &(tsr.value                  );      m_ireg[REG_TSR]         = m_reg["tsr"];
        m_reg["usprg0"]     = &(usprg0.value               );      m_ireg[REG_USPRG0]      = m_reg["usprg0"];
        m_reg["xer"]        = &(xer.value                  );      m_ireg[REG_XER]         = m_reg["xer"];

        m_reg["bbear"]      = &(bbear.value                 );     m_ireg[REG_BBEAR]       = m_reg["bbear"];
        m_reg["bbtar"]      = &(bbtar.value                 );     m_ireg[REG_BBTAR]       = m_reg["bbtar"];
        m_reg["bucsr"]      = &(bucsr.value                 );     m_ireg[REG_BUCSR]       = m_reg["bucsr"];
        m_reg["hid0"]       = &(hid0.value                  );     m_ireg[REG_HID0]        = m_reg["hid0"];
        m_reg["hid1"]       = &(hid1.value                  );     m_ireg[REG_HID1]        = m_reg["hid1"];
        m_reg["ivor32"]     = &(ivor32.value                );     m_ireg[REG_IVOR32]      = m_reg["ivor32"];
        m_reg["ivor33"]     = &(ivor33.value                );     m_ireg[REG_IVOR33]      = m_reg["ivor33"];
        m_reg["ivor34"]     = &(ivor34.value                );     m_ireg[REG_IVOR34]      = m_reg["ivor34"];
        m_reg["ivor35"]     = &(ivor35.value                );     m_ireg[REG_IVOR35]      = m_reg["ivor35"];
        m_reg["l1cfg0"]     = &(l1cfg0.value                );     m_ireg[REG_L1CFG0]      = m_reg["l1cfg0"];
        m_reg["l1cfg1"]     = &(l1cfg1.value                );     m_ireg[REG_L1CFG1]      = m_reg["l1cfg1"];
        m_reg["l1csr0"]     = &(l1csr0.value                );     m_ireg[REG_L1CSR0]      = m_reg["l1csr0"];
        m_reg["l1csr1"]     = &(l1csr1.value                );     m_ireg[REG_L1CSR1]      = m_reg["l1csr1"];
        m_reg["mas0"]       = &(mas0.value                  );     m_ireg[REG_MAS0]        = m_reg["mas0"];
        m_reg["mas1"]       = &(mas1.value                  );     m_ireg[REG_MAS1]        = m_reg["mas1"];
        m_reg["mas2"]       = &(mas2.value                  );     m_ireg[REG_MAS2]        = m_reg["mas2"];
        m_reg["mas3"]       = &(mas3.value                  );     m_ireg[REG_MAS3]        = m_reg["mas3"];
        m_reg["mas4"]       = &(mas4.value                  );     m_ireg[REG_MAS4]        = m_reg["mas4"];
        m_reg["mas5"]       = &(mas5.value                  );     m_ireg[REG_MAS5]        = m_reg["mas5"];
        m_reg["mas6"]       = &(mas6.value                  );     m_ireg[REG_MAS6]        = m_reg["mas6"];
        m_reg["mas7"]       = &(mas7.value                  );     m_ireg[REG_MAS7]        = m_reg["mas7"];
        m_reg["mcar"]       = &(mcar.value                  );     m_ireg[REG_MCAR]        = m_reg["mcar"];
        m_reg["mcsr"]       = &(mcsr.value                  );     m_ireg[REG_MCSR]        = m_reg["mcsr"];
        m_reg["mcsrr0"]     = &(mcsrr0.value                );     m_ireg[REG_MCSRR0]      = m_reg["mcsrr0"];
        m_reg["mcsrr1"]     = &(mcsrr1.value                );     m_ireg[REG_MCSRR1]      = m_reg["mcsrr1"];
        m_reg["mmucfg"]     = &(mmucfg.value                );     m_ireg[REG_MMUCFG]      = m_reg["mmucfg"];
        m_reg["mmucsr0"]    = &(mmucsr0.value               );     m_ireg[REG_MMUCSR0]     = m_reg["mmucsr0"];
        m_reg["pid0"]       = &(pid0.value                  );     m_ireg[REG_PID0]        = m_reg["pid0"];
        m_reg["pid1"]       = &(pid1.value                  );     m_ireg[REG_PID1]        = m_reg["pid1"];
        m_reg["pid2"]       = &(pid2.value                  );     m_ireg[REG_PID2]        = m_reg["pid2"];
        m_reg["spefscr"]    = &(spefscr.value               );     m_ireg[REG_SPEFSCR]     = m_reg["spefscr"];
        m_reg["svr"]        = &(svr.value                   );     m_ireg[REG_SVR]         = m_reg["svr"];
        m_reg["tlb0cfg"]    = &(tlb0cfg.value               );     m_ireg[REG_TLB0CFG]     = m_reg["tlb0cfg"];
        m_reg["tlb1cfg"]    = &(tlb1cfg.value               );     m_ireg[REG_TLB1CFG]     = m_reg["tlb1cfg"];

        // PMRs
        m_reg["pmc0"]       = &(pmc0.value                  );     m_ireg[REG_PMC0]        = m_reg["pmc0"];
        m_reg["pmc1"]       = &(pmc1.value                  );     m_ireg[REG_PMC1]        = m_reg["pmc1"];
        m_reg["pmc2"]       = &(pmc2.value                  );     m_ireg[REG_PMC2]        = m_reg["pmc2"];
        m_reg["pmc3"]       = &(pmc3.value                  );     m_ireg[REG_PMC3]        = m_reg["pmc3"];
        m_reg["pmlca0"]     = &(pmlca0.value                );     m_ireg[REG_PMLCA0]      = m_reg["pmlca0"];
        m_reg["pmlca1"]     = &(pmlca1.value                );     m_ireg[REG_PMLCA1]      = m_reg["pmlca1"];
        m_reg["pmlca2"]     = &(pmlca2.value                );     m_ireg[REG_PMLCA2]      = m_reg["pmlca2"];
        m_reg["pmlca3"]     = &(pmlca3.value                );     m_ireg[REG_PMLCA3]      = m_reg["pmlca3"];
        m_reg["pmlcb0"]     = &(pmlcb0.value                );     m_ireg[REG_PMLCB0]      = m_reg["pmlcb0"];
        m_reg["pmlcb1"]     = &(pmlcb1.value                );     m_ireg[REG_PMLCB1]      = m_reg["pmlcb1"];
        m_reg["pmlcb2"]     = &(pmlcb2.value                );     m_ireg[REG_PMLCB2]      = m_reg["pmlcb2"];
        m_reg["pmlcb3"]     = &(pmlcb2.value                );     m_ireg[REG_PMLCB3]      = m_reg["pmlcb3"];
        m_reg["pmgc0"]      = &(pmgc0.value                 );     m_ireg[REG_PMGC0]       = m_reg["pmgc0"];
        m_reg["upmc0"]      = &(upmc0.value                 );     m_ireg[REG_UPMC0]       = m_reg["upmc0"];
        m_reg["upmc1"]      = &(upmc1.value                 );     m_ireg[REG_UPMC1]       = m_reg["upmc1"];
        m_reg["upmc2"]      = &(upmc2.value                 );     m_ireg[REG_UPMC2]       = m_reg["upmc2"];
        m_reg["upmc3"]      = &(upmc3.value                 );     m_ireg[REG_UPMC3]       = m_reg["upmc3"];
        m_reg["upmlca0"]    = &(upmlca0.value               );     m_ireg[REG_UPMLCA0]     = m_reg["upmlca0"];
        m_reg["upmlca1"]    = &(upmlca1.value               );     m_ireg[REG_UPMLCA1]     = m_reg["upmlca1"];
        m_reg["upmlca2"]    = &(upmlca2.value               );     m_ireg[REG_UPMLCA2]     = m_reg["upmlca2"];
        m_reg["upmlca3"]    = &(upmlca3.value               );     m_ireg[REG_UPMLCA3]     = m_reg["upmlca3"];
        m_reg["upmlcb0"]    = &(upmlcb0.value               );     m_ireg[REG_UPMLCB0]     = m_reg["upmlcb0"];
        m_reg["upmlcb1"]    = &(upmlcb1.value               );     m_ireg[REG_UPMLCB1]     = m_reg["upmlcb1"];
        m_reg["upmlcb2"]    = &(upmlcb2.value               );     m_ireg[REG_UPMLCB2]     = m_reg["upmlcb2"];
        m_reg["upmlcb3"]    = &(upmlcb3.value               );     m_ireg[REG_UPMLCB3]     = m_reg["upmlcb3"];
        m_reg["upmgc0"]     = &(upmgc0.value                );     m_ireg[REG_UPMGC0]      = m_reg["upmgc0"];

        // GPRS
        for (size_t i=0; i<PPC_NGPRS; i++){
            m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "r" << i))->str()] = &(gpr[i]); 
            m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "gpr" << i))->str()] = &(gpr[i]);
            m_ireg[REG_GPR0 + i] = &(gpr[i]);
        }

        // Initialize attr ptrs
        m_reg_attr["atbl"]       = &(atbl.attr                 );      m_ireg_attr[REG_ATBL]        = m_reg_attr["atbl"];
        m_reg_attr["atbu"]       = &(atbu.attr                 );      m_ireg_attr[REG_ATBU]        = m_reg_attr["atbu"];
        m_reg_attr["csrr0"]      = &(csrr0.attr                );      m_ireg_attr[REG_CSRR0]       = m_reg_attr["csrr0"];
        m_reg_attr["csrr1"]      = &(csrr1.attr                );      m_ireg_attr[REG_CSRR1]       = m_reg_attr["csrr1"];
        m_reg_attr["ctr"]        = &(ctr.attr                  );      m_ireg_attr[REG_CTR]         = m_reg_attr["ctr"];
        m_reg_attr["dac1"]       = &(dac1.attr                 );      m_ireg_attr[REG_DAC1]        = m_reg_attr["dac1"];
        m_reg_attr["dac2"]       = &(dac2.attr                 );      m_ireg_attr[REG_DAC2]        = m_reg_attr["dac2"];
        m_reg_attr["dbcr0"]      = &(dbcr0.attr                );      m_ireg_attr[REG_DBCR0]       = m_reg_attr["dbcr0"];
        m_reg_attr["dbcr1"]      = &(dbcr1.attr                );      m_ireg_attr[REG_DBCR1]       = m_reg_attr["dbcr1"];
        m_reg_attr["dbcr2"]      = &(dbcr2.attr                );      m_ireg_attr[REG_DBCR2]       = m_reg_attr["dbcr2"];
        m_reg_attr["dbsr"]       = &(dbsr.attr                 );      m_ireg_attr[REG_DBSR]        = m_reg_attr["dbsr"];
        m_reg_attr["dear"]       = &(dear.attr                 );      m_ireg_attr[REG_DEAR]        = m_reg_attr["dear"];
        m_reg_attr["dec"]        = &(dec.attr                  );      m_ireg_attr[REG_DEC]         = m_reg_attr["dec"];
        m_reg_attr["decar"]      = &(decar.attr                );      m_ireg_attr[REG_DECAR]       = m_reg_attr["decar"];
        m_reg_attr["esr"]        = &(esr.attr                  );      m_ireg_attr[REG_ESR]         = m_reg_attr["esr"];
        m_reg_attr["iac1"]       = &(iac1.attr                 );      m_ireg_attr[REG_IAC1]        = m_reg_attr["iac1"];
        m_reg_attr["iac2"]       = &(iac2.attr                 );      m_ireg_attr[REG_IAC2]        = m_reg_attr["iac2"];
        m_reg_attr["ivor0"]      = &(ivor0.attr                );      m_ireg_attr[REG_IVOR0]       = m_reg_attr["ivor0"];
        m_reg_attr["ivor1"]      = &(ivor1.attr                );      m_ireg_attr[REG_IVOR1]       = m_reg_attr["ivor1"]; 
        m_reg_attr["ivor2"]      = &(ivor2.attr                );      m_ireg_attr[REG_IVOR2]       = m_reg_attr["ivor2"];
        m_reg_attr["ivor3"]      = &(ivor3.attr                );      m_ireg_attr[REG_IVOR3]       = m_reg_attr["ivor3"];
        m_reg_attr["ivor4"]      = &(ivor4.attr                );      m_ireg_attr[REG_IVOR4]       = m_reg_attr["ivor4"];
        m_reg_attr["ivor5"]      = &(ivor5.attr                );      m_ireg_attr[REG_IVOR5]       = m_reg_attr["ivor5"];
        m_reg_attr["ivor6"]      = &(ivor6.attr                );      m_ireg_attr[REG_IVOR6]       = m_reg_attr["ivor6"];
        m_reg_attr["ivor7"]      = &(ivor7.attr                );      m_ireg_attr[REG_IVOR7]       = m_reg_attr["ivor7"];
        m_reg_attr["ivor8"]      = &(ivor8.attr                );      m_ireg_attr[REG_IVOR8]       = m_reg_attr["ivor8"];
        m_reg_attr["ivor9"]      = &(ivor9.attr                );      m_ireg_attr[REG_IVOR9]       = m_reg_attr["ivor9"];
        m_reg_attr["ivor10"]     = &(ivor10.attr               );      m_ireg_attr[REG_IVOR10]      = m_reg_attr["ivor10"];
        m_reg_attr["ivor11"]     = &(ivor11.attr               );      m_ireg_attr[REG_IVOR11]      = m_reg_attr["ivor11"];
        m_reg_attr["ivor12"]     = &(ivor12.attr               );      m_ireg_attr[REG_IVOR12]      = m_reg_attr["ivor12"];
        m_reg_attr["ivor13"]     = &(ivor13.attr               );      m_ireg_attr[REG_IVOR13]      = m_reg_attr["ivor13"];
        m_reg_attr["ivor14"]     = &(ivor14.attr               );      m_ireg_attr[REG_IVOR14]      = m_reg_attr["ivor14"];
        m_reg_attr["ivor15"]     = &(ivor15.attr               );      m_ireg_attr[REG_IVOR15]      = m_reg_attr["ivor15"];
        m_reg_attr["ivpr"]       = &(ivpr.attr                 );      m_ireg_attr[REG_IVPR]        = m_reg_attr["ivpr"];
        m_reg_attr["lr"]         = &(lr.attr                   );      m_ireg_attr[REG_LR]          = m_reg_attr["lr"];
        m_reg_attr["pid"]        = &(pid0.attr                 );      m_ireg_attr[REG_PID]         = m_reg_attr["pid"];
        m_reg_attr["pir"]        = &(pir.attr                  );      m_ireg_attr[REG_PIR]         = m_reg_attr["pir"];
        m_reg_attr["pvr"]        = &(pvr.attr                  );      m_ireg_attr[REG_PVR]         = m_reg_attr["pvr"];
        m_reg_attr["sprg0"]      = &(sprg0.attr                );      m_ireg_attr[REG_SPRG0]       = m_reg_attr["sprg0"];
        m_reg_attr["sprg1"]      = &(sprg1.attr                );      m_ireg_attr[REG_SPRG1]       = m_reg_attr["sprg1"];
        m_reg_attr["sprg2"]      = &(sprg2.attr                );      m_ireg_attr[REG_SPRG2]       = m_reg_attr["sprg2"];
        m_reg_attr["sprg3r"]     = &(sprg3.attr                );      m_ireg_attr[REG_SPRG3R]      = m_reg_attr["sprg3r"];
        m_reg_attr["sprg3"]      = &(sprg3.attr                );      m_ireg_attr[REG_SPRG3]       = m_reg_attr["sprg3"];
        m_reg_attr["sprg4r"]     = &(sprg4.attr                );      m_ireg_attr[REG_SPRG4R]      = m_reg_attr["sprg4r"];
        m_reg_attr["sprg4"]      = &(sprg4.attr                );      m_ireg_attr[REG_SPRG4]       = m_reg_attr["sprg4"];
        m_reg_attr["sprg5r"]     = &(sprg5.attr                );      m_ireg_attr[REG_SPRG5R]      = m_reg_attr["sprg5r"];
        m_reg_attr["sprg5"]      = &(sprg5.attr                );      m_ireg_attr[REG_SPRG5]       = m_reg_attr["sprg5"];
        m_reg_attr["sprg6r"]     = &(sprg6.attr                );      m_ireg_attr[REG_SPRG6R]      = m_reg_attr["sprg6r"];
        m_reg_attr["sprg6"]      = &(sprg6.attr                );      m_ireg_attr[REG_SPRG6]       = m_reg_attr["sprg6"];
        m_reg_attr["sprg7r"]     = &(sprg7.attr                );      m_ireg_attr[REG_SPRG7R]      = m_reg_attr["sprg7r"];
        m_reg_attr["sprg7"]      = &(sprg7.attr                );      m_ireg_attr[REG_SPRG7]       = m_reg_attr["sprg7"];
        m_reg_attr["srr0"]       = &(srr0.attr                 );      m_ireg_attr[REG_SRR0]        = m_reg_attr["srr0"];
        m_reg_attr["srr1"]       = &(srr1.attr                 );      m_ireg_attr[REG_SRR1]        = m_reg_attr["srr1"];
        m_reg_attr["tbrl"]       = &(tbl.attr                  );      m_ireg_attr[REG_TBRL]        = m_reg_attr["tbrl"];
        m_reg_attr["tbwl"]       = &(tbl.attr                  );      m_ireg_attr[REG_TBWL]        = m_reg_attr["tbwl"];
        m_reg_attr["tbru"]       = &(tbu.attr                  );      m_ireg_attr[REG_TBRU]        = m_reg_attr["tbru"];
        m_reg_attr["tbwu"]       = &(tbu.attr                  );      m_ireg_attr[REG_TBWU]        = m_reg_attr["tbwu"];
        m_reg_attr["tcr"]        = &(tcr.attr                  );      m_ireg_attr[REG_TCR]         = m_reg_attr["tcr"];
        m_reg_attr["tsr"]        = &(tsr.attr                  );      m_ireg_attr[REG_TSR]         = m_reg_attr["tsr"];
        m_reg_attr["usprg0"]     = &(usprg0.attr               );      m_ireg_attr[REG_USPRG0]      = m_reg_attr["usprg0"];
        m_reg_attr["xer"]        = &(xer.attr                  );      m_ireg_attr[REG_XER]         = m_reg_attr["xer"];

        m_reg_attr["bbear"]      = &(bbear.attr                 );     m_ireg_attr[REG_BBEAR]       = m_reg_attr["bbear"];
        m_reg_attr["bbtar"]      = &(bbtar.attr                 );     m_ireg_attr[REG_BBTAR]       = m_reg_attr["bbtar"];
        m_reg_attr["bucsr"]      = &(bucsr.attr                 );     m_ireg_attr[REG_BUCSR]       = m_reg_attr["bucsr"];
        m_reg_attr["hid0"]       = &(hid0.attr                  );     m_ireg_attr[REG_HID0]        = m_reg_attr["hid0"];
        m_reg_attr["hid1"]       = &(hid1.attr                  );     m_ireg_attr[REG_HID1]        = m_reg_attr["hid1"];
        m_reg_attr["ivor32"]     = &(ivor32.attr                );     m_ireg_attr[REG_IVOR32]      = m_reg_attr["ivor32"];
        m_reg_attr["ivor33"]     = &(ivor33.attr                );     m_ireg_attr[REG_IVOR33]      = m_reg_attr["ivor33"];
        m_reg_attr["ivor34"]     = &(ivor34.attr                );     m_ireg_attr[REG_IVOR34]      = m_reg_attr["ivor34"];
        m_reg_attr["ivor35"]     = &(ivor35.attr                );     m_ireg_attr[REG_IVOR35]      = m_reg_attr["ivor35"];
        m_reg_attr["l1cfg0"]     = &(l1cfg0.attr                );     m_ireg_attr[REG_L1CFG0]      = m_reg_attr["l1cfg0"];
        m_reg_attr["l1cfg1"]     = &(l1cfg1.attr                );     m_ireg_attr[REG_L1CFG1]      = m_reg_attr["l1cfg1"];
        m_reg_attr["l1csr0"]     = &(l1csr0.attr                );     m_ireg_attr[REG_L1CSR0]      = m_reg_attr["l1csr0"];
        m_reg_attr["l1csr1"]     = &(l1csr1.attr                );     m_ireg_attr[REG_L1CSR1]      = m_reg_attr["l1csr1"];
        m_reg_attr["mas0"]       = &(mas0.attr                  );     m_ireg_attr[REG_MAS1]        = m_reg_attr["mas0"];
        m_reg_attr["mas1"]       = &(mas1.attr                  );     m_ireg_attr[REG_MAS1]        = m_reg_attr["mas1"];
        m_reg_attr["mas2"]       = &(mas2.attr                  );     m_ireg_attr[REG_MAS2]        = m_reg_attr["mas2"];
        m_reg_attr["mas3"]       = &(mas3.attr                  );     m_ireg_attr[REG_MAS3]        = m_reg_attr["mas3"];
        m_reg_attr["mas4"]       = &(mas4.attr                  );     m_ireg_attr[REG_MAS4]        = m_reg_attr["mas4"];
        m_reg_attr["mas5"]       = &(mas5.attr                  );     m_ireg_attr[REG_MAS5]        = m_reg_attr["mas5"];
        m_reg_attr["mas6"]       = &(mas6.attr                  );     m_ireg_attr[REG_MAS6]        = m_reg_attr["mas6"];
        m_reg_attr["mas7"]       = &(mas7.attr                  );     m_ireg_attr[REG_MAS7]        = m_reg_attr["mas7"];
        m_reg_attr["mcar"]       = &(mcar.attr                  );     m_ireg_attr[REG_MCAR]        = m_reg_attr["mcar"];
        m_reg_attr["mcsr"]       = &(mcsr.attr                  );     m_ireg_attr[REG_MCSR]        = m_reg_attr["mcsr"];
        m_reg_attr["mcsrr0"]     = &(mcsrr0.attr                );     m_ireg_attr[REG_MCSRR0]      = m_reg_attr["mcsrr0"];
        m_reg_attr["mcsrr1"]     = &(mcsrr1.attr                );     m_ireg_attr[REG_MCSRR1]      = m_reg_attr["mcsrr1"];
        m_reg_attr["mmucfg"]     = &(mmucfg.attr                );     m_ireg_attr[REG_MMUCFG]      = m_reg_attr["mmucfg"];
        m_reg_attr["mmucsr0"]    = &(mmucsr0.attr               );     m_ireg_attr[REG_MMUCSR0]     = m_reg_attr["mmucsr0"];
        m_reg_attr["pid0"]       = &(pid0.attr                  );     m_ireg_attr[REG_PID0]        = m_reg_attr["pid0"];
        m_reg_attr["pid1"]       = &(pid1.attr                  );     m_ireg_attr[REG_PID1]        = m_reg_attr["pid1"];
        m_reg_attr["pid2"]       = &(pid2.attr                  );     m_ireg_attr[REG_PID2]        = m_reg_attr["pid2"];
        m_reg_attr["spefscr"]    = &(spefscr.attr               );     m_ireg_attr[REG_SPEFSCR]     = m_reg_attr["spefscr"];
        m_reg_attr["svr"]        = &(svr.attr                   );     m_ireg_attr[REG_SVR]         = m_reg_attr["svr"];
        m_reg_attr["tlb0cfg"]    = &(tlb0cfg.attr               );     m_ireg_attr[REG_TLB0CFG]     = m_reg_attr["tlb0cfg"];
        m_reg_attr["tlb1cfg"]    = &(tlb1cfg.attr               );     m_ireg_attr[REG_TLB1CFG]     = m_reg_attr["tlb1cfg"];

        // PMRs
        m_reg_attr["pmc0"]       = &(pmc0.attr                  );     m_ireg_attr[REG_PMC0]        = m_reg_attr["pmc0"];
        m_reg_attr["pmc1"]       = &(pmc1.attr                  );     m_ireg_attr[REG_PMC1]        = m_reg_attr["pmc1"];
        m_reg_attr["pmc2"]       = &(pmc2.attr                  );     m_ireg_attr[REG_PMC2]        = m_reg_attr["pmc2"];
        m_reg_attr["pmc3"]       = &(pmc3.attr                  );     m_ireg_attr[REG_PMC3]        = m_reg_attr["pmc3"];
        m_reg_attr["pmlca0"]     = &(pmlca0.attr                );     m_ireg_attr[REG_PMLCA0]      = m_reg_attr["pmlca0"];
        m_reg_attr["pmlca1"]     = &(pmlca1.attr                );     m_ireg_attr[REG_PMLCA1]      = m_reg_attr["pmlca1"];
        m_reg_attr["pmlca2"]     = &(pmlca2.attr                );     m_ireg_attr[REG_PMLCA2]      = m_reg_attr["pmlca2"];
        m_reg_attr["pmlca3"]     = &(pmlca3.attr                );     m_ireg_attr[REG_PMLCA3]      = m_reg_attr["pmlca3"];
        m_reg_attr["pmlcb0"]     = &(pmlcb0.attr                );     m_ireg_attr[REG_PMLCB0]      = m_reg_attr["pmlcb0"];
        m_reg_attr["pmlcb1"]     = &(pmlcb1.attr                );     m_ireg_attr[REG_PMLCB1]      = m_reg_attr["pmlcb1"];
        m_reg_attr["pmlcb2"]     = &(pmlcb2.attr                );     m_ireg_attr[REG_PMLCB2]      = m_reg_attr["pmlcb2"];
        m_reg_attr["pmlcb3"]     = &(pmlcb2.attr                );     m_ireg_attr[REG_PMLCB3]      = m_reg_attr["pmlcb3"];
        m_reg_attr["pmgc0"]      = &(pmgc0.attr                 );     m_ireg_attr[REG_PMGC0]       = m_reg_attr["pmgc0"];
        m_reg_attr["upmc0"]      = &(upmc0.attr                 );     m_ireg_attr[REG_UPMC0]       = m_reg_attr["upmc0"];
        m_reg_attr["upmc1"]      = &(upmc1.attr                 );     m_ireg_attr[REG_UPMC1]       = m_reg_attr["upmc1"];
        m_reg_attr["upmc2"]      = &(upmc2.attr                 );     m_ireg_attr[REG_UPMC2]       = m_reg_attr["upmc2"];
        m_reg_attr["upmc3"]      = &(upmc3.attr                 );     m_ireg_attr[REG_UPMC3]       = m_reg_attr["upmc3"];
        m_reg_attr["upmlca0"]    = &(upmlca0.attr               );     m_ireg_attr[REG_UPMLCA0]     = m_reg_attr["upmlca0"];
        m_reg_attr["upmlca1"]    = &(upmlca1.attr               );     m_ireg_attr[REG_UPMLCA1]     = m_reg_attr["upmlca1"];
        m_reg_attr["upmlca2"]    = &(upmlca2.attr               );     m_ireg_attr[REG_UPMLCA2]     = m_reg_attr["upmlca2"];
        m_reg_attr["upmlca3"]    = &(upmlca3.attr               );     m_ireg_attr[REG_UPMLCA3]     = m_reg_attr["upmlca3"];
        m_reg_attr["upmlcb0"]    = &(upmlcb0.attr               );     m_ireg_attr[REG_UPMLCB0]     = m_reg_attr["upmlcb0"];
        m_reg_attr["upmlcb1"]    = &(upmlcb1.attr               );     m_ireg_attr[REG_UPMLCB1]     = m_reg_attr["upmlcb1"];
        m_reg_attr["upmlcb2"]    = &(upmlcb2.attr               );     m_ireg_attr[REG_UPMLCB2]     = m_reg_attr["upmlcb2"];
        m_reg_attr["upmlcb3"]    = &(upmlcb3.attr               );     m_ireg_attr[REG_UPMLCB3]     = m_reg_attr["upmlcb3"];
        m_reg_attr["upmgc0"]     = &(upmgc0.attr                );     m_ireg_attr[REG_UPMGC0]      = m_reg_attr["upmgc0"];

    }


    // Functions for boost::python usage only
    template<int gprno> uint64_t& get_gpr(){
        return gpr[gprno];
    }
};

//
//
//
// python signal callback to respond to any python errors( we can attach a callback with signature int xxx() {} )
struct py_signal_callback {
    typedef int (*callback_ptr)();   // Return type is "int" to return any status indicator
    static callback_ptr callback;
};
py_signal_callback::callback_ptr py_signal_callback::callback = NULL;

#endif
