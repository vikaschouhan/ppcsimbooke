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
#define    REG_TYPE_MSR     0x1
#define    REG_TYPE_CR      0x2
#define    REG_TYPE_ACC     0x3
#define    REG_TYPE_GPR     0x4
#define    REG_TYPE_SPR     0x5
#define    REG_TYPE_PMR     0x6

#define    PPC_NGPRS        32
#define    PPC_NSPRS        1024
#define    PPC_NPMRS        1024

// PPC register ( 64 bit only )
struct ppc_reg64 {
    uint64_t         value;     // value
    const uint64_t   fvalue;    // fvalue is used in case register is read only
    const uint64_t   attr;      // attribute ( permissions etc. )
    const int        indx;      // register index no
    const int        type;      // register type

    // Constructors
    ppc_reg64(uint64_t val, uint64_t attr_, int regno, int type_):
        value(val),
        fvalue(val),
        attr(attr_),
        indx(regno),
        type(type_)
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


// e500v2 PPC register file ( this is the file we are gonna use in our cpu )
struct ppc_regs {
    ppc_reg64         msr;
    ppc_reg64         cr;
    ppc_reg64         acc;

    // GPRs
    ppc_reg64         gpr0;
    ppc_reg64         gpr1;
    ppc_reg64         gpr2;
    ppc_reg64         gpr3;
    ppc_reg64         gpr4;
    ppc_reg64         gpr5;
    ppc_reg64         gpr6;
    ppc_reg64         gpr7;
    ppc_reg64         gpr8;
    ppc_reg64         gpr9;
    ppc_reg64         gpr10;
    ppc_reg64         gpr11;
    ppc_reg64         gpr12;
    ppc_reg64         gpr13;
    ppc_reg64         gpr14;
    ppc_reg64         gpr15;
    ppc_reg64         gpr16;
    ppc_reg64         gpr17;
    ppc_reg64         gpr18;
    ppc_reg64         gpr19;
    ppc_reg64         gpr20;
    ppc_reg64         gpr21;
    ppc_reg64         gpr22;
    ppc_reg64         gpr23;
    ppc_reg64         gpr24;
    ppc_reg64         gpr25;
    ppc_reg64         gpr26;
    ppc_reg64         gpr27;
    ppc_reg64         gpr28;
    ppc_reg64         gpr29;
    ppc_reg64         gpr30;
    ppc_reg64         gpr31;

    // SPRs
    ppc_reg64         atbl;
    ppc_reg64         atbu;
    ppc_reg64         csrr0;
    ppc_reg64         csrr1;
    ppc_reg64         ctr;
    ppc_reg64         dac1;
    ppc_reg64         dac2;
    ppc_reg64         dbcr0;
    ppc_reg64         dbcr1;
    ppc_reg64         dbcr2;
    ppc_reg64         dbsr;
    ppc_reg64         dear;
    ppc_reg64         dec;
    ppc_reg64         decar;
    ppc_reg64         esr;
    ppc_reg64         iac1;
    ppc_reg64         iac2;
    ppc_reg64         ivor0;
    ppc_reg64         ivor1;
    ppc_reg64         ivor2;
    ppc_reg64         ivor3;
    ppc_reg64         ivor4;
    ppc_reg64         ivor5;
    ppc_reg64         ivor6;
    ppc_reg64         ivor7;
    ppc_reg64         ivor8;
    ppc_reg64         ivor9;
    ppc_reg64         ivor10;
    ppc_reg64         ivor11;
    ppc_reg64         ivor12;
    ppc_reg64         ivor13;
    ppc_reg64         ivor14;
    ppc_reg64         ivor15;
    ppc_reg64         ivpr;
    ppc_reg64         lr;
    ppc_reg64         pid0;
    ppc_reg64         pid1;
    ppc_reg64         pid2;
    ppc_reg64         pir;
    ppc_reg64         pvr;
    ppc_reg64         sprg0;
    ppc_reg64         sprg1;
    ppc_reg64         sprg2;
    ppc_reg64         sprg3;
    ppc_reg64         sprg4;
    ppc_reg64         sprg5;
    ppc_reg64         sprg6;
    ppc_reg64         sprg7;
    ppc_reg64         srr0;
    ppc_reg64         srr1;
    ppc_reg64         tbl;
    ppc_reg64         tbu;
    ppc_reg64         tcr;
    ppc_reg64         tsr;
    ppc_reg64         usprg0;
    ppc_reg64         xer;

    ppc_reg64         bbear;
    ppc_reg64         bbtar;
    ppc_reg64         bucsr;
    ppc_reg64         hid0;
    ppc_reg64         hid1;
    ppc_reg64         ivor32;
    ppc_reg64         ivor33;
    ppc_reg64         ivor34;
    ppc_reg64         ivor35;
    ppc_reg64         l1cfg0; 
    ppc_reg64         l1cfg1;
    ppc_reg64         l1csr0;
    ppc_reg64         l1csr1;
    ppc_reg64         mas0;
    ppc_reg64         mas1;
    ppc_reg64         mas2;
    ppc_reg64         mas3;
    ppc_reg64         mas4;
    ppc_reg64         mas5;
    ppc_reg64         mas6;
    ppc_reg64         mas7;
    ppc_reg64         mcar;
    ppc_reg64         mcsr;
    ppc_reg64         mcsrr0;
    ppc_reg64         mcsrr1;
    ppc_reg64         mmucfg;
    ppc_reg64         mmucsr0;
    ppc_reg64         spefscr;
    ppc_reg64         svr;
    ppc_reg64         tlb0cfg;
    ppc_reg64         tlb1cfg;

    // PMRs
    ppc_reg64         pmgc0;
    ppc_reg64         pmlca0;
    ppc_reg64         pmlca1;
    ppc_reg64         pmlca2;
    ppc_reg64         pmlca3;
    ppc_reg64         pmlcb0;
    ppc_reg64         pmlcb1;
    ppc_reg64         pmlcb2;
    ppc_reg64         pmlcb3;
    ppc_reg64         pmc0;
    ppc_reg64         pmc1;
    ppc_reg64         pmc2;
    ppc_reg64         pmc3;

    ppc_reg64         upmgc0;
    ppc_reg64         upmlca0;
    ppc_reg64         upmlca1;
    ppc_reg64         upmlca2;
    ppc_reg64         upmlca3;
    ppc_reg64         upmlcb0;
    ppc_reg64         upmlcb1;
    ppc_reg64         upmlcb2;
    ppc_reg64         upmlcb3;
    ppc_reg64         upmc0;
    ppc_reg64         upmc1;
    ppc_reg64         upmc2;
    ppc_reg64         upmc3;

    // register ptrs
    std::map<std::string, ppc_reg64*>  m_reg;
    std::map<int,         ppc_reg64*>  m_ireg;

    // Constructor
    ppc_regs(uint64_t msr_=0):
        msr     (msr_, (REG_READ_SUP | REG_WRITE_SUP | REG_READ_USR                                  ), 0            , REG_TYPE_MSR),
        cr      (0,    0                                                                              , 0            , REG_TYPE_CR ),
        acc     (0,    0                                                                              , 0            , REG_TYPE_ACC),

        gpr0    (0,    0                                                                              , 0            , REG_TYPE_GPR),
        gpr1    (0,    0                                                                              , 1            , REG_TYPE_GPR),
        gpr2    (0,    0                                                                              , 2            , REG_TYPE_GPR),
        gpr3    (0,    0                                                                              , 3            , REG_TYPE_GPR),
        gpr4    (0,    0                                                                              , 4            , REG_TYPE_GPR),
        gpr5    (0,    0                                                                              , 5            , REG_TYPE_GPR),
        gpr6    (0,    0                                                                              , 6            , REG_TYPE_GPR),
        gpr7    (0,    0                                                                              , 7            , REG_TYPE_GPR),
        gpr8    (0,    0                                                                              , 8            , REG_TYPE_GPR),
        gpr9    (0,    0                                                                              , 9            , REG_TYPE_GPR),
        gpr10   (0,    0                                                                              , 10           , REG_TYPE_GPR),
        gpr11   (0,    0                                                                              , 11           , REG_TYPE_GPR),
        gpr12   (0,    0                                                                              , 12           , REG_TYPE_GPR),
        gpr13   (0,    0                                                                              , 13           , REG_TYPE_GPR),
        gpr14   (0,    0                                                                              , 14           , REG_TYPE_GPR),
        gpr15   (0,    0                                                                              , 15           , REG_TYPE_GPR),
        gpr16   (0,    0                                                                              , 16           , REG_TYPE_GPR),
        gpr17   (0,    0                                                                              , 17           , REG_TYPE_GPR),
        gpr18   (0,    0                                                                              , 18           , REG_TYPE_GPR),
        gpr19   (0,    0                                                                              , 19           , REG_TYPE_GPR),
        gpr20   (0,    0                                                                              , 20           , REG_TYPE_GPR),
        gpr21   (0,    0                                                                              , 21           , REG_TYPE_GPR),
        gpr22   (0,    0                                                                              , 22           , REG_TYPE_GPR),
        gpr23   (0,    0                                                                              , 23           , REG_TYPE_GPR),
        gpr24   (0,    0                                                                              , 24           , REG_TYPE_GPR),
        gpr25   (0,    0                                                                              , 25           , REG_TYPE_GPR),
        gpr26   (0,    0                                                                              , 26           , REG_TYPE_GPR),
        gpr27   (0,    0                                                                              , 27           , REG_TYPE_GPR),
        gpr28   (0,    0                                                                              , 28           , REG_TYPE_GPR),
        gpr29   (0,    0                                                                              , 29           , REG_TYPE_GPR),
        gpr30   (0,    0                                                                              , 30           , REG_TYPE_GPR),
        gpr31   (0,    0                                                                              , 31           , REG_TYPE_GPR),

        atbl    (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR                                  ), SPRN_ATBL    , REG_TYPE_SPR),
        atbu    (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR                                  ), SPRN_ATBU    , REG_TYPE_SPR),
        csrr0   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_CSRR0   , REG_TYPE_SPR),
        csrr1   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_CSRR1   , REG_TYPE_SPR),
        ctr     (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_READ_USR  | REG_WRITE_USR                 ), SPRN_CTR     , REG_TYPE_SPR),
        dac1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DAC1    , REG_TYPE_SPR),
        dac2    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DAC2    , REG_TYPE_SPR),
        dbcr0   (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                                  ), SPRN_DBCR0   , REG_TYPE_SPR),
        dbcr1   (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                                  ), SPRN_DBCR1   , REG_TYPE_SPR),
        dbcr2   (0, (REG_READ_SUP  | REG_WRITE_SUP   | REG_REQ_SYNC                                  ), SPRN_DBCR2   , REG_TYPE_SPR),
        dbsr    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DBSR    , REG_TYPE_SPR),
        dear    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DEAR    , REG_TYPE_SPR),
        dec     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_DEC     , REG_TYPE_SPR),
        decar   (0, (REG_WRITE_SUP                                                                   ), SPRN_DECAR   , REG_TYPE_SPR),
        esr     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_ESR     , REG_TYPE_SPR),
        iac1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IAC1    , REG_TYPE_SPR),
        iac2    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IAC2    , REG_TYPE_SPR),
        ivor0   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR0   , REG_TYPE_SPR),
        ivor1   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR1   , REG_TYPE_SPR),
        ivor2   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR2   , REG_TYPE_SPR),
        ivor3   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR3   , REG_TYPE_SPR),
        ivor4   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR4   , REG_TYPE_SPR),
        ivor5   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR5   , REG_TYPE_SPR),
        ivor6   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR6   , REG_TYPE_SPR),
        ivor7   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR7   , REG_TYPE_SPR),
        ivor8   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR8   , REG_TYPE_SPR),
        ivor9   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR9   , REG_TYPE_SPR),
        ivor10  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR10  , REG_TYPE_SPR),
        ivor11  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR11  , REG_TYPE_SPR),
        ivor12  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR12  , REG_TYPE_SPR),
        ivor13  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR13  , REG_TYPE_SPR),
        ivor14  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR14  , REG_TYPE_SPR),
        ivor15  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR15  , REG_TYPE_SPR),
        ivpr    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVPR    , REG_TYPE_SPR),
        lr      (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR | REG_WRITE_USR                   ), SPRN_LR      , REG_TYPE_SPR),
        pid0    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_PID0    , REG_TYPE_SPR),
        pid1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_PID1    , REG_TYPE_SPR),
        pid2    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_PID2    , REG_TYPE_SPR),
        pir     (0, (REG_READ_SUP                                                                    ), SPRN_PIR     , REG_TYPE_SPR),
        pvr     (0, (REG_READ_SUP                                                                    ), SPRN_PVR     , REG_TYPE_SPR),
        sprg0   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG0   , REG_TYPE_SPR),
        sprg1   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG1   , REG_TYPE_SPR),
        sprg2   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG2   , REG_TYPE_SPR),
        sprg3   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG3   , REG_TYPE_SPR),
        sprg4   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG4   , REG_TYPE_SPR),
        sprg5   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG5   , REG_TYPE_SPR),
        sprg6   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG6   , REG_TYPE_SPR),
        sprg7   (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SPRG7   , REG_TYPE_SPR),
        srr0    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SRR0    , REG_TYPE_SPR),
        srr1    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_SRR1    , REG_TYPE_SPR),
        tbl     (0, (REG_READ_USR                                                                    ), SPRN_TBWL    , REG_TYPE_SPR),
        tbu     (0, (REG_WRITE_SUP                                                                   ), SPRN_TBWU    , REG_TYPE_SPR),
        tcr     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_TCR     , REG_TYPE_SPR),
        tsr     (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_TSR     , REG_TYPE_SPR),
        usprg0  (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR                  ), SPRN_USPRG0  , REG_TYPE_SPR),
        xer     (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR                  ), SPRN_XER     , REG_TYPE_SPR),

        bbear   (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR   | REG_REQ_SYNC ), SPRN_BBEAR   , REG_TYPE_SPR),
        bbtar   (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_READ_USR  | REG_WRITE_USR   | REG_REQ_SYNC ), SPRN_BBTAR   , REG_TYPE_SPR),
        bucsr   (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_BUCSR   , REG_TYPE_SPR),
        hid0    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_HID0    , REG_TYPE_SPR),
        hid1    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_HID1    , REG_TYPE_SPR),
        ivor32  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR32  , REG_TYPE_SPR),
        ivor33  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR33  , REG_TYPE_SPR),
        ivor34  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR34  , REG_TYPE_SPR),
        ivor35  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_IVOR35  , REG_TYPE_SPR),
        l1cfg0  (0, (REG_READ_SUP                                                                    ), SPRN_L1CFG0  , REG_TYPE_SPR),
        l1cfg1  (0, (REG_READ_SUP                                                                    ), SPRN_L1CFG1  , REG_TYPE_SPR),
        l1csr0  (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_L1CSR0  , REG_TYPE_SPR),
        l1csr1  (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_L1CSR1  , REG_TYPE_SPR),
        mas0    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS0    , REG_TYPE_SPR),
        mas1    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS1    , REG_TYPE_SPR),
        mas2    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS2    , REG_TYPE_SPR),
        mas3    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS3    , REG_TYPE_SPR),
        mas4    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS4    , REG_TYPE_SPR),
        mas5    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS5    , REG_TYPE_SPR),
        mas6    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS6    , REG_TYPE_SPR),
        mas7    (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MAS7    , REG_TYPE_SPR),
        mcar    (0, (REG_READ_SUP                                                                    ), SPRN_MCAR    , REG_TYPE_SPR),
        mcsr    (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_MCSR    , REG_TYPE_SPR),
        mcsrr0  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_MCSRR0  , REG_TYPE_SPR),
        mcsrr1  (0, (REG_READ_SUP  | REG_WRITE_SUP                                                   ), SPRN_MCSRR1  , REG_TYPE_SPR),
        mmucfg  (0, (REG_READ_SUP                                                                    ), SPRN_MMUCFG  , REG_TYPE_SPR),
        mmucsr0 (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_MMUCSR0 , REG_TYPE_SPR),
        spefscr (0, (REG_READ_SUP  | REG_WRITE_SUP  | REG_REQ_SYNC                                   ), SPRN_SPEFSCR , REG_TYPE_SPR),
        svr     (0, (REG_READ_SUP                                                                    ), SPRN_SVR     , REG_TYPE_SPR),
        tlb0cfg (0, (REG_READ_SUP                                                                    ), SPRN_TLB0CFG , REG_TYPE_SPR),
        tlb1cfg (0, (REG_READ_SUP                                                                    ), SPRN_TLB1CFG , REG_TYPE_SPR),

        pmgc0   (0, 0, PMRN_PMGC0   , REG_TYPE_PMR),
        pmlca0  (0, 0, PMRN_PMLCA0  , REG_TYPE_PMR),
        pmlca1  (0, 0, PMRN_PMLCA1  , REG_TYPE_PMR),
        pmlca2  (0, 0, PMRN_PMLCA2  , REG_TYPE_PMR),
        pmlca3  (0, 0, PMRN_PMLCA3  , REG_TYPE_PMR),
        pmlcb0  (0, 0, PMRN_PMLCB0  , REG_TYPE_PMR),
        pmlcb1  (0, 0, PMRN_PMLCB1  , REG_TYPE_PMR),
        pmlcb2  (0, 0, PMRN_PMLCB2  , REG_TYPE_PMR),
        pmlcb3  (0, 0, PMRN_PMLCB3  , REG_TYPE_PMR),
        pmc0    (0, 0, PMRN_PMC0    , REG_TYPE_PMR),
        pmc1    (0, 0, PMRN_PMC1    , REG_TYPE_PMR),
        pmc2    (0, 0, PMRN_PMC2    , REG_TYPE_PMR),
        pmc3    (0, 0, PMRN_PMC3    , REG_TYPE_PMR),
        upmgc0  (0, 0, PMRN_UPMGC0  , REG_TYPE_PMR),
        upmlca0 (0, 0, PMRN_UPMLCA0 , REG_TYPE_PMR),
        upmlca1 (0, 0, PMRN_UPMLCA1 , REG_TYPE_PMR),
        upmlca2 (0, 0, PMRN_UPMLCA2 , REG_TYPE_PMR),
        upmlca3 (0, 0, PMRN_UPMLCA3 , REG_TYPE_PMR),
        upmlcb0 (0, 0, PMRN_UPMLCB0 , REG_TYPE_PMR),
        upmlcb1 (0, 0, PMRN_UPMLCB1 , REG_TYPE_PMR),
        upmlcb2 (0, 0, PMRN_UPMLCB2 , REG_TYPE_PMR),
        upmlcb3 (0, 0, PMRN_UPMLCB3 , REG_TYPE_PMR),
        upmc0   (0, 0, PMRN_UPMC0   , REG_TYPE_PMR),
        upmc1   (0, 0, PMRN_UPMC1   , REG_TYPE_PMR),
        upmc2   (0, 0, PMRN_UPMC2   , REG_TYPE_PMR),
        upmc3   (0, 0, PMRN_UPMC2   , REG_TYPE_PMR)
    {
        // Initialize value ptrs
        m_reg["msr"]        = &(msr     );      m_ireg[REG_MSR]         = m_reg["msr"];
        m_reg["cr"]         = &(cr      );      m_ireg[REG_CR]          = m_reg["cr"];
        m_reg["acc"]        = &(acc     );      m_ireg[REG_ACC]         = m_reg["acc"];

        m_reg["atbl"]       = &(atbl    );      m_ireg[REG_ATBL]        = m_reg["atbl"];
        m_reg["atbu"]       = &(atbu    );      m_ireg[REG_ATBU]        = m_reg["atbu"];
        m_reg["csrr0"]      = &(csrr0   );      m_ireg[REG_CSRR0]       = m_reg["csrr0"];
        m_reg["csrr1"]      = &(csrr1   );      m_ireg[REG_CSRR1]       = m_reg["csrr1"];
        m_reg["ctr"]        = &(ctr     );      m_ireg[REG_CTR]         = m_reg["ctr"];
        m_reg["dac1"]       = &(dac1    );      m_ireg[REG_DAC1]        = m_reg["dac1"];
        m_reg["dac2"]       = &(dac2    );      m_ireg[REG_DAC2]        = m_reg["dac2"];
        m_reg["dbcr0"]      = &(dbcr0   );      m_ireg[REG_DBCR0]       = m_reg["dbcr0"];
        m_reg["dbcr1"]      = &(dbcr1   );      m_ireg[REG_DBCR1]       = m_reg["dbcr1"];
        m_reg["dbcr2"]      = &(dbcr2   );      m_ireg[REG_DBCR2]       = m_reg["dbcr2"];
        m_reg["dbsr"]       = &(dbsr    );      m_ireg[REG_DBSR]        = m_reg["dbsr"];
        m_reg["dear"]       = &(dear    );      m_ireg[REG_DEAR]        = m_reg["dear"];
        m_reg["dec"]        = &(dec     );      m_ireg[REG_DEC]         = m_reg["dec"];
        m_reg["decar"]      = &(decar   );      m_ireg[REG_DECAR]       = m_reg["decar"];
        m_reg["esr"]        = &(esr     );      m_ireg[REG_ESR]         = m_reg["esr"];
        m_reg["iac1"]       = &(iac1    );      m_ireg[REG_IAC1]        = m_reg["iac1"];
        m_reg["iac2"]       = &(iac2    );      m_ireg[REG_IAC2]        = m_reg["iac2"];
        m_reg["ivor0"]      = &(ivor0   );      m_ireg[REG_IVOR0]       = m_reg["ivor0"];
        m_reg["ivor1"]      = &(ivor1   );      m_ireg[REG_IVOR1]       = m_reg["ivor1"]; 
        m_reg["ivor2"]      = &(ivor2   );      m_ireg[REG_IVOR2]       = m_reg["ivor2"];
        m_reg["ivor3"]      = &(ivor3   );      m_ireg[REG_IVOR3]       = m_reg["ivor3"];
        m_reg["ivor4"]      = &(ivor4   );      m_ireg[REG_IVOR4]       = m_reg["ivor4"];
        m_reg["ivor5"]      = &(ivor5   );      m_ireg[REG_IVOR5]       = m_reg["ivor5"];
        m_reg["ivor6"]      = &(ivor6   );      m_ireg[REG_IVOR6]       = m_reg["ivor6"];
        m_reg["ivor7"]      = &(ivor7   );      m_ireg[REG_IVOR7]       = m_reg["ivor7"];
        m_reg["ivor8"]      = &(ivor8   );      m_ireg[REG_IVOR8]       = m_reg["ivor8"];
        m_reg["ivor9"]      = &(ivor9   );      m_ireg[REG_IVOR9]       = m_reg["ivor9"];
        m_reg["ivor10"]     = &(ivor10  );      m_ireg[REG_IVOR10]      = m_reg["ivor10"];
        m_reg["ivor11"]     = &(ivor11  );      m_ireg[REG_IVOR11]      = m_reg["ivor11"];
        m_reg["ivor12"]     = &(ivor12  );      m_ireg[REG_IVOR12]      = m_reg["ivor12"];
        m_reg["ivor13"]     = &(ivor13  );      m_ireg[REG_IVOR13]      = m_reg["ivor13"];
        m_reg["ivor14"]     = &(ivor14  );      m_ireg[REG_IVOR14]      = m_reg["ivor14"];
        m_reg["ivor15"]     = &(ivor15  );      m_ireg[REG_IVOR15]      = m_reg["ivor15"];
        m_reg["ivpr"]       = &(ivpr    );      m_ireg[REG_IVPR]        = m_reg["ivpr"];
        m_reg["lr"]         = &(lr      );      m_ireg[REG_LR]          = m_reg["lr"];
        m_reg["pid"]        = &(pid0    );      m_ireg[REG_PID]         = m_reg["pid"];
        m_reg["pir"]        = &(pir     );      m_ireg[REG_PIR]         = m_reg["pir"];
        m_reg["pvr"]        = &(pvr     );      m_ireg[REG_PVR]         = m_reg["pvr"];
        m_reg["sprg0"]      = &(sprg0   );      m_ireg[REG_SPRG0]       = m_reg["sprg0"];
        m_reg["sprg1"]      = &(sprg1   );      m_ireg[REG_SPRG1]       = m_reg["sprg1"];
        m_reg["sprg2"]      = &(sprg2   );      m_ireg[REG_SPRG2]       = m_reg["sprg2"];
        m_reg["sprg3r"]     = &(sprg3   );      m_ireg[REG_SPRG3R]      = m_reg["sprg3r"];
        m_reg["sprg3"]      = &(sprg3   );      m_ireg[REG_SPRG3]       = m_reg["sprg3"];
        m_reg["sprg4r"]     = &(sprg4   );      m_ireg[REG_SPRG4R]      = m_reg["sprg4r"];
        m_reg["sprg4"]      = &(sprg4   );      m_ireg[REG_SPRG4]       = m_reg["sprg4"];
        m_reg["sprg5r"]     = &(sprg5   );      m_ireg[REG_SPRG5R]      = m_reg["sprg5r"];
        m_reg["sprg5"]      = &(sprg5   );      m_ireg[REG_SPRG5]       = m_reg["sprg5"];
        m_reg["sprg6r"]     = &(sprg6   );      m_ireg[REG_SPRG6R]      = m_reg["sprg6r"];
        m_reg["sprg6"]      = &(sprg6   );      m_ireg[REG_SPRG6]       = m_reg["sprg6"];
        m_reg["sprg7r"]     = &(sprg7   );      m_ireg[REG_SPRG7R]      = m_reg["sprg7r"];
        m_reg["sprg7"]      = &(sprg7   );      m_ireg[REG_SPRG7]       = m_reg["sprg7"];
        m_reg["srr0"]       = &(srr0    );      m_ireg[REG_SRR0]        = m_reg["srr0"];
        m_reg["srr1"]       = &(srr1    );      m_ireg[REG_SRR1]        = m_reg["srr1"];
        m_reg["tbrl"]       = &(tbl     );      m_ireg[REG_TBRL]        = m_reg["tbrl"];
        m_reg["tbwl"]       = &(tbl     );      m_ireg[REG_TBWL]        = m_reg["tbwl"];
        m_reg["tbru"]       = &(tbu     );      m_ireg[REG_TBRU]        = m_reg["tbru"];
        m_reg["tbwu"]       = &(tbu     );      m_ireg[REG_TBWU]        = m_reg["tbwu"];
        m_reg["tcr"]        = &(tcr     );      m_ireg[REG_TCR]         = m_reg["tcr"];
        m_reg["tsr"]        = &(tsr     );      m_ireg[REG_TSR]         = m_reg["tsr"];
        m_reg["usprg0"]     = &(usprg0  );      m_ireg[REG_USPRG0]      = m_reg["usprg0"];
        m_reg["xer"]        = &(xer     );      m_ireg[REG_XER]         = m_reg["xer"];

        m_reg["bbear"]      = &(bbear    );     m_ireg[REG_BBEAR]       = m_reg["bbear"];
        m_reg["bbtar"]      = &(bbtar    );     m_ireg[REG_BBTAR]       = m_reg["bbtar"];
        m_reg["bucsr"]      = &(bucsr    );     m_ireg[REG_BUCSR]       = m_reg["bucsr"];
        m_reg["hid0"]       = &(hid0     );     m_ireg[REG_HID0]        = m_reg["hid0"];
        m_reg["hid1"]       = &(hid1     );     m_ireg[REG_HID1]        = m_reg["hid1"];
        m_reg["ivor32"]     = &(ivor32   );     m_ireg[REG_IVOR32]      = m_reg["ivor32"];
        m_reg["ivor33"]     = &(ivor33   );     m_ireg[REG_IVOR33]      = m_reg["ivor33"];
        m_reg["ivor34"]     = &(ivor34   );     m_ireg[REG_IVOR34]      = m_reg["ivor34"];
        m_reg["ivor35"]     = &(ivor35   );     m_ireg[REG_IVOR35]      = m_reg["ivor35"];
        m_reg["l1cfg0"]     = &(l1cfg0   );     m_ireg[REG_L1CFG0]      = m_reg["l1cfg0"];
        m_reg["l1cfg1"]     = &(l1cfg1   );     m_ireg[REG_L1CFG1]      = m_reg["l1cfg1"];
        m_reg["l1csr0"]     = &(l1csr0   );     m_ireg[REG_L1CSR0]      = m_reg["l1csr0"];
        m_reg["l1csr1"]     = &(l1csr1   );     m_ireg[REG_L1CSR1]      = m_reg["l1csr1"];
        m_reg["mas0"]       = &(mas0     );     m_ireg[REG_MAS0]        = m_reg["mas0"];
        m_reg["mas1"]       = &(mas1     );     m_ireg[REG_MAS1]        = m_reg["mas1"];
        m_reg["mas2"]       = &(mas2     );     m_ireg[REG_MAS2]        = m_reg["mas2"];
        m_reg["mas3"]       = &(mas3     );     m_ireg[REG_MAS3]        = m_reg["mas3"];
        m_reg["mas4"]       = &(mas4     );     m_ireg[REG_MAS4]        = m_reg["mas4"];
        m_reg["mas5"]       = &(mas5     );     m_ireg[REG_MAS5]        = m_reg["mas5"];
        m_reg["mas6"]       = &(mas6     );     m_ireg[REG_MAS6]        = m_reg["mas6"];
        m_reg["mas7"]       = &(mas7     );     m_ireg[REG_MAS7]        = m_reg["mas7"];
        m_reg["mcar"]       = &(mcar     );     m_ireg[REG_MCAR]        = m_reg["mcar"];
        m_reg["mcsr"]       = &(mcsr     );     m_ireg[REG_MCSR]        = m_reg["mcsr"];
        m_reg["mcsrr0"]     = &(mcsrr0   );     m_ireg[REG_MCSRR0]      = m_reg["mcsrr0"];
        m_reg["mcsrr1"]     = &(mcsrr1   );     m_ireg[REG_MCSRR1]      = m_reg["mcsrr1"];
        m_reg["mmucfg"]     = &(mmucfg   );     m_ireg[REG_MMUCFG]      = m_reg["mmucfg"];
        m_reg["mmucsr0"]    = &(mmucsr0  );     m_ireg[REG_MMUCSR0]     = m_reg["mmucsr0"];
        m_reg["pid0"]       = &(pid0     );     m_ireg[REG_PID0]        = m_reg["pid0"];
        m_reg["pid1"]       = &(pid1     );     m_ireg[REG_PID1]        = m_reg["pid1"];
        m_reg["pid2"]       = &(pid2     );     m_ireg[REG_PID2]        = m_reg["pid2"];
        m_reg["spefscr"]    = &(spefscr  );     m_ireg[REG_SPEFSCR]     = m_reg["spefscr"];
        m_reg["svr"]        = &(svr      );     m_ireg[REG_SVR]         = m_reg["svr"];
        m_reg["tlb0cfg"]    = &(tlb0cfg  );     m_ireg[REG_TLB0CFG]     = m_reg["tlb0cfg"];
        m_reg["tlb1cfg"]    = &(tlb1cfg  );     m_ireg[REG_TLB1CFG]     = m_reg["tlb1cfg"];

        // PMRs
        m_reg["pmc0"]       = &(pmc0     );     m_ireg[REG_PMC0]        = m_reg["pmc0"];
        m_reg["pmc1"]       = &(pmc1     );     m_ireg[REG_PMC1]        = m_reg["pmc1"];
        m_reg["pmc2"]       = &(pmc2     );     m_ireg[REG_PMC2]        = m_reg["pmc2"];
        m_reg["pmc3"]       = &(pmc3     );     m_ireg[REG_PMC3]        = m_reg["pmc3"];
        m_reg["pmlca0"]     = &(pmlca0   );     m_ireg[REG_PMLCA0]      = m_reg["pmlca0"];
        m_reg["pmlca1"]     = &(pmlca1   );     m_ireg[REG_PMLCA1]      = m_reg["pmlca1"];
        m_reg["pmlca2"]     = &(pmlca2   );     m_ireg[REG_PMLCA2]      = m_reg["pmlca2"];
        m_reg["pmlca3"]     = &(pmlca3   );     m_ireg[REG_PMLCA3]      = m_reg["pmlca3"];
        m_reg["pmlcb0"]     = &(pmlcb0   );     m_ireg[REG_PMLCB0]      = m_reg["pmlcb0"];
        m_reg["pmlcb1"]     = &(pmlcb1   );     m_ireg[REG_PMLCB1]      = m_reg["pmlcb1"];
        m_reg["pmlcb2"]     = &(pmlcb2   );     m_ireg[REG_PMLCB2]      = m_reg["pmlcb2"];
        m_reg["pmlcb3"]     = &(pmlcb2   );     m_ireg[REG_PMLCB3]      = m_reg["pmlcb3"];
        m_reg["pmgc0"]      = &(pmgc0    );     m_ireg[REG_PMGC0]       = m_reg["pmgc0"];
        m_reg["upmc0"]      = &(upmc0    );     m_ireg[REG_UPMC0]       = m_reg["upmc0"];
        m_reg["upmc1"]      = &(upmc1    );     m_ireg[REG_UPMC1]       = m_reg["upmc1"];
        m_reg["upmc2"]      = &(upmc2    );     m_ireg[REG_UPMC2]       = m_reg["upmc2"];
        m_reg["upmc3"]      = &(upmc3    );     m_ireg[REG_UPMC3]       = m_reg["upmc3"];
        m_reg["upmlca0"]    = &(upmlca0  );     m_ireg[REG_UPMLCA0]     = m_reg["upmlca0"];
        m_reg["upmlca1"]    = &(upmlca1  );     m_ireg[REG_UPMLCA1]     = m_reg["upmlca1"];
        m_reg["upmlca2"]    = &(upmlca2  );     m_ireg[REG_UPMLCA2]     = m_reg["upmlca2"];
        m_reg["upmlca3"]    = &(upmlca3  );     m_ireg[REG_UPMLCA3]     = m_reg["upmlca3"];
        m_reg["upmlcb0"]    = &(upmlcb0  );     m_ireg[REG_UPMLCB0]     = m_reg["upmlcb0"];
        m_reg["upmlcb1"]    = &(upmlcb1  );     m_ireg[REG_UPMLCB1]     = m_reg["upmlcb1"];
        m_reg["upmlcb2"]    = &(upmlcb2  );     m_ireg[REG_UPMLCB2]     = m_reg["upmlcb2"];
        m_reg["upmlcb3"]    = &(upmlcb3  );     m_ireg[REG_UPMLCB3]     = m_reg["upmlcb3"];
        m_reg["upmgc0"]     = &(upmgc0   );     m_ireg[REG_UPMGC0]      = m_reg["upmgc0"];

        // GPRS
        m_reg["r0"]         = &(gpr0);
        m_reg["r1"]         = &(gpr1);
        m_reg["r2"]         = &(gpr2);
        m_reg["r3"]         = &(gpr3);
        m_reg["r4"]         = &(gpr4);
        m_reg["r5"]         = &(gpr5);
        m_reg["r6"]         = &(gpr6);
        m_reg["r7"]         = &(gpr7);
        m_reg["r8"]         = &(gpr8);
        m_reg["r9"]         = &(gpr9);
        m_reg["r10"]        = &(gpr10);
        m_reg["r11"]        = &(gpr11);
        m_reg["r12"]        = &(gpr12);
        m_reg["r13"]        = &(gpr13);
        m_reg["r14"]        = &(gpr14);
        m_reg["r15"]        = &(gpr15);
        m_reg["r16"]        = &(gpr16);
        m_reg["r17"]        = &(gpr17);
        m_reg["r18"]        = &(gpr18);
        m_reg["r19"]        = &(gpr19);
        m_reg["r20"]        = &(gpr20);
        m_reg["r21"]        = &(gpr21);
        m_reg["r22"]        = &(gpr22);
        m_reg["r23"]        = &(gpr23);
        m_reg["r24"]        = &(gpr24);
        m_reg["r25"]        = &(gpr25);
        m_reg["r26"]        = &(gpr26);
        m_reg["r27"]        = &(gpr27);
        m_reg["r28"]        = &(gpr28);
        m_reg["r29"]        = &(gpr29);
        m_reg["r30"]        = &(gpr30);
        m_reg["r31"]        = &(gpr31);

        for (size_t i=0; i<PPC_NGPRS; i++){
            m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "gpr" << i))->str()]
                = m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "r" << i))->str()];
            m_ireg[REG_GPR0 + i]
                = m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "r" << i))->str()];
        }
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
