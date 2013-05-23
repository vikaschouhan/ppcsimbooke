#include "globals.h"

///////////////////////////////////////////////////////////////////////////////////////////
// instruction call frame
//////////////////////////////////////////////////////////////////////////////////////////

ppcsimbooke::instr_call::instr_call(){
    opc     = 0;
    hv      = 0;
    nargs   = 0;
    for(int i=0; i<N_IC_ARGS; i++){
        arg[i].v = arg[i].p = arg[i].t = 0;
    }
}

// Dump state
void ppcsimbooke::instr_call::dump_state() const{
    std::cout << "name   : " << opcname << std::endl;
    std::cout << "opcode : " << std::hex << std::showbase << opc << std::endl;
    std::cout << "hash   : " << std::hex << std::showbase << hv << std::endl;
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
void ppcsimbooke::instr_call::print_instr() const{
    std::string lfmt = fmt;
    for(int i=nargs; i<N_IC_ARGS; i++){
        lfmt += "%c";
    }
    lfmt += "\n";
    // We use printf for printing , since we have format of arguments in a string.
    if(opcname == "")
        printf(".long 0x%lx\n", (arg[0].v & 0xffffffff));
    else
        printf(lfmt.c_str(), opcname.c_str(), arg[0].v, arg[1].v, arg[2].v, arg[3].v, arg[4].v, arg[5].v);
}


// Get instruction representation in string form ( Used for DEBUG logs )
char* ppcsimbooke::instr_call::get_instr_str() const{
    static char instr_str[100];
    std::string lfmt = fmt;
    for(int i=nargs; i<N_IC_ARGS; i++){
        lfmt += "%c";
    }
    lfmt += " ";
    // We use printf for printing , since we have format of arguments in a string.
    if(opcname == "")
        sprintf(instr_str, ".long 0x%lx ", (arg[0].v & 0xffffffff));
    else
        sprintf(instr_str, lfmt.c_str(), opcname.c_str(), arg[0].v, arg[1].v, arg[2].v, arg[3].v, arg[4].v, arg[5].v);
    return instr_str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// register_file member functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
ppcsimbooke::ppc_regs::ppc_regs(bool c_m, uint64_t pc_):
    cm(c_m),

    pc(pc_), nip(pc_),

    msr     (0,    (REG_READ_SUP | REG_WRITE_SUP | REG_READ_USR                                  ), 0            , REG_TYPE_MSR),
    cr      (0,    0                                                                              , 0            , REG_TYPE_CR ),
    acc     (0,    0                                                                              , 0            , REG_TYPE_ACC),

    gpr{ ppc_reg64(0,    0                                                                              , 0            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 1            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 2            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 3            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 4            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 5            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 6            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 7            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 8            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 9            , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 10           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 11           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 12           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 13           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 14           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 15           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 16           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 17           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 18           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 19           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 20           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 21           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 22           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 23           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 24           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 25           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 26           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 27           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 28           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 29           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 30           , REG_TYPE_GPR),
         ppc_reg64(0,    0                                                                              , 31           , REG_TYPE_GPR),
       },

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
    m_reg["r0"]         = &(gpr[0]);
    m_reg["r1"]         = &(gpr[1]);
    m_reg["r2"]         = &(gpr[2]);
    m_reg["r3"]         = &(gpr[3]);
    m_reg["r4"]         = &(gpr[4]);
    m_reg["r5"]         = &(gpr[5]);
    m_reg["r6"]         = &(gpr[6]);
    m_reg["r7"]         = &(gpr[7]);
    m_reg["r8"]         = &(gpr[8]);
    m_reg["r9"]         = &(gpr[9]);
    m_reg["r10"]        = &(gpr[10]);
    m_reg["r11"]        = &(gpr[11]);
    m_reg["r12"]        = &(gpr[12]);
    m_reg["r13"]        = &(gpr[13]);
    m_reg["r14"]        = &(gpr[14]);
    m_reg["r15"]        = &(gpr[15]);
    m_reg["r16"]        = &(gpr[16]);
    m_reg["r17"]        = &(gpr[17]);
    m_reg["r18"]        = &(gpr[18]);
    m_reg["r19"]        = &(gpr[19]);
    m_reg["r20"]        = &(gpr[20]);
    m_reg["r21"]        = &(gpr[21]);
    m_reg["r22"]        = &(gpr[22]);
    m_reg["r23"]        = &(gpr[23]);
    m_reg["r24"]        = &(gpr[24]);
    m_reg["r25"]        = &(gpr[25]);
    m_reg["r26"]        = &(gpr[26]);
    m_reg["r27"]        = &(gpr[27]);
    m_reg["r28"]        = &(gpr[28]);
    m_reg["r29"]        = &(gpr[29]);
    m_reg["r30"]        = &(gpr[30]);
    m_reg["r31"]        = &(gpr[31]);

    for (size_t i=0; i<PPC_NGPRS; i++){
        m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "gpr" << i))->str()]
            = m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "r" << i))->str()];
        m_ireg[REG_GPR0 + i]
            = m_reg[static_cast<std::ostringstream *>(&(std::ostringstream() << "r" << i))->str()];
    }

    // set some default values
    msr |= (c_m) << RSHIFT<MSR_CM>::VAL;
}

// Update CR0
void ppcsimbooke::ppc_regs::update_cr0(uint64_t value){
    int c;

    if unlikely(cm) {
        if ((int64_t)value < 0)
            c = 8;
        else if ((int64_t)value > 0)
            c = 4;
        else
            c = 2;
    } else {
        if ((int32_t)value < 0)
            c = 8;
        else if ((int32_t)value > 0)
            c = 4;
        else
            c = 2;
    }

    /*  SO bit, copied from XER:  */
    c |= (xer >> 31) & 1;

    cr &= ~((uint32_t)0xf << 28);
    cr |= ((uint32_t)c << 28);
}

// Update CR0 using host flags
void ppcsimbooke::ppc_regs::update_cr0_host(x86_flags &hf){
    int c = 0;

    if(hf.sf){ c = 8;  }          // If sign flag, set cr0[lt] flag
    else     { c |= 4; }          // else set cr0[gt] flag
    if(hf.zf){ c |= 2; }          // if zero flag, set cr0[eq] flag

    /*  SO bit, copied from XER:  */
    c |= (xer >> 31) & 1;

    cr &= ~((uint32_t)0xf << 28);
    cr |= ((uint32_t)c << 28);
}

// Updates CR[bf] with val i.e CR[bf] <- (val & 0xf)
// bf may range from [0:7]
void ppcsimbooke::ppc_regs::update_crF(unsigned bf, unsigned val){
    bf &= 0x7;
    val &= 0xf;
    cr &= ~( 0xf << (7 - bf)*4 );
    cr |=  ((val & 0xf) << (7 - bf)*4);
}

// Get crF  ( F -> [0:7] )
unsigned ppcsimbooke::ppc_regs::get_crF(unsigned bf){
    return (cr >> (7 - bf)*4) & 0xf;
}

// Update CR by exact field value [0:31]
void ppcsimbooke::ppc_regs::update_crf(unsigned field, bool value){
    field &= 0x1f;
    value &= 0x1;
    cr &= ~(0x1 << (31 - field));
    cr |= (value << (31 - field));
}

// Get CR bit at exact field
bool ppcsimbooke::ppc_regs::get_crf(unsigned field){
    return (cr >> (31 - field)) & 0x1;
}

void ppcsimbooke::ppc_regs::update_xerF(unsigned bf, unsigned val){
    bf &= 0x7;
    val &= 0xf;
    xer &= ~( 0xf << (7 - bf)*4 );
    xer |=  ((val & 0xf) << (7 - bf)*4);
}

unsigned ppcsimbooke::ppc_regs::get_xerF(unsigned bf){
    return (xer >> (7 - bf)*4) & 0xf;
}

void ppcsimbooke::ppc_regs::update_xerf(unsigned field, bool value){
    field &= 0x1f;
    value &= 0x1;
    xer &= ~(0x1 << (31 - field));
    xer |= (value << (31 - field));
}

bool ppcsimbooke::ppc_regs::get_xerf(unsigned field){
    return (xer >> (31 - field)) & 0x1;
}

// Update XER[SO] & XER[OV] by value.
// so_ov is a 2 bit value with ((XER[SO] << 1) | XER[OV])
// NOTE : since XER[32:35] = { SO, OV, CA, RESERVED } , hence XER = XER | (so_ov << 30)
void ppcsimbooke::ppc_regs::update_xer_so_ov(uint8_t so_ov){
    xer &= ~((uint32_t)0x3 << 30);
    xer |= (static_cast<uint64_t>(so_ov) << 30);
}

// so_ov=0x3 if x86_flags::of=1
void ppcsimbooke::ppc_regs::update_xer_so_ov_host(x86_flags &hf){
    uint64_t v = (hf.of) ? 0x3:0;
    xer &= ~((uint32_t)0x3 << 30);
    xer |= (v << 30);
}

// Update XER[CA]
void ppcsimbooke::ppc_regs::update_xer_ca(bool value){
    uint32_t val = (value) ? 1:0;
    xer &= XER_CA;                              // clear XER[CA]
    xer |= val << RSHIFT<XER_CA>::VAL;          // Insert value into XER[CA]
}

// Update XER[CA] using host flags
void ppcsimbooke::ppc_regs::update_xer_ca_host(x86_flags &hf){
    xer &= XER_CA;                                                  // clear XER[CA]
    xer |= static_cast<uint64_t>(hf.cf) << RSHIFT<XER_CA>::VAL;     // Insert value into XER[CA]
}

// Get XER[SO]
bool ppcsimbooke::ppc_regs::get_xer_so(){
    return (xer & XER_SO) ? 1:0;
}

bool ppcsimbooke::ppc_regs::get_xer_ca(){
    return (xer & XER_CA) ? 1:0;
}

bool ppcsimbooke::ppc_regs::get_xer_ov(){
    return (xer & XER_OV) ? 1:0;
}

// Exception Conditions
// ==========================
// 0. Denormalized Input Values :-
//            Truncated to a properly signed zero value. (taken care in x86_mxcsr constructor)
// 1. Overflow :-
//            Result is set to pmax/nmax (+/-) if exception is masked. If exception is not masked, interrupt is taken
//            & destination is not updated.
// 2. Underflow :-
//            Result is set to +0/-0 (+/-) if exception is masked. If exception is not masked, interrupt is taken
//            & destination is not updated.
// 3. Invalid operation / Input errors :-
//            If any input is inf, denorm or NaN or both inputs are +/-0 (FP divide only), FINV[H]=1,FG[H]=1, FX[H]=1.
//            If exception is not masked, an interrupt is taken & destination is not updated.
// 4. Round/Inexact :-
//            If the result is inexact/overflows (overflow exceptions disabled) or underflows (underflow exceptions
//            disabled), FINX[H]=1. If inexact exception is unmasked, an interrupt occurs & destination is updated with
//            truncated results. FG[H]/FX[H] bits are properly updated.
//            FG[H]/FX[H] are set to zero, if interrupt is taken due to overflow/underflow or if invalid operation/input error
//            is signalled.
// 5. Divide by zero :-
//            If an invalid operation/input error doesn't happen, divisor is (+/-)0 & divident is a non-zero normalized no,
//            FDBZ[H]=1. If exception is unmasked, an interrupt is taken.
// ==========================
void ppcsimbooke::ppc_regs::update_spefscr(x86_mxcsr &hf, bool high){
    uint32_t m = hf.v;

    // check for errors
    if likely(m & X86_MXCSR_E){
        return;
    }

    // Input error condition
    if(m & X86_MXCSR_IE){
        if(high) { spefscr |= SPEFSCR_FINVH; spefscr &= ~(SPEFSCR_FGH | SPEFSCR_FXH); }
        else     { spefscr |= SPEFSCR_FINV;  spefscr &= ~(SPEFSCR_FG | SPEFSCR_FX);   }
        spefscr |= SPEFSCR_FINVS;

        if(spefscr & SPEFSCR_FINVE){
            // throw ppc exception
        }
    }

    // divide by zero error condition
    if(m & X86_MXCSR_ZE){
        if(high) { spefscr |= SPEFSCR_FDBZH; }
        else     { spefscr |= SPEFSCR_FDBZ;  }
        spefscr |= SPEFSCR_FDBZS;

        if(spefscr & SPEFSCR_FDBZE){
            // throw ppc exception
        }
    }

    // Inexact/Rounding error
    if(m & X86_MXCSR_PE){
        spefscr |= SPEFSCR_FINXS;

        if(spefscr &SPEFSCR_FINXE){
            // throw ppc exception
        }
    }

    // overflow
    if(m & X86_MXCSR_OE){
        if(high) { spefscr |= SPEFSCR_FOVFH; }
        else     { spefscr |= SPEFSCR_FOVF;  }

        if(spefscr & SPEFSCR_FOVFE){
            // throw ppc exception
        }
    }

    // underflow
    if(m & X86_MXCSR_UE){
        if(high) { spefscr |= SPEFSCR_FUNFH; }
        else     { spefscr |= SPEFSCR_FUNF;  }

        if(spefscr & SPEFSCR_FUNFE){
            // throw exception
        }
    }

    // clear all non sticky flags of SPEFSCR
    spefscr &= ~(SPEFSCR_FINVH | SPEFSCR_FINV | SPEFSCR_FGH | SPEFSCR_FG | SPEFSCR_FXH | SPEFSCR_FX |
                 SPEFSCR_FDBZH | SPEFSCR_FDBZ | SPEFSCR_FOVFH | SPEFSCR_FOVF | SPEFSCR_FUNFH |
                 SPEFSCR_FUNF);

    // clear all sticky flags of host's mxcsr register
    // NOTE : x86 doesn't have intermediate flags for each instruction. Hence we track
    //        SSE fpu exceptions using the sticky flags.
    hf.clear_all_error_flags();
}

// Update PC with NIP
void ppcsimbooke::ppc_regs::update_pc(){
    pc = nip;
}

void ppcsimbooke::ppc_regs::inc_pc(){
    pc += 4;
}

void ppcsimbooke::ppc_regs::inc_nip(){
    nip += 4;
}

// std::cout overloading for register file
std::ostream& ppcsimbooke::operator<<(std::ostream& ostr, const ppcsimbooke::ppc_regs& reg_file){
#define PPCSIMBOOKE_GET_REGV(rg)  std::setw(16) << reg_file.rg.get_v()

    ostr << std::showbase << std::hex << std::endl;
    ostr << "Register file ####################################" << std::endl;
    ostr << "CM          = " << reg_file.cm                      << std::endl;
    ostr << "PC          = " << reg_file.pc                      << std::endl;
    ostr << "NIP         = " << reg_file.nip                     << std::endl;
    ostr << "MSR         = " << PPCSIMBOOKE_GET_REGV(msr)        << std::endl;
    ostr << "CR          = " << PPCSIMBOOKE_GET_REGV(cr)         << std::endl;
    ostr << "ACC         = " << PPCSIMBOOKE_GET_REGV(acc)        << std::endl;

    ostr << "GPR[0]      = " << PPCSIMBOOKE_GET_REGV(gpr[0])     << std::endl;
    ostr << "GPR[1]      = " << PPCSIMBOOKE_GET_REGV(gpr[1])     << std::endl;
    ostr << "GPR[2]      = " << PPCSIMBOOKE_GET_REGV(gpr[2])     << std::endl;
    ostr << "GPR[3]      = " << PPCSIMBOOKE_GET_REGV(gpr[3])     << std::endl;
    ostr << "GPR[4]      = " << PPCSIMBOOKE_GET_REGV(gpr[4])     << std::endl;
    ostr << "GPR[5]      = " << PPCSIMBOOKE_GET_REGV(gpr[5])     << std::endl;
    ostr << "GPR[6]      = " << PPCSIMBOOKE_GET_REGV(gpr[6])     << std::endl;
    ostr << "GPR[7]      = " << PPCSIMBOOKE_GET_REGV(gpr[7])     << std::endl;
    ostr << "GPR[8]      = " << PPCSIMBOOKE_GET_REGV(gpr[8])     << std::endl;
    ostr << "GPR[9]      = " << PPCSIMBOOKE_GET_REGV(gpr[9])     << std::endl;
    ostr << "GPR[10]     = " << PPCSIMBOOKE_GET_REGV(gpr[10])    << std::endl;
    ostr << "GPR[11]     = " << PPCSIMBOOKE_GET_REGV(gpr[11])    << std::endl;
    ostr << "GPR[12]     = " << PPCSIMBOOKE_GET_REGV(gpr[12])    << std::endl;
    ostr << "GPR[13]     = " << PPCSIMBOOKE_GET_REGV(gpr[13])    << std::endl;
    ostr << "GPR[14]     = " << PPCSIMBOOKE_GET_REGV(gpr[14])    << std::endl;
    ostr << "GPR[15]     = " << PPCSIMBOOKE_GET_REGV(gpr[15])    << std::endl;
    ostr << "GPR[16]     = " << PPCSIMBOOKE_GET_REGV(gpr[16])    << std::endl;
    ostr << "GPR[17]     = " << PPCSIMBOOKE_GET_REGV(gpr[17])    << std::endl;
    ostr << "GPR[18]     = " << PPCSIMBOOKE_GET_REGV(gpr[18])    << std::endl;
    ostr << "GPR[19]     = " << PPCSIMBOOKE_GET_REGV(gpr[19])    << std::endl;
    ostr << "GPR[20]     = " << PPCSIMBOOKE_GET_REGV(gpr[20])    << std::endl;
    ostr << "GPR[21]     = " << PPCSIMBOOKE_GET_REGV(gpr[21])    << std::endl;
    ostr << "GPR[22]     = " << PPCSIMBOOKE_GET_REGV(gpr[22])    << std::endl;
    ostr << "GPR[23]     = " << PPCSIMBOOKE_GET_REGV(gpr[23])    << std::endl;
    ostr << "GPR[24]     = " << PPCSIMBOOKE_GET_REGV(gpr[24])    << std::endl;
    ostr << "GPR[25]     = " << PPCSIMBOOKE_GET_REGV(gpr[25])    << std::endl;
    ostr << "GPR[26]     = " << PPCSIMBOOKE_GET_REGV(gpr[26])    << std::endl;
    ostr << "GPR[27]     = " << PPCSIMBOOKE_GET_REGV(gpr[27])    << std::endl;
    ostr << "GPR[28]     = " << PPCSIMBOOKE_GET_REGV(gpr[28])    << std::endl;
    ostr << "GPR[29]     = " << PPCSIMBOOKE_GET_REGV(gpr[29])    << std::endl;
    ostr << "GPR[30]     = " << PPCSIMBOOKE_GET_REGV(gpr[30])    << std::endl;
    ostr << "GPR[31]     = " << PPCSIMBOOKE_GET_REGV(gpr[31])    << std::endl;

    ostr << "ATBL        = " << PPCSIMBOOKE_GET_REGV(atbl)            << std::endl;
    ostr << "ATBU        = " << PPCSIMBOOKE_GET_REGV(atbu)            << std::endl;
    ostr << "CSRR0       = " << PPCSIMBOOKE_GET_REGV(csrr0)           << std::endl;
    ostr << "CSRR1       = " << PPCSIMBOOKE_GET_REGV(csrr1)           << std::endl;
    ostr << "CTR         = " << PPCSIMBOOKE_GET_REGV(ctr)             << std::endl;
    ostr << "DAC1        = " << PPCSIMBOOKE_GET_REGV(dac1)            << std::endl;
    ostr << "DAC2        = " << PPCSIMBOOKE_GET_REGV(dac2)            << std::endl;
    ostr << "DBCR0       = " << PPCSIMBOOKE_GET_REGV(dbcr0)           << std::endl;
    ostr << "DBCR1       = " << PPCSIMBOOKE_GET_REGV(dbcr1)           << std::endl;
    ostr << "DBCR2       = " << PPCSIMBOOKE_GET_REGV(dbcr2)           << std::endl;
    ostr << "DBSR        = " << PPCSIMBOOKE_GET_REGV(dbsr)            << std::endl;
    ostr << "DEAR        = " << PPCSIMBOOKE_GET_REGV(dear)            << std::endl;
    ostr << "DEC         = " << PPCSIMBOOKE_GET_REGV(dec)             << std::endl;
    ostr << "DECAR       = " << PPCSIMBOOKE_GET_REGV(decar)           << std::endl;
    ostr << "ESR         = " << PPCSIMBOOKE_GET_REGV(esr)             << std::endl;
    ostr << "IAC1        = " << PPCSIMBOOKE_GET_REGV(iac1)            << std::endl;
    ostr << "IAC2        = " << PPCSIMBOOKE_GET_REGV(iac2)            << std::endl;
    ostr << "IVOR0       = " << PPCSIMBOOKE_GET_REGV(ivor0)           << std::endl;
    ostr << "IVOR1       = " << PPCSIMBOOKE_GET_REGV(ivor1)           << std::endl;
    ostr << "IVOR2       = " << PPCSIMBOOKE_GET_REGV(ivor2)           << std::endl;
    ostr << "IVOR3       = " << PPCSIMBOOKE_GET_REGV(ivor3)           << std::endl;
    ostr << "IVOR4       = " << PPCSIMBOOKE_GET_REGV(ivor4)           << std::endl;
    ostr << "IVOR5       = " << PPCSIMBOOKE_GET_REGV(ivor5)           << std::endl;
    ostr << "IVOR6       = " << PPCSIMBOOKE_GET_REGV(ivor6)           << std::endl;
    ostr << "IVOR7       = " << PPCSIMBOOKE_GET_REGV(ivor7)           << std::endl;
    ostr << "IVOR8       = " << PPCSIMBOOKE_GET_REGV(ivor8)           << std::endl;
    ostr << "IvOR9       = " << PPCSIMBOOKE_GET_REGV(ivor9)           << std::endl;
    ostr << "IVOR10      = " << PPCSIMBOOKE_GET_REGV(ivor10)          << std::endl;
    ostr << "IVOR11      = " << PPCSIMBOOKE_GET_REGV(ivor11)          << std::endl;
    ostr << "IVOR12      = " << PPCSIMBOOKE_GET_REGV(ivor12)          << std::endl;
    ostr << "IVOR13      = " << PPCSIMBOOKE_GET_REGV(ivor13)          << std::endl;
    ostr << "IVOR14      = " << PPCSIMBOOKE_GET_REGV(ivor14)          << std::endl;
    ostr << "IVOR15      = " << PPCSIMBOOKE_GET_REGV(ivor15)          << std::endl;
    ostr << "IVPR        = " << PPCSIMBOOKE_GET_REGV(ivpr)            << std::endl;
    ostr << "LR          = " << PPCSIMBOOKE_GET_REGV(lr)              << std::endl;
    ostr << "PID0        = " << PPCSIMBOOKE_GET_REGV(pid0)            << std::endl;
    ostr << "PID1        = " << PPCSIMBOOKE_GET_REGV(pid1)            << std::endl;
    ostr << "PID2        = " << PPCSIMBOOKE_GET_REGV(pid2)            << std::endl;
    ostr << "PIR         = " << PPCSIMBOOKE_GET_REGV(pir)             << std::endl;
    ostr << "PVR         = " << PPCSIMBOOKE_GET_REGV(pvr)             << std::endl;
    ostr << "SPRG0       = " << PPCSIMBOOKE_GET_REGV(sprg0)           << std::endl;
    ostr << "SPRG1       = " << PPCSIMBOOKE_GET_REGV(sprg1)           << std::endl;
    ostr << "SPRG2       = " << PPCSIMBOOKE_GET_REGV(sprg2)           << std::endl;
    ostr << "SPRG3       = " << PPCSIMBOOKE_GET_REGV(sprg3)           << std::endl;
    ostr << "SPRG4       = " << PPCSIMBOOKE_GET_REGV(sprg4)           << std::endl;
    ostr << "SPRG5       = " << PPCSIMBOOKE_GET_REGV(sprg5)           << std::endl;
    ostr << "SPRG6       = " << PPCSIMBOOKE_GET_REGV(sprg6)           << std::endl;
    ostr << "SPRG7       = " << PPCSIMBOOKE_GET_REGV(sprg7)           << std::endl;
    ostr << "SRR0        = " << PPCSIMBOOKE_GET_REGV(srr0)            << std::endl;
    ostr << "SRR1        = " << PPCSIMBOOKE_GET_REGV(srr1)            << std::endl;
    ostr << "TBL         = " << PPCSIMBOOKE_GET_REGV(tbl)             << std::endl;
    ostr << "TBU         = " << PPCSIMBOOKE_GET_REGV(tbu)             << std::endl;
    ostr << "TCR         = " << PPCSIMBOOKE_GET_REGV(tcr)             << std::endl;
    ostr << "TSR         = " << PPCSIMBOOKE_GET_REGV(tsr)             << std::endl;
    ostr << "USPRG0      = " << PPCSIMBOOKE_GET_REGV(usprg0)          << std::endl;
    ostr << "XER         = " << PPCSIMBOOKE_GET_REGV(xer)             << std::endl;

    ostr << "BBEAR       = " << PPCSIMBOOKE_GET_REGV(bbear)           << std::endl;
    ostr << "BBTAR       = " << PPCSIMBOOKE_GET_REGV(bbtar)           << std::endl;
    ostr << "BUCSR       = " << PPCSIMBOOKE_GET_REGV(bucsr)           << std::endl;
    ostr << "HID0        = " << PPCSIMBOOKE_GET_REGV(hid0)            << std::endl;
    ostr << "HID1        = " << PPCSIMBOOKE_GET_REGV(hid1)            << std::endl;
    ostr << "IVOR32      = " << PPCSIMBOOKE_GET_REGV(ivor32)          << std::endl;
    ostr << "IVOR33      = " << PPCSIMBOOKE_GET_REGV(ivor33)          << std::endl;
    ostr << "IVOR34      = " << PPCSIMBOOKE_GET_REGV(ivor34)          << std::endl;
    ostr << "IVOR35      = " << PPCSIMBOOKE_GET_REGV(ivor35)          << std::endl;
    ostr << "L1CFG0      = " << PPCSIMBOOKE_GET_REGV(l1cfg0)          << std::endl;
    ostr << "L1CFG1      = " << PPCSIMBOOKE_GET_REGV(l1cfg1)          << std::endl;
    ostr << "L1CSR0      = " << PPCSIMBOOKE_GET_REGV(l1csr0)          << std::endl;
    ostr << "L1CSR1      = " << PPCSIMBOOKE_GET_REGV(l1csr1)          << std::endl;
    ostr << "MAS0        = " << PPCSIMBOOKE_GET_REGV(mas0)            << std::endl;
    ostr << "MAS1        = " << PPCSIMBOOKE_GET_REGV(mas1)            << std::endl;
    ostr << "MAS2        = " << PPCSIMBOOKE_GET_REGV(mas2)            << std::endl;
    ostr << "MAS3        = " << PPCSIMBOOKE_GET_REGV(mas3)            << std::endl;
    ostr << "MAS4        = " << PPCSIMBOOKE_GET_REGV(mas4)            << std::endl;
    ostr << "MAS5        = " << PPCSIMBOOKE_GET_REGV(mas5)            << std::endl;
    ostr << "MAS6        = " << PPCSIMBOOKE_GET_REGV(mas6)            << std::endl;
    ostr << "MAS7        = " << PPCSIMBOOKE_GET_REGV(mas7)            << std::endl;
    ostr << "MCAR        = " << PPCSIMBOOKE_GET_REGV(mcar)            << std::endl;
    ostr << "MCSR        = " << PPCSIMBOOKE_GET_REGV(mcsr)            << std::endl;
    ostr << "MCSRR0      = " << PPCSIMBOOKE_GET_REGV(mcsrr0)          << std::endl;
    ostr << "MCSRR1      = " << PPCSIMBOOKE_GET_REGV(mcsrr1)          << std::endl;
    ostr << "MMUCFG      = " << PPCSIMBOOKE_GET_REGV(mmucfg)          << std::endl;
    ostr << "MMUCSR0     = " << PPCSIMBOOKE_GET_REGV(mmucsr0)         << std::endl;
    ostr << "SPEFSCR     = " << PPCSIMBOOKE_GET_REGV(spefscr)         << std::endl;
    ostr << "SVR         = " << PPCSIMBOOKE_GET_REGV(svr)             << std::endl;
    ostr << "TLB0CFG     = " << PPCSIMBOOKE_GET_REGV(tlb0cfg)         << std::endl;
    ostr << "TLB1CFG     = " << PPCSIMBOOKE_GET_REGV(tlb1cfg)         << std::endl;

    ostr << "PMGC0       = " << PPCSIMBOOKE_GET_REGV(pmgc0)           << std::endl;
    ostr << "PMLCA0      = " << PPCSIMBOOKE_GET_REGV(pmlca0)          << std::endl;
    ostr << "PMLCA1      = " << PPCSIMBOOKE_GET_REGV(pmlca1)          << std::endl;
    ostr << "PMLCA2      = " << PPCSIMBOOKE_GET_REGV(pmlca2)          << std::endl;
    ostr << "PMLCA3      = " << PPCSIMBOOKE_GET_REGV(pmlca3)          << std::endl;
    ostr << "PMLCB0      = " << PPCSIMBOOKE_GET_REGV(pmlcb0)          << std::endl;
    ostr << "PMLCB1      = " << PPCSIMBOOKE_GET_REGV(pmlcb1)          << std::endl;
    ostr << "PMLCB2      = " << PPCSIMBOOKE_GET_REGV(pmlcb2)          << std::endl;
    ostr << "PMLCB3      = " << PPCSIMBOOKE_GET_REGV(pmlcb3)          << std::endl;
    ostr << "PMC0        = " << PPCSIMBOOKE_GET_REGV(pmc0)            << std::endl;
    ostr << "PMC1        = " << PPCSIMBOOKE_GET_REGV(pmc1)            << std::endl;
    ostr << "PMC2        = " << PPCSIMBOOKE_GET_REGV(pmc2)            << std::endl;
    ostr << "PMC3        = " << PPCSIMBOOKE_GET_REGV(pmc3)            << std::endl;

    ostr << "UPMGC0      = " << PPCSIMBOOKE_GET_REGV(upmgc0)          << std::endl;
    ostr << "UPMLCA0     = " << PPCSIMBOOKE_GET_REGV(upmlca0)         << std::endl;
    ostr << "UPMLCA1     = " << PPCSIMBOOKE_GET_REGV(upmlca1)         << std::endl;
    ostr << "UPMLCA2     = " << PPCSIMBOOKE_GET_REGV(upmlca2)         << std::endl;
    ostr << "UPMLCA3     = " << PPCSIMBOOKE_GET_REGV(upmlca3)         << std::endl;
    ostr << "UPMLCB0     = " << PPCSIMBOOKE_GET_REGV(upmlcb0)         << std::endl;
    ostr << "UPMLCB1     = " << PPCSIMBOOKE_GET_REGV(upmlcb1)         << std::endl;
    ostr << "UPMLCB2     = " << PPCSIMBOOKE_GET_REGV(upmlcb2)         << std::endl;
    ostr << "UPMLCB3     = " << PPCSIMBOOKE_GET_REGV(upmlcb3)         << std::endl;
    ostr << "UPMC0       = " << PPCSIMBOOKE_GET_REGV(upmc0)           << std::endl;
    ostr << "UPMC1       = " << PPCSIMBOOKE_GET_REGV(upmc1)           << std::endl;
    ostr << "UPMC2       = " << PPCSIMBOOKE_GET_REGV(upmc2)           << std::endl;
    ostr << "UPMC3       = " << PPCSIMBOOKE_GET_REGV(upmc3)           << std::endl;

    ostr << "##################################################"    << std::endl;

#undef PPCSIMBOOKE_GET_REGV
    return ostr;
}
