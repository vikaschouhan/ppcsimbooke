/*
 * Contains register definitions for the Freescale Embedded Performance
 * Monitor.
 */
#ifndef _POWERPC_REG_FSL_EMB_H__
#define _POWERPC_REG_FSL_EMB_H__

#ifndef __ASSEMBLY__
/* Performance Monitor Registers */
#define mfpmr(rn)	({unsigned int rval; \
			asm volatile("mfpmr %0," __stringify(rn) \
				     : "=r" (rval)); rval;})
#define mtpmr(rn, v)	asm volatile("mtpmr " __stringify(rn) ",%0" : : "r" (v))
#endif /* __ASSEMBLY__ */

/* Freescale Book E Performance Monitor APU Registers */
#define PMRN_PMC0	0x010	/* Performance Monitor Counter 0 */
#define PMRN_PMC1	0x011	/* Performance Monitor Counter 1 */
#define PMRN_PMC2	0x012	/* Performance Monitor Counter 1 */
#define PMRN_PMC3	0x013	/* Performance Monitor Counter 1 */
#define PMRN_PMLCA0	0x090	/* PM Local Control A0 */
#define PMRN_PMLCA1	0x091	/* PM Local Control A1 */
#define PMRN_PMLCA2	0x092	/* PM Local Control A2 */
#define PMRN_PMLCA3	0x093	/* PM Local Control A3 */

#define PMLCA_FC	0x80000000	/* Freeze Counter */
#define PMLCA_FCS	0x40000000	/* Freeze in Supervisor */
#define PMLCA_FCU	0x20000000	/* Freeze in User */
#define PMLCA_FCM1	0x10000000	/* Freeze when PMM==1 */
#define PMLCA_FCM0	0x08000000	/* Freeze when PMM==0 */
#define PMLCA_CE	0x04000000	/* Condition Enable */

#define PMLCA_EVENT_MASK 0x00ff0000	/* Event field */
#define PMLCA_EVENT_SHIFT	16

#define PMRN_PMLCB0	0x110	/* PM Local Control B0 */
#define PMRN_PMLCB1	0x111	/* PM Local Control B1 */
#define PMRN_PMLCB2	0x112	/* PM Local Control B2 */
#define PMRN_PMLCB3	0x113	/* PM Local Control B3 */

#define PMLCB_THRESHMUL_MASK	0x0700	/* Threshold Multiple Field */
#define PMLCB_THRESHMUL_SHIFT	8

#define PMLCB_THRESHOLD_MASK	0x003f	/* Threshold Field */
#define PMLCB_THRESHOLD_SHIFT	0

#define PMRN_PMGC0	0x190	/* PM Global Control 0 */

#define PMGC0_FAC	0x80000000	/* Freeze all Counters */
#define PMGC0_PMIE	0x40000000	/* Interrupt Enable */
#define PMGC0_FCECE	0x20000000	/* Freeze countes on
					   Enabled Condition or
					   Event */

#define PMRN_UPMC0	0x000	/* User Performance Monitor Counter 0 */
#define PMRN_UPMC1	0x001	/* User Performance Monitor Counter 1 */
#define PMRN_UPMC2	0x002	/* User Performance Monitor Counter 1 */
#define PMRN_UPMC3	0x003	/* User Performance Monitor Counter 1 */
#define PMRN_UPMLCA0	0x080	/* User PM Local Control A0 */
#define PMRN_UPMLCA1	0x081	/* User PM Local Control A1 */
#define PMRN_UPMLCA2	0x082	/* User PM Local Control A2 */
#define PMRN_UPMLCA3	0x083	/* User PM Local Control A3 */
#define PMRN_UPMLCB0	0x100	/* User PM Local Control B0 */
#define PMRN_UPMLCB1	0x101	/* User PM Local Control B1 */
#define PMRN_UPMLCB2	0x102	/* User PM Local Control B2 */
#define PMRN_UPMLCB3	0x103	/* User PM Local Control B3 */
#define PMRN_UPMGC0	0x180	/* User PM Global Control 0 */


#endif /* _POWERPC_REG_FSL_EMB_H__ */
