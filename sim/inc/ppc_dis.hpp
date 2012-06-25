#ifndef _PPC_DIS_HPP_
#define _PPC_DIS_HPP_

#include "config.h"
#include "third_party/ppcdis/ppc-opc.h"
#include "third_party/ppcdis/ppc-dis.h"

#define  N_PPC_OPCODES  65

class ppc_dis_booke{
    ppc_cpu_t m_dialect;
    static uint16_t m_ppc_opcd_indices[N_PPC_OPCODES];
    static bool     m_opcd_indices_done;

    private:
    // Initialize opcode indices for faster look up
    static void init_opcd_indices(){
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
    void init_dialect(){
       m_dialect = ppc_parse_cpu (0, "e500");
    }

    // Return new dialect according to specified cpu name
    ppc_cpu_t ppc_parse_cpu (ppc_cpu_t ppc_cpu, const char *arg)
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
    long operand_value_powerpc (const struct powerpc_operand *operand, unsigned long insn)
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

    const struct powerpc_opcode * lookup_powerpc (unsigned long insn)
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


    public:
    // constructor
    ppc_dis_booke(){
        init_dialect();
        if(!m_opcd_indices_done){
            init_opcd_indices();
            m_opcd_indices_done = 1;
        }
    }
};

// Static members
uint16_t ppc_dis_booke::m_ppc_opcd_indices[N_PPC_OPCODES];
bool     ppc_dis_booke::m_opcd_indices_done = 0;

#endif
