/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 *  Copyright (C) 1994, 1995, 1996, 2001 Ralf Baechle
 *  Copyright (C) 1994, 1995, 1996 Paul M. Antoine.
 */
#ifndef __ASM_STACKFRAME_H
#define __ASM_STACKFRAME_H

#include <asm/addrspace.h>
#include <asm/mipsregs.h>
#include <asm/processor.h>
#include <asm/asm.h>
#include <asm/offset.h>
#include <linux/config.h>

#if defined (CONFIG_MIPS_TAS_DEV) || defined (CONFIG_MIPS_TAS_DEV_MODULE)
#include <linux/tas_dev.h>
#endif

#ifdef TAS_NEEDS_EXCEPTION_EPILOGUE
#define		TAS_EXCEPTION_EPILOGUE \
		li	k1, _TAS_ACCESS_MAGIC;
#else
#define		TAS_EXCEPTION_EPILOGUE
#endif

#ifdef CONFIG_CPU_R5900_CONTEXT
#define L_GREG	lq
#define S_GREG	sq
#define MOVE(dst,src)	por	dst, src, $0
#else
#define L_GREG	lw
#define S_GREG	sw
#define MOVE(dst,src)	move	dst, src
#endif

#define SAVE_AT                                          \
		.set	push;                            \
		.set	noat;                            \
		S_GREG	$1, PT_R1(sp);                   \
		.set	pop

#ifdef CONFIG_CPU_R5900_CONTEXT
#define SAVE_TEMP                                        \
		pmfhi	v1;                              \
		S_GREG	$8, PT_R8(sp);                   \
		S_GREG	$9, PT_R9(sp);                   \
		S_GREG	v1, PT_HI(sp);                   \
		pmflo	v1;                              \
		S_GREG	$10,PT_R10(sp);                  \
		S_GREG	$11, PT_R11(sp);                 \
		S_GREG	v1,  PT_LO(sp);                  \
		S_GREG	$12, PT_R12(sp);                 \
		S_GREG	$13, PT_R13(sp);                 \
		S_GREG	$14, PT_R14(sp);                 \
		S_GREG	$15, PT_R15(sp);                 \
		mfsa	v1;                              \
		S_GREG	$24, PT_R24(sp);                 \
		sw	v1,  PT_SA(sp)
#else
#define SAVE_TEMP                                        \
		mfhi	v1;                              \
		S_GREG	$8, PT_R8(sp);                   \
		S_GREG	$9, PT_R9(sp);                   \
		S_GREG	v1, PT_HI(sp);                   \
		mflo	v1;                              \
		S_GREG	$10,PT_R10(sp);                  \
		S_GREG	$11, PT_R11(sp);                 \
		S_GREG	v1,  PT_LO(sp);                  \
		S_GREG	$12, PT_R12(sp);                 \
		S_GREG	$13, PT_R13(sp);                 \
		S_GREG	$14, PT_R14(sp);                 \
		S_GREG	$15, PT_R15(sp);                 \
		S_GREG	$24, PT_R24(sp)
#endif

#define SAVE_STATIC                                      \
		S_GREG	$16, PT_R16(sp);                 \
		S_GREG	$17, PT_R17(sp);                 \
		S_GREG	$18, PT_R18(sp);                 \
		S_GREG	$19, PT_R19(sp);                 \
		S_GREG	$20, PT_R20(sp);                 \
		S_GREG	$21, PT_R21(sp);                 \
		S_GREG	$22, PT_R22(sp);                 \
		S_GREG	$23, PT_R23(sp);                 \
		S_GREG	$30, PT_R30(sp)

#define __str2(x) #x
#define __str(x) __str2(x)

#ifdef CONFIG_CPU_R5900_CONTEXT
#define save_static_function(symbol)                                    \
__asm__ (                                                               \
        ".globl\t" #symbol "\n\t"                                       \
        ".align\t2\n\t"                                                 \
        ".type\t" #symbol ", @function\n\t"                             \
        ".ent\t" #symbol ", 0\n"                                        \
        #symbol":\n\t"                                                  \
        ".frame\t$29, 0, $31\n\t"                                       \
        "sq\t$16,"__str(PT_R16)"($29)\t\t\t# save_static_function\n\t"  \
        "sq\t$17,"__str(PT_R17)"($29)\n\t"                              \
        "sq\t$18,"__str(PT_R18)"($29)\n\t"                              \
        "sq\t$19,"__str(PT_R19)"($29)\n\t"                              \
        "sq\t$20,"__str(PT_R20)"($29)\n\t"                              \
        "sq\t$21,"__str(PT_R21)"($29)\n\t"                              \
        "sq\t$22,"__str(PT_R22)"($29)\n\t"                              \
        "sq\t$23,"__str(PT_R23)"($29)\n\t"                              \
        "sq\t$30,"__str(PT_R30)"($29)\n\t"                              \
        ".end\t" #symbol "\n\t"                                         \
        ".size\t" #symbol",. - " #symbol)
#else
#define save_static_function(symbol)                                    \
__asm__ (                                                               \
        ".globl\t" #symbol "\n\t"                                       \
        ".align\t2\n\t"                                                 \
        ".type\t" #symbol ", @function\n\t"                             \
        ".ent\t" #symbol ", 0\n"                                        \
        #symbol":\n\t"                                                  \
        ".frame\t$29, 0, $31\n\t"                                       \
        "sw\t$16,"__str(PT_R16)"($29)\t\t\t# save_static_function\n\t"  \
        "sw\t$17,"__str(PT_R17)"($29)\n\t"                              \
        "sw\t$18,"__str(PT_R18)"($29)\n\t"                              \
        "sw\t$19,"__str(PT_R19)"($29)\n\t"                              \
        "sw\t$20,"__str(PT_R20)"($29)\n\t"                              \
        "sw\t$21,"__str(PT_R21)"($29)\n\t"                              \
        "sw\t$22,"__str(PT_R22)"($29)\n\t"                              \
        "sw\t$23,"__str(PT_R23)"($29)\n\t"                              \
        "sw\t$30,"__str(PT_R30)"($29)\n\t"                              \
        ".end\t" #symbol "\n\t"                                         \
        ".size\t" #symbol",. - " #symbol)
#endif

/* Used in declaration of save_static functions.  */
#define static_unused static __attribute__((unused))


#ifdef CONFIG_SMP
#  define GET_SAVED_SP                                   \
                mfc0    k0, CP0_CONTEXT;                 \
                lui     k1, %hi(kernelsp);               \
                srl     k0, k0, 23;                      \
		sll     k0, k0, 2;                       \
                addu    k1, k0;                          \
                lw      k1, %lo(kernelsp)(k1);        

#else
#  define GET_SAVED_SP                                   \
		lui	k1, %hi(kernelsp);               \
		lw	k1, %lo(kernelsp)(k1);           
#endif
 
#if defined(CONFIG_CPU_LX45XXX)                         
#define SAVE_SOME                                        \
		.set	push;                            \
		.set	reorder;                         \
		mfc0	k0, CP0_STATUS;                  \
		sll	k0, 3;     /* extract cu0 bit */ \
		.set	noreorder;                       \
		bltz	k0, 8f;                          \
		 move	k1, sp;                          \
		.set	reorder;                         \
		/* Called from user mode, new stack. */  \
		GET_SAVED_SP                             \
8:                                                       \
		move	k0, sp;                          \
		subu	sp, k1, PT_SIZE;                 \
		sw	k0, PT_R29(sp);                  \
		sw	$3, PT_R3(sp);                   \
		sw	$0, PT_R0(sp);			 \
		mfc0	v1, CP0_STATUS;                  \
		sw	$2, PT_R2(sp);                   \
		sw	v1, PT_STATUS(sp);               \
		sw	$4, PT_R4(sp);                   \
		mfc0	v1, CP0_CAUSE;                   \
		sw	$5, PT_R5(sp);                   \
		sw	v1, PT_CAUSE(sp);                \
		sw	$6, PT_R6(sp);                   \
		mfc0	v1, CP0_EPC;                     \
		sw	$7, PT_R7(sp);                   \
		sw	v1, PT_EPC(sp);                  \
		sw	$25, PT_R25(sp);                 \
		.word	0x40630800;                      \
		sw	$28, PT_R28(sp);                 \
		sw	v1, PT_ECAUSE(sp);               \
		sw	$31, PT_R31(sp);                 \
		.word	0x40630000;                      \
		 nop;                                    \
		sw	v1, PT_ESTATUS(sp);              \
		 nop;                                    \
		ori	$28, sp, 0x1fff;                 \
		xori	$28, 0x1fff;                     \
		.set	pop

#else

#define SAVE_SOME                                        \
		.set	push;                            \
		.set	reorder;                         \
		mfc0	k0, CP0_STATUS;                  \
		sll	k0, 3;     /* extract cu0 bit */ \
		.set	noreorder;                       \
		bltz	k0, 8f;                          \
		 move	k1, sp;                          \
		.set	reorder;                         \
		/* Called from user mode, new stack. */  \
                GET_SAVED_SP                             \
8:                                                       \
		MOVE	(k0, sp);                        \
		subu	sp, k1, PT_SIZE;                 \
		S_GREG	k0, PT_R29(sp);                  \
                S_GREG	$3, PT_R3(sp);                   \
		S_GREG	$0, PT_R0(sp);			 \
		mfc0	v1, CP0_STATUS;                  \
		S_GREG	$2, PT_R2(sp);                   \
		sw	v1, PT_STATUS(sp);               \
		S_GREG	$4, PT_R4(sp);                   \
		mfc0	v1, CP0_CAUSE;                   \
		S_GREG	$5, PT_R5(sp);                   \
		sw	v1, PT_CAUSE(sp);                \
		S_GREG	$6, PT_R6(sp);                   \
		mfc0	v1, CP0_EPC;                     \
		S_GREG	$7, PT_R7(sp);                   \
		sw	v1, PT_EPC(sp);                  \
		S_GREG	$25, PT_R25(sp);                 \
		S_GREG	$28, PT_R28(sp);                 \
		S_GREG	$31, PT_R31(sp);                 \
		ori	$28, sp, 0x1fff;                 \
		xori	$28, 0x1fff;                     \
		.set	pop
#endif /*CONFIG_CPU_LX45XXX*/ 

#define SAVE_ALL                                         \
		SAVE_SOME;                               \
		SAVE_AT;                                 \
		SAVE_TEMP;                               \
		SAVE_STATIC

#define RESTORE_AT                                       \
		.set	push;                            \
		.set	noat;                            \
		L_GREG	$1,  PT_R1(sp);                  \
		.set	pop;

#ifdef CONFIG_CPU_R5900_CONTEXT
#define RESTORE_TEMP                                     \
		L_GREG	$24, PT_LO(sp);                  \
		L_GREG	$8, PT_R8(sp);                   \
		pmtlo	$24;                             \
		L_GREG	$9, PT_R9(sp);                   \
		L_GREG	$24, PT_HI(sp);                  \
		L_GREG	$10,PT_R10(sp);                  \
		pmthi	$24;                             \
		L_GREG	$11, PT_R11(sp);                 \
		L_GREG	$12, PT_R12(sp);                 \
		L_GREG	$13, PT_R13(sp);                 \
		lw	$24, PT_SA(sp);                  \
		L_GREG	$14, PT_R14(sp);                 \
		mtsa	$24;                             \
		L_GREG	$15, PT_R15(sp);                 \
		L_GREG	$24, PT_R24(sp)
#else
#define RESTORE_TEMP                                     \
		L_GREG	$24, PT_LO(sp);                  \
		L_GREG	$8, PT_R8(sp);                   \
		L_GREG	$9, PT_R9(sp);                   \
		mtlo	$24;                             \
		L_GREG	$24, PT_HI(sp);                  \
		L_GREG	$10,PT_R10(sp);                  \
		L_GREG	$11, PT_R11(sp);                 \
		mthi	$24;                             \
		L_GREG	$12, PT_R12(sp);                 \
		L_GREG	$13, PT_R13(sp);                 \
		L_GREG	$14, PT_R14(sp);                 \
		L_GREG	$15, PT_R15(sp);                 \
		L_GREG	$24, PT_R24(sp)
#endif

#define RESTORE_STATIC                                   \
		L_GREG	$16, PT_R16(sp);                 \
		L_GREG	$17, PT_R17(sp);                 \
		L_GREG	$18, PT_R18(sp);                 \
		L_GREG	$19, PT_R19(sp);                 \
		L_GREG	$20, PT_R20(sp);                 \
		L_GREG	$21, PT_R21(sp);                 \
		L_GREG	$22, PT_R22(sp);                 \
		L_GREG	$23, PT_R23(sp);                 \
		L_GREG	$30, PT_R30(sp)

#if defined(CONFIG_CPU_LX45XXX)

#define RESTORE_SOME                                  \
		.set	push;                         \
		.set	reorder;                      \
		mfc0	t0, CP0_STATUS;               \
		.set	pop;                          \
		ori	t0, 0x1f;                     \
		xori	t0, 0x1f;                     \
		mtc0	t0, CP0_STATUS;               \
		li	v1, 0xff00;                   \
		and	t0, v1;	                      \
		lw	v0, PT_STATUS(sp);            \
		nor	v1, $0, v1;                   \
		and	v0, v1;                       \
		or	v0, t0;                       \
		mtc0	v0, CP0_STATUS;               \
		.word	0x40680000;                   \
		lui     v1, 0xff;                     \
		and     t0, v1;                       \
		lw      v0, PT_ESTATUS(sp);           \
		nor     v1, $0, v1;                   \
		and     v0, v1;                       \
		or      v0, t0;                       \
		.word   0x40e20000;                   \
		lw	$31, PT_R31(sp);              \
		lw	$28, PT_R28(sp);              \
		lw	$25, PT_R25(sp);              \
		lw	$7,  PT_R7(sp);               \
		lw	$6,  PT_R6(sp);               \
		lw	$5,  PT_R5(sp);               \
		lw	$4,  PT_R4(sp);               \
		lw	$3,  PT_R3(sp);               \
		lw	$2,  PT_R2(sp)

#define RESTORE_SP_AND_RET			         \
     		.set	push;				 \
     		.set	noreorder;			 \
     		lw	k0, PT_EPC(sp);                  \
     		lw	sp,  PT_R29(sp);                 \
		TAS_EXCEPTION_EPILOGUE		         \
     		jr	k0;                              \
     		 rfe;					 \
     		.set	pop
     
#elif defined(CONFIG_CPU_R3000) || defined(CONFIG_CPU_TX39XX)

#define RESTORE_SOME                                     \
		.set	push;                            \
		.set	reorder;                         \
		mfc0	t0, CP0_STATUS;                  \
		.set	pop;                             \
		ori	t0, 0x1f;                        \
		xori	t0, 0x1f;                        \
		mtc0	t0, CP0_STATUS;                  \
		li	v1, 0xff00;                      \
		and	t0, v1;				 \
		lw	v0, PT_STATUS(sp);               \
		nor	v1, $0, v1;			 \
		and	v0, v1;				 \
		or	v0, t0;				 \
		mtc0	v0, CP0_STATUS;                  \
		lw	$31, PT_R31(sp);                 \
		lw	$28, PT_R28(sp);                 \
		lw	$25, PT_R25(sp);                 \
		lw	$7,  PT_R7(sp);                  \
		lw	$6,  PT_R6(sp);                  \
		lw	$5,  PT_R5(sp);                  \
		lw	$4,  PT_R4(sp);                  \
		lw	$3,  PT_R3(sp);                  \
		lw	$2,  PT_R2(sp)

#define RESTORE_SP_AND_RET                               \
		.set	push;				 \
		.set	noreorder;			 \
		lw	k0, PT_EPC(sp);                  \
		lw	sp,  PT_R29(sp);                 \
		TAS_EXCEPTION_EPILOGUE		         \
		jr	k0;                              \
		 rfe;					 \
		.set	pop

#else

#define RESTORE_SOME                                     \
		.set	push;                            \
		.set	reorder;                         \
		mfc0	t0, CP0_STATUS;                  \
		.set	pop;                             \
		ori	t0, 0x1f;                        \
		xori	t0, 0x1f;                        \
		MTC0_asm(t0, CP0_STATUS);                  \
		li	v1, 0xff00;                      \
		and	t0, v1;				 \
		lw	v0, PT_STATUS(sp);               \
		nor	v1, $0, v1;			 \
		and	v0, v1;				 \
		or	v0, t0;				 \
		MTC0_asm(v0, CP0_STATUS);                  \
		lw	v1, PT_EPC(sp);                  \
		MTC0_asm(v1, CP0_EPC);                     \
		L_GREG	$31, PT_R31(sp);                 \
		L_GREG	$28, PT_R28(sp);                 \
		L_GREG	$25, PT_R25(sp);                 \
		L_GREG	$7,  PT_R7(sp);                  \
		L_GREG	$6,  PT_R6(sp);                  \
		L_GREG	$5,  PT_R5(sp);                  \
		L_GREG	$4,  PT_R4(sp);                  \
		L_GREG	$3,  PT_R3(sp);                  \
		L_GREG	$2,  PT_R2(sp)

#define RESTORE_SP_AND_RET                               \
		L_GREG	sp,  PT_R29(sp);                 \
		TAS_EXCEPTION_EPILOGUE		         \
		.set	mips3;				 \
		eret;					 \
		.set	mips0

#endif

#define RESTORE_SP                                       \
		L_GREG	sp,  PT_R29(sp);                 \

#define RESTORE_ALL                                      \
		RESTORE_SOME;                            \
		RESTORE_AT;                              \
		RESTORE_TEMP;                            \
		RESTORE_STATIC;                          \
		RESTORE_SP

#define RESTORE_ALL_AND_RET                              \
		RESTORE_SOME;                            \
		RESTORE_AT;                              \
		RESTORE_TEMP;                            \
		RESTORE_STATIC;                          \
		RESTORE_SP_AND_RET


/*
 * Move to kernel mode and disable interrupts.
 * Set cp0 enable bit as sign that we're running on the kernel stack
 */
#define CLI                                             \
		mfc0	t0,CP0_STATUS;                  \
		li	t1,ST0_CU0|0x1f;                \
		or	t0,t1;                          \
		xori	t0,0x1f;                        \
		MTC0_asm(t0,CP0_STATUS)

/*
 * Move to kernel mode and enable interrupts.
 * Set cp0 enable bit as sign that we're running on the kernel stack
 */
#define STI                                             \
		mfc0	t0,CP0_STATUS;                  \
		li	t1,ST0_CU0|0x1f;                \
		or	t0,t1;                          \
		xori	t0,0x1e;                        \
		MTC0_asm(t0,CP0_STATUS)

/*
 * Just move to kernel mode and leave interrupts as they are.
 * Set cp0 enable bit as sign that we're running on the kernel stack
 */
#define KMODE                                           \
		mfc0	t0,CP0_STATUS;                  \
		li	t1,ST0_CU0|0x1e;                \
		or	t0,t1;                          \
		xori	t0,0x1e;                        \
		MTC0_asm(t0,CP0_STATUS)

#endif /* __ASM_STACKFRAME_H */
