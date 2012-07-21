#ifndef _PPC_DIS_HPP_
#define _PPC_DIS_HPP_

// ppc_dis.hpp ( disassembler facilities for powerPC cpu module )
// This file contains disassembler class and corresponding member functions.
//
// Authors :
//     GNU project.
//     Vikas Chouhan ( presentisgood@gmail.com )  Copyright 2012.
//
// This file is part of ppc-sim library bundled with test_gen_ppc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
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

// Most of the functions in this module will seem to C'ish, since these
// were taken from ppc-dis project, which is written in C.
//
// NOTE: The functions in this file are in messy state. Hope to clean it some day.
//       Also since most of the functions were originally in C, I had to make changes
//       to fit them in C++ perspective. So they are kinda in mixed state.

#include "config.h"
#include "cpu_ppc_regs.h"
#include "third_party/ppcdis/ppcdis.h"
#include "third_party/lru/lru.hpp"

#define  N_PPC_OPCODES  65

class ppc_dis_booke{
    public:
    typedef lru_cache<uint32_t, instr_call>     ppc_dis_cache;
    typedef lru_cache<std::string, instr_call>  ppc_dis_cache2;

    private:
    ppc_cpu_t       m_dialect;
    ppc_dis_cache   m_dis_cache;                         // Disassembler cache
    ppc_dis_cache   m_dis_cache2;                        // Second cache for string based opcodes
    static uint16_t m_ppc_opcd_indices[N_PPC_OPCODES];
    static bool     m_opcd_indices_done;

    private:
    // Initialize opcode indices for faster look up
    static void init_opcd_indices();
    // Initialize dialect
    void init_dialect();
    // Return new dialect according to specified cpu name
    ppc_cpu_t ppc_parse_cpu (ppc_cpu_t ppc_cpu, const char *arg);
    // Extract operand value from instruction
    long operand_value_powerpc (const struct powerpc_operand *operand, unsigned long insn);

    /* Find a match for INSN in the opcode table, given machine DIALECT.
    A DIALECT of -1 is special, matching all machine opcode variations.  */
    const struct powerpc_opcode * lookup_powerpc (unsigned long insn);

    // Helper function : check if standard delimiters are present
    int if_std_delims_present(char *str);
    // Helper function : Get number of operands from powerpc_opcode struct
    int get_num_operands(const struct powerpc_opcode *opcode);
    // Show instruction format
    void print_insn_fmt (const char *insn_name);
    // Helper function : Get delimiter character for next argno argument
    char get_delim_char(const struct powerpc_opcode *opc, int argno);
    
    public:
    // constructor
    ppc_dis_booke(){
        init_dialect();
        if(!m_opcd_indices_done){
            init_opcd_indices();
            m_opcd_indices_done = 1;
        }
        m_dis_cache.set_size(256);   // 256 entry cache
        m_dis_cache2.set_size(16);   // 16 entry cache
    }

    // Disassemble a 32 bit opcode into a call frame
    instr_call disasm(uint32_t opcd, int endianness = EMUL_BIG_ENDIAN);
    // Second form of disassemble. Takes a string, decodes it and creates an instruction call frame
    // if valid
    instr_call disasm(std::string instr);

};

// Static members
uint16_t ppc_dis_booke::m_ppc_opcd_indices[N_PPC_OPCODES];
bool     ppc_dis_booke::m_opcd_indices_done = 0;

// Member functions.
//
// Initialize opcode indices for faster look up
void ppc_dis_booke::init_opcd_indices(){
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
void ppc_dis_booke::init_dialect(){
   m_dialect = ppc_parse_cpu (0, "e500");
}

// Return new dialect according to specified cpu name
ppc_cpu_t ppc_dis_booke::ppc_parse_cpu (ppc_cpu_t ppc_cpu, const char *arg)
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
long ppc_dis_booke::operand_value_powerpc (const struct powerpc_operand *operand, unsigned long insn)
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

const struct powerpc_opcode * ppc_dis_booke::lookup_powerpc (unsigned long insn)
{
    const struct powerpc_opcode *opcode;
    const struct powerpc_opcode *opcode_end;
    unsigned long op;

    /* Get the major opcode of the instruction.  */
    op = PPC_OP (insn);

    /* Find the first match in the opcode table for this major opcode.  */
    opcode_end = powerpc_opcodes + m_ppc_opcd_indices[op + 1];
    for (opcode = powerpc_opcodes + m_ppc_opcd_indices[op]; opcode < opcode_end; ++opcode)
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

        return opcode;
    }

    return NULL;
}

int ppc_dis_booke::if_std_delims_present(char *str)
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
int ppc_dis_booke::get_num_operands(const struct powerpc_opcode *opcode){
    int num_oprs = 0;
    if(opcode == NULL)
        return -1;
    while(opcode->operands[num_oprs++] != 0){}
    return (num_oprs-1);
} 

// Disassemble a 32 bit opcode into a call frame
instr_call ppc_dis_booke::disasm(uint32_t opcd, int endianness)
{
    instr_call  call_this;
    unsigned long insn;
    const struct powerpc_opcode *opcode;
    int i = 0;

    // We reverse endianness if LITTLE endian specified
    if (endianness == EMUL_LITTLE_ENDIAN)
        insn = (((opcd >>  0) & 0xff) << 24) |
               (((opcd >>  8) & 0xff) << 16) |
               (((opcd >> 16) & 0xff) <<  8) |
               (((opcd >> 24) & 0xff) <<  0) ;
    else
        insn = opcd;

    try{
        call_this = m_dis_cache[insn];
    }catch(sim_exception& e){
        if(e.err_code() == SIM_EXCEPT_ILLEGAL_OP)
            goto exit_0;
        else
            LTHROW(sim_exception_fatal("Something is seriously funcked up !!"), DEBUG4);
    }
    return call_this;

    exit_0:
    opcode = lookup_powerpc (insn);

    if (opcode == NULL && (m_dialect & PPC_OPCODE_ANY) != 0)
    {
        m_dialect = -1;
        opcode = lookup_powerpc (insn);
    }

    if (opcode != NULL)
    {
        const unsigned char *opindex;
        const struct powerpc_operand *operand;
        int need_comma;
        int need_paren;

        /* If we are here, it means correct opcode was found in the table */
        /* Store opcode name in passed disassemble_info */
        call_this.opcode = std::string(opcode->name);

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
            call_this.arg[i].v = value;

            if (need_comma)
            {
                call_this.fmt += ",";
                need_comma = 0;
            }

            /* Print the operand as directed by the flags.  */
            if ((operand->flags & PPC_OPERAND_GPR) != 0 || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && value != 0)){
                call_this.fmt += "r%ld";
                call_this.arg[i].p = (REG_GPR0 + value);
                call_this.arg[i].t = 1;
            } else if ((operand->flags & PPC_OPERAND_FPR) != 0){
                call_this.fmt += "f%ld";
                call_this.arg[i].p = (REG_FPR0 + value);
                call_this.arg[i].t = 1;
            } else if ((operand->flags & PPC_OPERAND_VR) != 0){
                call_this.fmt += "v%ld";
                // VRs are not supported at this time.
            } else if ((operand->flags & PPC_OPERAND_VSR) != 0){
                call_this.fmt += "vs%ld";
                // Booke cpus shouldn't come here
            } else if ((operand->flags & PPC_OPERAND_RELATIVE) != 0){
                call_this.fmt += "0x%lx";
                call_this.arg[i].p = value;
                call_this.arg[i].t = 0;
            } else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0){
                call_this.fmt += "0x%lx";
                call_this.arg[i].p = value;
                call_this.arg[i].t = 0;
            } else if ((operand->flags & PPC_OPERAND_FSL) != 0){
                call_this.fmt += "fsl%ld";
                // Booke cpus shouln't come here
            } else if ((operand->flags & PPC_OPERAND_FCR) != 0){
                call_this.fmt += "fcr%ld";
                // Booke cpus shouldn't come here
            } else if ((operand->flags & PPC_OPERAND_UDI) != 0){
                call_this.fmt += "%ld";
                // Booke cpus shouldn't come here
            } else if ((operand->flags & PPC_OPERAND_CR) != 0 && (m_dialect & PPC_OPCODE_PPC) != 0){
            
                if (operand->bitm == 7)
                    call_this.fmt += "cr%ld";
                else
                    call_this.fmt += "cr[%ld]";
                call_this.arg[i].p = value;
                call_this.arg[i].t = 0;
            }
            else{
                call_this.fmt += "0x%x";
                call_this.arg[i].p = value;
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
        /* We could not find a match. Return with NULL opcode */
        call_this.opcode    = "";
        call_this.arg[0].v  = insn;
        call_this.arg[0].p  = insn;
        call_this.arg[0].t  = 0;
    }

    // Update cache
    m_dis_cache.insert(insn, call_this);

    return call_this;
}

// Disassemble a string representation of instruction
instr_call ppc_dis_booke::disasm(std::string instr)
{
    char *token = NULL;
    char *tmp_str = (char *)instr.c_str();
    char delim = ',';
    instr_call call_this;

    int i=0;
    long value = 0;
    int noperands = 0;
    const struct powerpc_operand* operand = NULL;
    const struct powerpc_opcode*  opcode  = NULL;

    token = find_tok(&tmp_str, ' ');    /* One space is first required between instr and it's operands */
    assert_and_throw(!if_std_delims_present(token), sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Wrong instr" + std::string(token)));

    for(i=0; i<powerpc_num_opcodes; i++){
        if(!strcmp(token, powerpc_opcodes[i].name)){
            opcode = (powerpc_opcodes + i);
            break;
        }
    }

    /* Remove all spaces */
    prune_all_spaces(tmp_str);

    assert_and_throw(opcode != NULL, sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Wrong opcode"));

    // Get opcode's name
    call_this.opcode = std::string(opcode->name);
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
            throw(sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Wrong instruction format"));
        }

        if (i != 0){
            if(*tmp_str != delim){
                printf("Wrong instr format. Correct instr format is \n");
                print_insn_fmt(opcode->name);
                throw(sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Wrong instruction format"));
            }
            tmp_str ++;
        }

        if(tmp_str == NULL || *tmp_str == '\0'){
            printf("Wrong instr format. Correct instr format is \n");
            print_insn_fmt(opcode->name);
            throw(sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Wrong instruction format"));
        }

        delim = get_delim_char(opcode, i);
        token = find_tok(&tmp_str, delim);
        if((token == NULL) || (*token == '\0'))
            break;
        if(if_std_delims_present(token)){
            printf("Wrong instr format. Correct format is \n");
            print_insn_fmt(opcode->name);
            throw(sim_exception(SIM_EXCEPT_ILLEGAL_OP, "Wrong instruction format"));
        }
        
        operand = (powerpc_operands + opcode->operands[i]);
        value = 0;

        if ((operand->flags & PPC_OPERAND_GPR) != 0
            || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && token != 0)){
            sscanf(token, "r%ld", &value);
            call_this.fmt    += "r%ld";
            call_this.arg[i].p = (REG_GPR0 + value);
            call_this.arg[i].t = 1;
        }
        else if ((operand->flags & PPC_OPERAND_FPR) != 0){
            sscanf(token, "f%ld", &value);
            call_this.fmt    += "f%ld";
            call_this.arg[i].p = (REG_FPR0 + value);
            call_this.arg[i].t = 1;
        }
        else if ((operand->flags & PPC_OPERAND_VR) != 0){
            sscanf(token, "v%ld", &value);
            call_this.fmt    += "v%ld";
            // VRs not supported at this time. targ should be specified when supported
        }
        else if ((operand->flags & PPC_OPERAND_VSR) != 0){
            sscanf(token, "vs%ld", &value);
            call_this.fmt    += "vs%ld";
            // Booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_FSL) != 0){
            sscanf(token, "fsl%ld", &value);
            call_this.fmt    += "fsl%ld";
            // booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_FCR) != 0){
            sscanf(token, "fcr%ld", &value);
            call_this.fmt    += "fcr%ld";
            // booke cpus shouldn't come here
        }
        else if ((operand->flags & PPC_OPERAND_UDI) != 0){
            sscanf(token, "0x%lx", &value);
            call_this.fmt    += "0x%lx";
            // booke cpus shouldn't come here
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
                int cr, ind = 0;

                if (cr != 0)
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
            call_this.arg[i].t = 0;
        }
        else{
            sscanf(token, "0x%lx", &value);
            call_this.fmt     += "0x%lx";
            call_this.arg[i].p = value;
            call_this.arg[i].t = 0;
        }

        call_this.fmt += std::string(1, delim);    // Add delimiter to format string

        call_this.arg[i].v = value;
        call_this.nargs++;
    }

    return call_this;

}

// show instruction format
void ppc_dis_booke::print_insn_fmt (const char *insn_name)
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
        int skip_optional;

        if (opcode->operands[0] != 0)
            fprintf(stream, "%-7s ", opcode->name);
        else
            fprintf(stream, "%s", opcode->name);

        /* Now extract and print the operands.  */
        need_comma = 0;
        need_paren = 0;
        skip_optional = -1;
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
            else if ((operand->flags & PPC_OPERAND_FPR) != 0)
                fprintf(stream, "fX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_VR) != 0)
                fprintf(stream, "vX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_VSR) != 0)
                fprintf(stream, "vsX(X=integer)");
            else if ((operand->flags & PPC_OPERAND_RELATIVE) != 0){
                //(memaddr + value, info);
            }
            else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0){
                //(*info->print_address_func) ((bfd_vma) value & 0xffffffff, info);
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
char ppc_dis_booke::get_delim_char(const struct powerpc_opcode *opc, int argno){
    const struct powerpc_operand *curr_opr = (powerpc_operands + opc->operands[argno]);
    const struct powerpc_operand *prev_opr = NULL;
    const struct powerpc_operand *next_opr = (powerpc_operands + opc->operands[argno+1]);

    /* Delimiter for first arg is always comma */
    if(argno == 0){
        return ',';
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

#endif
