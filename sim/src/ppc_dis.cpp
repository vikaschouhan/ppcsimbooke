#include "ppc_dis.h"

// static members
uint16_t ppcsimbooke::ppcsimbooke_dis::ppcdis::m_ppc_opcd_indices[ppcsimbooke::ppcsimbooke_dis::N_PPC_OPCODES];
bool     ppcsimbooke::ppcsimbooke_dis::ppcdis::m_opcd_indices_done = 0;

// Member functions.
//

///////////////////////////////////////////////////////////////////////////////////////////////////
// private helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Initialize opcode indices for faster look up
void ppcsimbooke::ppcsimbooke_dis::ppcdis::init_opcd_indices(){
    int i;
    unsigned short last;

    i = powerpc_num_opcodes;
    while (--i >= 0)
    {
        unsigned op = PPC_OP (powerpc_opcodes[i].opcode);
        m_ppc_opcd_indices[op] = i;
    }
    last = powerpc_num_opcodes;
    for (i = 64; i > 0; --i)
    {
        if (m_ppc_opcd_indices[i] == 0)
            m_ppc_opcd_indices[i] = last;
        last = m_ppc_opcd_indices[i];
    }
}

// Initialize dialect
void ppcsimbooke::ppcsimbooke_dis::ppcdis::init_dialect(){
   m_dialect = ppc_parse_cpu (0, "e500x2");
}

// Return new dialect according to specified cpu name
ppc_cpu_t ppcsimbooke::ppcsimbooke_dis::ppcdis::ppc_parse_cpu (ppc_cpu_t ppc_cpu, const char *arg)
{
    /* Sticky bits.  */
    ppc_cpu_t retain_flags = ppc_cpu & (PPC_OPCODE_ALTIVEC | PPC_OPCODE_VSX
                             | PPC_OPCODE_SPE | PPC_OPCODE_ANY);
    unsigned int i;

    for (i = 0; i < sizeof (ppc_opts) / sizeof (ppc_opts[0]); i++)
    {
        if (strcmp (ppc_opts[i].opt, arg) == 0)
        {
            if (ppc_opts[i].sticky)
            {
                retain_flags |= ppc_opts[i].sticky;
                if ((ppc_cpu & ~(ppc_cpu_t) (PPC_OPCODE_ALTIVEC | PPC_OPCODE_VSX
                       | PPC_OPCODE_SPE | PPC_OPCODE_ANY)) != 0)
                break;
            }
            ppc_cpu = ppc_opts[i].cpu;
            break;
        }
    }
    if (i >= sizeof (ppc_opts) / sizeof (ppc_opts[0]))
        return 0;

    ppc_cpu |= retain_flags;
    return ppc_cpu;
}

// Extract operand value from instruction
long ppcsimbooke::ppcsimbooke_dis::ppcdis::operand_value_powerpc (const struct powerpc_operand *operand, unsigned long insn)
{
    long value;
    int invalid;
    /* Extract the value from the instruction.  */
    if (operand->extract)
        value = (*operand->extract) (insn, m_dialect, &invalid);
    else
    {
        value = (insn >> operand->shift) & operand->bitm;
        if ((operand->flags & PPC_OPERAND_SIGNED) != 0)
        {
            /* BITM is always some number of zeros followed by some
               number of ones, followed by some numer of zeros.  */
            unsigned long top = operand->bitm;
            /* top & -top gives the rightmost 1 bit, so this
               fills in any trailing zeros.  */
            top |= (top & -top) - 1;
            top &= ~(top >> 1);
            value = (value ^ top) - top;
        }
    }

    return value;
}

/* Find a match for INSN in the opcode table, given machine DIALECT.
A DIALECT of -1 is special, matching all machine opcode variations.  */
// Return the opcode entry and also the index number
std::pair<const struct powerpc_opcode*, uint32_t> ppcsimbooke::ppcsimbooke_dis::ppcdis::lookup_powerpc (unsigned long insn)
{
    const struct powerpc_opcode *opcode;
    const struct powerpc_opcode *opcode_end;
    unsigned long op;
    uint32_t indx = 0;

    /* Get the major opcode of the instruction.  */
    op = PPC_OP (insn);

    /* Find the first match in the opcode table for this major opcode.  */
    opcode_end = powerpc_opcodes + m_ppc_opcd_indices[op + 1];
    for (opcode = powerpc_opcodes + m_ppc_opcd_indices[op], indx=0; opcode < opcode_end; ++opcode, ++indx)
    {
        const unsigned char *opindex;
        const struct powerpc_operand *operand;
        int invalid;

        // FIXME
        // There are many instances in powerpc_opcodes database
        // where mask and opcode don't match. Such instrs can't be disassembled
        // with successfully. This needs to be fixed in GNU libopcode library itself.
        //
        // Commenting below code until the problem is fixed.
        //
        // Revrting it, as it started creating problems later on
        if ((insn & opcode->mask) != opcode->opcode || (m_dialect != (ppc_cpu_t)(-1) && ((opcode->flags & m_dialect) == 0
                   || (opcode->deprecated & m_dialect) != 0)))
            continue;

        /* Check validity of operands.  */
        invalid = 0;
        for (opindex = opcode->operands; *opindex != 0; opindex++)
        {
            operand = powerpc_operands + *opindex;
            if (operand->extract)
                (*operand->extract) (insn, m_dialect, &invalid);
        }
        if (invalid)
            continue;

        return std::make_pair(opcode, m_ppc_opcd_indices[op] + indx);
    }

    return std::make_pair(reinterpret_cast<const struct powerpc_opcode*>(NULL), 0);
}

int ppcsimbooke::ppcsimbooke_dis::ppcdis::if_std_delims_present(char *str)
{
    char std_delims[] = " ,()";
    int i, j;
    for(i=0; str[i] != '\0'; i++)
        for(j=0; std_delims[j] != '\0'; j++){
            if(str[i] == std_delims[j])
                return 1;
        }
    return 0; 
}  

// Get number of operands from powerpc_opcode struct
int ppcsimbooke::ppcsimbooke_dis::ppcdis::get_num_operands(const struct powerpc_opcode *opcode){
    int num_oprs = 0;
    if(opcode == NULL)
        return -1;
    while(opcode->operands[num_oprs++] != 0){}
    return (num_oprs-1);
} 

/////////////////////////////////////////////////////////////////////////////////////////////
// public disassembler functions
////////////////////////////////////////////////////////////////////////////////////////////

// Disassemble a 32 bit opcode into a call frame
ppcsimbooke::instr_call ppcsimbooke::ppcsimbooke_dis::ppcdis::disasm(uint32_t opcd, uint64_t pc, int endianness)
{
    instr_call  call_this;
    unsigned long insn;
    const struct powerpc_opcode *opcode;
    int i = 0;
    bool rad = 0;        // flag for relative addressing mode
    _dis_info  dinfo;    // extra disassembler info
    std::pair<const struct powerpc_opcode*, uint32_t> opc_pair;

    // We reverse endianness if LITTLE endian specified
    if (endianness == EMUL_LITTLE_ENDIAN)
        insn = (((opcd >>  0) & 0xff) << 24) |
               (((opcd >>  8) & 0xff) << 16) |
               (((opcd >> 16) & 0xff) <<  8) |
               (((opcd >> 24) & 0xff) <<  0) ;
    else
        insn = opcd;

    // search in the cache first
    call_this = m_dis_cache[insn];
    dinfo     = m_dis_cache.info_at(insn);
    if(m_dis_cache.error()){
        goto exit_0;
    }

    LOG_DEBUG4("Loading ", call_this, " from PC:", dinfo.pc, std::endl);

    // If pc required and current pc == cached pc, return the value
    if(dinfo.pcr && ( dinfo.pc == pc))
        return call_this;
    // if pc not required, then also return
    if(!dinfo.pcr)
        return call_this;

    exit_0:
    opc_pair = lookup_powerpc (insn);
    opcode   = opc_pair.first;

    if (opcode == NULL && (m_dialect & PPC_OPCODE_ANY) != 0)
    {
        m_dialect = -1;
        opc_pair = lookup_powerpc (insn);
        opcode   = opc_pair.first;
    }

    if (opcode != NULL)
    {
        const unsigned char *opindex;
        const struct powerpc_operand *operand;
        int need_comma;
        int need_paren;

        /* If we are here, it means correct opcode was found in the table */
        /* Store opcode name in passed disassemble_info */
        call_this.opcname = std::string(opcode->name);
        call_this.hv      = (opcode->opcode << 32 | opc_pair.second);
        call_this.opc     = opcode->opcode;

        if (opcode->operands[0] != 0)
            call_this.fmt = "%-7s ";
        else
            call_this.fmt = "%s";

        /* Now extract and print the operands.  */
        need_comma = 0;
        need_paren = 0;
        for (opindex = opcode->operands, i=0; *opindex != 0; opindex++, i++)
        {
            long value;

            operand = powerpc_operands + *opindex;

            /* Operands that are marked FAKE are simply ignored.  We
               already made sure that the extract function considered
               the instruction to be valid.  */
            if ((operand->flags & PPC_OPERAND_FAKE) != 0)
                continue;

            /* If all of the optional operands have the value zero,
               then don't print any of them.  */
            //if ((operand->flags & PPC_OPERAND_OPTIONAL) != 0)
            //{
            //}

            value = operand_value_powerpc (operand, insn);

            if (need_comma)
            {
                call_this.fmt += ",";
                need_comma = 0;
            }

            /* Print the operand as directed by the flags.  */
            if ((operand->flags & PPC_OPERAND_GPR) != 0 || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && value != 0)){
                call_this.fmt += "r%ld";
                call_this.arg[i].p = (REG_GPR0 + value);
                call_this.arg[i].v = value;
                call_this.arg[i].t = 1;
            //} else if ((operand->flags & PPC_OPERAND_FPR) != 0){
            //    call_this.fmt += "f%ld";
            //    call_this.arg[i].p = (REG_FPR0 + value);
            //    call_this.arg[i].v = value;
            //    call_this.arg[i].t = 1;
            } else if ((operand->flags & PPC_OPERAND_VR) != 0){
                call_this.fmt += "v%ld";
                // FIXME: VRs are not supported at this time.
            } else if ((operand->flags & PPC_OPERAND_VSR) != 0){
                call_this.fmt += "vs%ld";
                // FIXME : Booke cpus shouldn't come here
            } else if ((operand->flags & PPC_OPERAND_RELATIVE) != 0){
                call_this.fmt += "0x%lx";
                call_this.arg[i].p = (value + pc);
                call_this.arg[i].v = (value + pc);
                call_this.arg[i].t = 0;
                rad = 1;
            } else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0){
                call_this.fmt += "0x%lx";
                call_this.arg[i].p = value;
                call_this.arg[i].v = value;
                call_this.arg[i].t = 0;
            } else if ((operand->flags & PPC_OPERAND_FSL) != 0){
                call_this.fmt += "fsl%ld";
                // FIXME: Booke cpus shouln't come here
            } else if ((operand->flags & PPC_OPERAND_FCR) != 0){
                call_this.fmt += "fcr%ld";
                // FIXME : Booke cpus shouldn't come here
            } else if ((operand->flags & PPC_OPERAND_UDI) != 0){
                call_this.fmt += "%ld";
                // FIXME : Booke cpus shouldn't come here
            } else if ((operand->flags & PPC_OPERAND_CR) != 0 && (m_dialect & PPC_OPCODE_PPC) != 0){
            
                if (operand->bitm == 7)
                    call_this.fmt += "cr%ld";
                else
                    call_this.fmt += "cr[%ld]";
                call_this.arg[i].p = value;
                call_this.arg[i].v = value;
                call_this.arg[i].t = 0;
            }
            else{
                call_this.fmt += "0x%x";
                call_this.arg[i].p = value;
                call_this.arg[i].v = value;
                call_this.arg[i].t = 0;
            }

            if (need_paren)
            {
                call_this.fmt += ")";
                need_paren = 0;
            }

            if ((operand->flags & PPC_OPERAND_PARENS) == 0)
                need_comma = 1;
            else
            {
                call_this.fmt += "(";
                need_paren = 1;
            }

            // Increment argument count
            call_this.nargs++;
        }

    }else{
        // Throw an Illegal Program exception.
        LTHROW(sim_except_ppc(PPC_EXCEPTION_PRG, PPC_EXCEPT_PRG_ILG, "Illegal Program Exception."), DEBUG4);
    }

    // Update cache
    m_dis_cache.insert(insn, call_this, _dis_info(pc, rad));

    return call_this;
}

// Disassemble a 32 bit opcode (passed as a char pointer) into a call frame
ppcsimbooke::instr_call ppcsimbooke::ppcsimbooke_dis::ppcdis::disasm(uint8_t* opcd, uint64_t pc, int endianness)
{
    return disasm(read_buff<uint32_t>(opcd, endianness), pc, endianness);
}


// Disassemble a string representation of instruction ( something like asm + disasm )
ppcsimbooke::instr_call ppcsimbooke::ppcsimbooke_dis::ppcdis::disasm(std::string instr, uint64_t pc)
{
    char *token = NULL;
    char *tmp_str = (char *)instr.c_str();
    char delim = ',';
    instr_call call_this;
    uint32_t indx = 0;

    int i=0;
    size_t value = 0;
    int noperands = 0;
    const struct powerpc_operand* operand = NULL;
    const struct powerpc_opcode*  opcode  = NULL;

    token = find_tok(&tmp_str, ' ');    /* One space is first required between instr and it's operands */
    assert_and_throw(!if_std_delims_present(token), sim_except(SIM_EXCEPT_EINVAL, "Wrong instr" + std::string(token)));

    // Try to get the opcode entry
    for(i=0, indx=0; i<powerpc_num_opcodes; i++, indx++){
        if(!strcmp(token, powerpc_opcodes[i].name)){
            opcode = (powerpc_opcodes + i);
            break;
        }
    }

    /* Remove all spaces */
    prune_all_spaces(tmp_str);

    /* Throw normal sim exception ( since this function is not meant for the simulation itself ). */
    assert_and_throw(opcode != NULL, sim_except(SIM_EXCEPT_EINVAL, "Wrong opcode"));

    // Get opcode's name
    call_this.opcname = std::string(opcode->name);
    call_this.hv      = (opcode->opcode << 32 | indx);
    call_this.opc     = opcode->opcode;

    if (opcode->operands[0] != 0)
        call_this.fmt = "%-7s ";
    else
        call_this.fmt = "%s";

    /* Get number of operands */
    noperands = get_num_operands(opcode);
    for(i=0; i<noperands; i++){
        
        if(tmp_str == NULL || *tmp_str == '\0'){
            printf("Wrong instr format. Correct instr format is \n");
            print_insn_fmt(opcode->name);
            throw(sim_except(SIM_EXCEPT_EINVAL, "Wrong instruction format"));
        }

        if (i != 0){
            if(*tmp_str != delim){
                printf("Wrong instr format. Correct instr format is \n");
                print_insn_fmt(opcode->name);
                throw(sim_except(SIM_EXCEPT_EINVAL, "Wrong instruction format"));
            }
            tmp_str ++;
        }

        if(tmp_str == NULL || *tmp_str == '\0'){
            printf("Wrong instr format. Correct instr format is \n");
            print_insn_fmt(opcode->name);
            throw(sim_except(SIM_EXCEPT_EINVAL, "Wrong instruction format"));
        }

        delim = get_delim_char(opcode, i);
        token = find_tok(&tmp_str, delim);
        if((token == NULL) || (*token == '\0'))
            break;
        if(if_std_delims_present(token)){
            printf("Wrong instr format. Correct format is \n");
            print_insn_fmt(opcode->name);
            throw(sim_except(SIM_EXCEPT_EINVAL, "Wrong instruction format"));
        }
        
        operand = (powerpc_operands + opcode->operands[i]);
        value = 0;

        if ((operand->flags & PPC_OPERAND_GPR) != 0
            || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && token != 0)){
            sscanf(token, "r%ld", &value);
            call_this.fmt    += "r%ld";
            call_this.arg[i].p = (REG_GPR0 + value);
            call_this.arg[i].v = value;
            call_this.arg[i].t = 1;
        }
        //else if ((operand->flags & PPC_OPERAND_FPR) != 0){
        //    sscanf(token, "f%ld", &value);
        //    call_this.fmt    += "f%ld";
        //    call_this.arg[i].p = (REG_FPR0 + value);
        //    call_this.arg[i].v = value;
        //    call_this.arg[i].t = 1;
        //}
        else if ((operand->flags & PPC_OPERAND_VR) != 0){
            sscanf(token, "v%ld", &value);
            call_this.fmt    += "v%ld";
            // FIXME: VRs not supported at this time. targ should be specified when supported
        }
        else if ((operand->flags & PPC_OPERAND_VSR) != 0){
            sscanf(token, "vs%ld", &value);
            call_this.fmt    += "vs%ld";
            // FIXME: Booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_RELATIVE) != 0){
	        sscanf(token, "0x%llx", reinterpret_cast<unsigned long long *>(&value));
            call_this.fmt += "0x%llx";
            call_this.arg[i].p = value - pc;
            call_this.arg[i].v = value - pc;                       // Relative addressing mode
            call_this.arg[i].t = 0;
        }
	    else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0){
	        sscanf(token, "0x%llx", reinterpret_cast<unsigned long long *>(&value));
            call_this.fmt += "0x%llx";
            call_this.arg[i].p = value;
            call_this.arg[i].v = value;
            call_this.arg[i].t = 0;
        }
        else if ((operand->flags & PPC_OPERAND_FSL) != 0){
            sscanf(token, "fsl%ld", &value);
            call_this.fmt    += "fsl%ld";
            // FIXME: booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_FCR) != 0){
            sscanf(token, "fcr%ld", &value);
            call_this.fmt    += "fcr%ld";
            // FIXME : booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_UDI) != 0){
            sscanf(token, "0x%lx", &value);
            call_this.fmt    += "0x%lx";
            // FIXME : booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_CR) != 0)
        {
            if (operand->bitm == 7){
                sscanf(token, "cr%ld", &value);
                call_this.fmt  += "cr%ld";
            }
            else
            {
                static const char *cbnames[4] = { "lt", "gt", "eq", "so" };
                char cbname[5];
                int cr, ind;
                cr = ind = 0;

                sscanf(token, "4*cr%d+%s", &cr, cbname);
                for(ind=0; ind < 4; i++)
                    if(!strcmp(cbname, cbnames[i]))
                        break;
                if(ind == 4){
                    /* Error */
                }
                value = 4*cr + ind;
                call_this.fmt  += "cr[%ld]";
            }
            call_this.arg[i].p = value;
            call_this.arg[i].v = value;
            call_this.arg[i].t = 0;
        }
        else{
            sscanf(token, "0x%lx", &value);
            call_this.fmt     += "0x%lx";
            call_this.arg[i].p = value;
            call_this.arg[i].v = value;
            call_this.arg[i].t = 0;
        }

        call_this.fmt += std::string(1, delim);    // Add delimiter to format string

        call_this.arg[i].v = value;
        call_this.nargs++;
    }

    return call_this;

}

//////////////////////////////////////////////////////////////////////////////////////
// misc public functions
//////////////////////////////////////////////////////////////////////////////////////

// Get opcode's index in disassembler LUT
int ppcsimbooke::ppcsimbooke_dis::ppcdis::get_opc_index(std::string opcname){
    for (int indx = 0; indx < powerpc_num_opcodes; indx++){
        if(!strcmp(opcname.c_str(), powerpc_opcodes[indx].name)){
            if (!(m_dialect != (ppc_cpu_t)(-1) &&
                       ((powerpc_opcodes[indx].flags & m_dialect) == 0 || (powerpc_opcodes[indx].deprecated & m_dialect) != 0)))
                return indx;
        }
    }
    // Throw a warning on undefined opcodes
    // NOTE : If an opcode was not found in the opcode table,
    //        this merely means that the extended opcode was not there.
    //        In that case, the disassembler module will automatically take care
    //        not to generate extended opcodes.
    std::cerr << "Warning !!! Undefined opcode " << opcname << std::endl;
    return 0xffffffff;
}

// Get binary opcode as a hashed value
uint64_t ppcsimbooke::ppcsimbooke_dis::ppcdis::get_opc_hash(std::string opcname){
    for (int indx = 0; indx < powerpc_num_opcodes; indx++){
        if(!strcmp(opcname.c_str(), powerpc_opcodes[indx].name)){
            if (!(m_dialect != (ppc_cpu_t)(-1) &&
                       ((powerpc_opcodes[indx].flags & m_dialect) == 0 || (powerpc_opcodes[indx].deprecated & m_dialect) != 0)))
                return (powerpc_opcodes[indx].opcode << 32 | indx);
        }
    }
    // Throw a warning on undefined opcodes
    // NOTE : If an opcode was not found in the opcode table,
    //        this merely means that the extended opcode was not there.
    //        In that case, the disassembler module will automatically take care
    //        not to generate extended opcodes.
    std::cerr << "Warning !!! Undefined opcode " << opcname << std::endl;
    return 0x0;
}

/////////////////////////////////////////////////////////////////////////////////////
// private helpers ??
/////////////////////////////////////////////////////////////////////////////////////

// show instruction format
void ppcsimbooke::ppcsimbooke_dis::ppcdis::print_insn_fmt (const char *insn_name)
{
    int i = 0;
    FILE *stream = stdout;
    const struct powerpc_opcode *opcode = NULL;

    if(insn_name == NULL)
        return;

    for(i=0; i<powerpc_num_opcodes; i++){
        if(!strcmp(insn_name, powerpc_opcodes[i].name)){
            opcode = &powerpc_opcodes[i];
            break;
        }
    }

    if (opcode == NULL)
        return;

    //bk();

    if (opcode != NULL)
    {
        const unsigned char *opindex;
        const struct powerpc_operand *operand;
        int need_comma;
        int need_paren;
        //int skip_optional;

        if (opcode->operands[0] != 0)
            fprintf(stream, "%-7s ", opcode->name);
        else
            fprintf(stream, "%s", opcode->name);

        /* Now extract and print the operands.  */
        need_comma = 0;
        need_paren = 0;
        //skip_optional = -1;
        for (opindex = opcode->operands; *opindex != 0; opindex++)
        {
            operand = powerpc_operands + *opindex;

            /* Operands that are marked FAKE are simply ignored.  We
               already made sure that the extract function considered
               the instruction to be valid.  */
            if ((operand->flags & PPC_OPERAND_FAKE) != 0)
                continue;

            if (need_comma)
            {
                printf(",");
                need_comma = 0;
            }

            /* Print the operand as directed by the flags.  */
            if ((operand->flags & PPC_OPERAND_GPR) != 0){
                fprintf(stream, "rX(X=integer)");
            }
            else if((operand->flags & PPC_OPERAND_GPR_0) != 0){
                    fprintf(stream, "rX|0(X=integer)");
            }
            //else if ((operand->flags & PPC_OPERAND_FPR) != 0)
            //    fprintf(stream, "fX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_VR) != 0)
                fprintf(stream, "vX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_VSR) != 0)
                fprintf(stream, "vsX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_RELATIVE) != 0){
                fprintf(stream, "hex_val");
            }
            else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0){
                fprintf(stream, "hex_val");
            }
            else if ((operand->flags & PPC_OPERAND_FSL) != 0)
                fprintf(stream, "fslX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_FCR) != 0)
                fprintf(stream,  "fcrX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_UDI) != 0)
                fprintf(stream, "hex_val");
            else if ((operand->flags & PPC_OPERAND_CR) != 0 && (m_dialect & PPC_OPCODE_PPC) != 0)
            {
                if (operand->bitm == 7)
                    fprintf(stream, "crX(X=[0-7])");
                else
                {
                    fprintf(stream, "4*crX+Y(X=[0-7], Y=[\"lt\", \"gt\", \"eq\", \"so\"])");
                }
            }
            else
                fprintf(stream, "hex_val");

            // All operands are required.
            //if ((operand->flags & PPC_OPERAND_OPTIONAL) != 0)
            //    fprintf(stream, " {OPTIONAL}");

            if (need_paren)
            {
                fprintf(stream, ")");
                need_paren = 0;
            }

            if ((operand->flags & PPC_OPERAND_PARENS) == 0)
                need_comma = 1;
            else
            {
                fprintf(stream, "( ");
                need_paren = 1;
            }
        }
        
        fprintf(stream, "\n");
        /* We have found and printed an instruction; return.  */
        return ;
    }

    fprintf(stream, "couldn't find a match for %s\n", insn_name);
    return ;
}

// Helper function
char ppcsimbooke::ppcsimbooke_dis::ppcdis::get_delim_char(const struct powerpc_opcode *opc, int argno){
    const struct powerpc_operand *curr_opr = (powerpc_operands + opc->operands[argno]);
    const struct powerpc_operand *prev_opr = NULL;
    const struct powerpc_operand *next_opr = (powerpc_operands + opc->operands[argno+1]);

    /* Delimiter for first arg is always comma */
    if(argno == 0){
        if(opc->operands[argno + 1])
            return ',';
        else
            return 0x20;
    }
    prev_opr = (powerpc_operands + opc->operands[argno - 1]);

    /* Check if next operand is enclosed within a () */
    if(curr_opr->flags & PPC_OPERAND_PARENS){
        return '(';
    }
    /* Check if next operand is enclosed within a () */
    else if(prev_opr->flags & PPC_OPERAND_PARENS){
	    return ')';
    }
    /* If this condition is hit, that means we are over now */
    else if(!next_opr->bitm){
        return '\0';
    }
    else
	{
	    return ',';
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// instruction group detection functions (public)
////////////////////////////////////////////////////////////////////////////////////////////

// check for all branch types
bool ppcsimbooke::ppcsimbooke_dis::ppcdis::is_branch(instr_call& ic){
    static const uint32_t bm_lut[]  = { 0x40000000, 0x48000000, 0x4c000020, 0x4c000420 };
    //                                       bc         b           bclr        bcctr
  
    for (size_t i=0; i<(sizeof(bm_lut)/sizeof(uint32_t)); i++){
        if((bm_lut[i] & ic.opc) == bm_lut[i]) return true;
    }
    return false;
}

// check for system call
bool ppcsimbooke::ppcsimbooke_dis::ppcdis::is_sc(instr_call& ic){
    static const uint32_t sc_mask = (0x17 << 26);
    return ((ic.opc & sc_mask) == sc_mask) ? true:false;
}

// check for rfXi
bool ppcsimbooke::ppcsimbooke_dis::ppcdis::is_rfxi(instr_call& ic){
    static const uint32_t rfxim_lut[] = { 0x4c00004c, 0x4c00004e, 0x4c000064, 0x4c000066 };
    //                                        rfmci      rfdi        rfi         rfci 

    for (size_t i=0; i<(sizeof(rfxim_lut)/sizeof(uint32_t)); i++){
        if((ic.opc & rfxim_lut[i]) == rfxim_lut[i]) return true;
    }
    return false;
}

// control transfer group check
bool ppcsimbooke::ppcsimbooke_dis::ppcdis::is_control_xfer(instr_call& ic){
    return is_branch(ic) || is_sc(ic) || is_rfxi(ic);
}


