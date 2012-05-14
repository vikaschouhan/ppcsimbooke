/*
 *  bclr:  Branch Conditional to Link Register
 *
 *  arg[0] = bo
 *  arg[1] = 31 - bi
 *  arg[2] = bh
 */
X(bclr)
{
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1];
	int ctr_ok, cond_ok;
	uint64_t old_pc = cpu->pc;
	MODE_uint_t tmp, addr = cpu->cd.ppc.spr[SPR_LR];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );
	if (ctr_ok && cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace_return(cpu);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}
X(bclr_20)
{
	cpu->pc = cpu->cd.ppc.spr[SPR_LR];
	quick_pc_to_pointers(cpu);
}
X(bclr_l)
{
	uint64_t low_pc, old_pc = cpu->pc;
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1]  /* ,bh = ic->arg[2]*/;
	int ctr_ok, cond_ok;
	MODE_uint_t tmp, addr = cpu->cd.ppc.spr[SPR_LR];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );

	/*  Calculate return PC:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (ctr_ok && cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace_return(cpu);
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace(cpu, cpu->pc);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}


/*
 *  bcctr:  Branch Conditional to Count register
 *
 *  arg[0] = bo
 *  arg[1] = 31 - bi
 *  arg[2] = bh
 */
X(bcctr)
{
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1];
	uint64_t old_pc = cpu->pc;
	MODE_uint_t addr = cpu->cd.ppc.spr[SPR_CTR];
	int cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );
	if (cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace_return(cpu);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}
X(bcctr_l)
{
	uint64_t low_pc, old_pc = cpu->pc;
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1]  /*,bh = ic->arg[2] */;
	MODE_uint_t addr = cpu->cd.ppc.spr[SPR_CTR];
	int cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );

	/*  Calculate return PC:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace(cpu, cpu->pc);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}


/*
 *  b:  Branch (to a different translated page)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 */
X(b)
{
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (int32_t)ic->arg[0];

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);
}
X(ba)
{
	cpu->pc = (int32_t)ic->arg[0];
	quick_pc_to_pointers(cpu);
}


/*
 *  bc:  Branch Conditional (to a different translated page)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 *  arg[1] = bo
 *  arg[2] = 31-bi
 */
X(bc)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> (bi31m)) & 1)  );
	if (ctr_ok && cond_ok)
		instr(b)(cpu,ic);
}
X(bcl)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	int low_pc;

	/*  Calculate LR:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> bi31m) & 1)  );
	if (ctr_ok && cond_ok)
		instr(b)(cpu,ic);
}


/*
 *  b_samepage:  Branch (to within the same translated page)
 *
 *  arg[0] = pointer to new ppc_instr_call
 */
X(b_samepage)
{
	cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}


/*
 *  bc_samepage:  Branch Conditional (to within the same page)
 *
 *  arg[0] = new ic ptr
 *  arg[1] = bo
 *  arg[2] = 31-bi
 */
X(bc_samepage)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> bi31m) & 1)  );
	if (ctr_ok && cond_ok)
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}
X(bc_samepage_simple0)
{
	int bi31m = ic->arg[2];
	if (!((cpu->cd.ppc.cr >> bi31m) & 1))
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}
X(bc_samepage_simple1)
{
	int bi31m = ic->arg[2];
	if ((cpu->cd.ppc.cr >> bi31m) & 1)
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}
X(bcl_samepage)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	int low_pc;

	/*  Calculate LR:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> bi31m) & 1)  );
	if (ctr_ok && cond_ok)
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}


/*
 *  bl:  Branch and Link (to a different translated page)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl)
{
	/*  Calculate LR and new PC:  */
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc + ic->arg[1];
	cpu->pc += (int32_t)ic->arg[0];

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);
}
X(bla)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->pc = (int32_t)ic->arg[0];
	quick_pc_to_pointers(cpu);
}


/*
 *  bl_trace:  Branch and Link (to a different translated page)  (with trace)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl_trace)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	/*  Calculate new PC from start of page + arg[0]  */
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (int32_t)ic->arg[0];

	cpu_functioncall_trace(cpu, cpu->pc);

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);
}
X(bla_trace)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->pc = (int32_t)ic->arg[0];
	cpu_functioncall_trace(cpu, cpu->pc);
	quick_pc_to_pointers(cpu);
}


/*
 *  bl_samepage:  Branch and Link (to within the same translated page)
 *
 *  arg[0] = pointer to new ppc_instr_call
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl_samepage)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}


/*
 *  bl_samepage_trace:  Branch and Link (to within the same translated page)
 *
 *  arg[0] = pointer to new ppc_instr_call
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl_samepage_trace)
{
	uint32_t low_pc;

	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];

	/*  Calculate new PC (for the trace)  */
	low_pc = ((size_t)cpu->cd.ppc.next_ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call);
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu_functioncall_trace(cpu, cpu->pc);
}
/*
 *  bclr:  Branch Conditional to Link Register
 *
 *  arg[0] = bo
 *  arg[1] = 31 - bi
 *  arg[2] = bh
 */
X(bclr)
{
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1];
	int ctr_ok, cond_ok;
	uint64_t old_pc = cpu->pc;
	MODE_uint_t tmp, addr = cpu->cd.ppc.spr[SPR_LR];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );
	if (ctr_ok && cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace_return(cpu);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}
X(bclr_20)
{
	cpu->pc = cpu->cd.ppc.spr[SPR_LR];
	quick_pc_to_pointers(cpu);
}
X(bclr_l)
{
	uint64_t low_pc, old_pc = cpu->pc;
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1]  /* ,bh = ic->arg[2]*/;
	int ctr_ok, cond_ok;
	MODE_uint_t tmp, addr = cpu->cd.ppc.spr[SPR_LR];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );

	/*  Calculate return PC:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (ctr_ok && cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace_return(cpu);
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace(cpu, cpu->pc);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}


/*
 *  bcctr:  Branch Conditional to Count register
 *
 *  arg[0] = bo
 *  arg[1] = 31 - bi
 *  arg[2] = bh
 */
X(bcctr)
{
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1];
	uint64_t old_pc = cpu->pc;
	MODE_uint_t addr = cpu->cd.ppc.spr[SPR_CTR];
	int cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );
	if (cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace_return(cpu);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}
X(bcctr_l)
{
	uint64_t low_pc, old_pc = cpu->pc;
	unsigned int bo = ic->arg[0], bi31m = ic->arg[1]  /*,bh = ic->arg[2] */;
	MODE_uint_t addr = cpu->cd.ppc.spr[SPR_CTR];
	int cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) == ((cpu->cd.ppc.cr >> bi31m) & 1) );

	/*  Calculate return PC:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (cond_ok) {
		uint64_t mask_within_page =
		    ((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT)
		    | ((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		cpu->pc = addr & ~((1 << PPC_INSTR_ALIGNMENT_SHIFT) - 1);
		/*  TODO: trace in separate (duplicate) function?  */
		if (cpu->machine->show_trace_tree)
			cpu_functioncall_trace(cpu, cpu->pc);
		if ((old_pc  & ~mask_within_page) ==
		    (cpu->pc & ~mask_within_page)) {
			cpu->cd.ppc.next_ic =
			    cpu->cd.ppc.cur_ic_page +
			    ((cpu->pc & mask_within_page) >>
			    PPC_INSTR_ALIGNMENT_SHIFT);
		} else {
			/*  Find the new physical page and update pointers:  */
			quick_pc_to_pointers(cpu);
		}
	}
}


/*
 *  b:  Branch (to a different translated page)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 */
X(b)
{
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (int32_t)ic->arg[0];

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);
}
X(ba)
{
	cpu->pc = (int32_t)ic->arg[0];
	quick_pc_to_pointers(cpu);
}


/*
 *  bc:  Branch Conditional (to a different translated page)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 *  arg[1] = bo
 *  arg[2] = 31-bi
 */
X(bc)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> (bi31m)) & 1)  );
	if (ctr_ok && cond_ok)
		instr(b)(cpu,ic);
}
X(bcl)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	int low_pc;

	/*  Calculate LR:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> bi31m) & 1)  );
	if (ctr_ok && cond_ok)
		instr(b)(cpu,ic);
}


/*
 *  b_samepage:  Branch (to within the same translated page)
 *
 *  arg[0] = pointer to new ppc_instr_call
 */
X(b_samepage)
{
	cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}


/*
 *  bc_samepage:  Branch Conditional (to within the same page)
 *
 *  arg[0] = new ic ptr
 *  arg[1] = bo
 *  arg[2] = 31-bi
 */
X(bc_samepage)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> bi31m) & 1)  );
	if (ctr_ok && cond_ok)
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}
X(bc_samepage_simple0)
{
	int bi31m = ic->arg[2];
	if (!((cpu->cd.ppc.cr >> bi31m) & 1))
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}
X(bc_samepage_simple1)
{
	int bi31m = ic->arg[2];
	if ((cpu->cd.ppc.cr >> bi31m) & 1)
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}
X(bcl_samepage)
{
	MODE_uint_t tmp;
	unsigned int ctr_ok, cond_ok, bi31m = ic->arg[2], bo = ic->arg[1];
	int low_pc;

	/*  Calculate LR:  */
	low_pc = ((size_t)ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call) + 1;
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1)
	    << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);

	if (!(bo & 4))
		cpu->cd.ppc.spr[SPR_CTR] --;
	ctr_ok = (bo >> 2) & 1;
	tmp = cpu->cd.ppc.spr[SPR_CTR];
	ctr_ok |= ( (tmp == 0) == ((bo >> 1) & 1) );
	cond_ok = (bo >> 4) & 1;
	cond_ok |= ( ((bo >> 3) & 1) ==
	    ((cpu->cd.ppc.cr >> bi31m) & 1)  );
	if (ctr_ok && cond_ok)
		cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}


/*
 *  bl:  Branch and Link (to a different translated page)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl)
{
	/*  Calculate LR and new PC:  */
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->cd.ppc.spr[SPR_LR] = cpu->pc + ic->arg[1];
	cpu->pc += (int32_t)ic->arg[0];

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);
}
X(bla)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->pc = (int32_t)ic->arg[0];
	quick_pc_to_pointers(cpu);
}


/*
 *  bl_trace:  Branch and Link (to a different translated page)  (with trace)
 *
 *  arg[0] = relative offset (as an int32_t) from start of page
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl_trace)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	/*  Calculate new PC from start of page + arg[0]  */
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (int32_t)ic->arg[0];

	cpu_functioncall_trace(cpu, cpu->pc);

	/*  Find the new physical page and update the translation pointers:  */
	quick_pc_to_pointers(cpu);
}
X(bla_trace)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->pc = (int32_t)ic->arg[0];
	cpu_functioncall_trace(cpu, cpu->pc);
	quick_pc_to_pointers(cpu);
}


/*
 *  bl_samepage:  Branch and Link (to within the same translated page)
 *
 *  arg[0] = pointer to new ppc_instr_call
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl_samepage)
{
	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];
}


/*
 *  bl_samepage_trace:  Branch and Link (to within the same translated page)
 *
 *  arg[0] = pointer to new ppc_instr_call
 *  arg[1] = lr offset (relative to start of current page)
 */
X(bl_samepage_trace)
{
	uint32_t low_pc;

	/*  Calculate LR:  */
	cpu->cd.ppc.spr[SPR_LR] = (cpu->pc & ~((PPC_IC_ENTRIES_PER_PAGE-1) 
	    << PPC_INSTR_ALIGNMENT_SHIFT)) + ic->arg[1];

	cpu->cd.ppc.next_ic = (struct ppc_instr_call *) ic->arg[0];

	/*  Calculate new PC (for the trace)  */
	low_pc = ((size_t)cpu->cd.ppc.next_ic - (size_t)
	    cpu->cd.ppc.cur_ic_page) / sizeof(struct ppc_instr_call);
	cpu->pc &= ~((PPC_IC_ENTRIES_PER_PAGE-1) << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu->pc += (low_pc << PPC_INSTR_ALIGNMENT_SHIFT);
	cpu_functioncall_trace(cpu, cpu->pc);
}

