/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1997, 1998, 1999, 2000, 2001 by Ralf Baechle
 * Copyright (C) 2001 MIPS Technologies, Inc.
 */
#include <asm/asm.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>
#include <asm/isadep.h>
#include <asm/sysmips.h>
#include <asm/unistd.h>

/* This duplicates the definition from <linux/sched.h> */
#define PT_TRACESYS	0x00000002	/* tracing system calls */

/* This duplicates the definition from <asm/signal.h> */
#define SIGILL		4		/* Illegal instruction (ANSI).  */

/* Highest syscall used of any syscall flavour */
#define MAX_SYSCALL_NO	__NR_Linux + __NR_Linux_syscalls

#ifdef CONFIG_CPU_R5900_CONTEXT
#define S_GREG  sq
#define L_GREG  lq
#else
#define S_GREG  sw
#define L_GREG  lw
#endif

	.align  5
NESTED(handle_sys, PT_SIZE, sp)
	.set	noat
	SAVE_SOME
	STI
	.set	at

	lw	t1, PT_EPC(sp)		# skip syscall on return

	sltiu	t0, v0, MAX_SYSCALL_NO + 1 # check syscall number
	addiu	t1, 4			# skip to next instruction
	beqz	t0, illegal_syscall
	sw	t1, PT_EPC(sp)

	/* XXX Put both in one cacheline, should save a bit. */
	sll	t0, v0, 2
	lw	t2, sys_call_table(t0)	# syscall routine
	lbu	t3, sys_narg_table(v0)	# number of arguments
	beqz	t2, illegal_syscall;

	subu	t0, t3, 5		# 5 or more arguments?
	S_GREG	a3, PT_R26(sp)		# save a3 for syscall restarting
	bgez	t0, stackargs

stack_done:
        S_GREG	a3, PT_R26(sp)          # save for syscall restart
	lw	t0, TASK_PTRACE($28)	# syscall tracing enabled?
	andi	t0, PT_TRACESYS
	bnez	t0, trace_a_syscall

	jalr	t2			# Do The Real Thing (TM)

	li	t0, -EMAXERRNO - 1	# error?
	sltu	t0, t0, v0
	S_GREG	t0, PT_R7(sp)		# set error flag
	beqz	t0, 1f

	negu	v0			# error
	S_GREG	v0, PT_R0(sp)		# set flag for syscall restarting
1:	S_GREG	v0, PT_R2(sp)		# result

EXPORT(o32_ret_from_sys_call)
	mfc0	t0, CP0_STATUS		# need_resched and signals atomic test
	ori	t0, t0, 1
	xori	t0, t0, 1
	MTC0_asm(t0, CP0_STATUS)

	lw	t2, TASK_NEED_RESCHED($28)
	bnez	t2, o32_reschedule
	lw	v0, TASK_SIGPENDING($28)
	bnez	v0, signal_return
restore_all:
	RESTORE_SOME
	RESTORE_SP_AND_RET

/* Put this behind restore_all for the sake of the branch prediction.  */
signal_return:
	.type	signal_return, @function

	mfc0	t0, CP0_STATUS
	ori	t0, t0, 1
	MTC0_asm(t0, CP0_STATUS)

	SAVE_STATIC
	move	a0, zero
	move	a1, sp
	jal	do_signal
	b	restore_all

o32_reschedule:
	jal	schedule
	b	o32_ret_from_sys_call

/* ------------------------------------------------------------------------ */

trace_a_syscall:
	SAVE_STATIC
	S_GREG	t2, PT_R1(sp)
	jal	syscall_trace
	L_GREG	t2, PT_R1(sp)

	L_GREG	a0, PT_R4(sp)		# Restore argument registers
	L_GREG	a1, PT_R5(sp)
	L_GREG	a2, PT_R6(sp)
	L_GREG	a3, PT_R7(sp)
	jalr	t2

	li	t0, -EMAXERRNO - 1	# error?
	sltu	t0, t0, v0
	S_GREG	t0, PT_R7(sp)		# set error flag
	beqz	t0, 1f

	negu	v0			# error
	S_GREG	v0, PT_R0(sp)		# set flag for syscall restarting
1:	S_GREG	v0, PT_R2(sp)		# result

	jal	syscall_trace
	j	ret_from_sys_call

/* ------------------------------------------------------------------------ */

	/*
	 * More than four arguments.  Try to deal with it by copying the
	 * stack arguments from the user stack to the kernel stack.
	 * This Sucks (TM).
	 */
stackargs:
	L_GREG	t0, PT_R29(sp)		# get old user stack pointer
	subu	t3, 4
	sll	t1, t3, 2		# stack valid?

	addu	t1, t0			# end address
	or	t0, t1
	bltz	t0, bad_stack		# -> sp is bad

	L_GREG	t0, PT_R29(sp)		# get old user stack pointer
	la	t1, 3f			# copy 1 to 2 arguments
	sll	t3, t3, 4
	subu	t1, t3
	jr	t1

	/* Ok, copy the args from the luser stack to the kernel stack */
	/*
	 * I know Ralf doesn't like nops but this avoids code
	 * duplication for R3000 targets (and this is the
	 * only place where ".set reorder" doesn't help).
	 * Harald.
	 */
	.set    push
	.set    noreorder
1:	lw	t1, 20(t0)		# argument #6 from usp
	nop
	sw	t1, 20(sp)
	nop
2:	lw	t1, 16(t0)		# argument #5 from usp
	nop
	sw	t1, 16(sp)
	nop
	.set	pop

3:	j	stack_done		# go back

	.section __ex_table,"a"
	PTR	1b,bad_stack
	PTR	2b,bad_stack
	.previous

	/*
	 * The stackpointer for a call with more than 4 arguments is bad.
	 * We probably should handle this case a bit more drastic.
	 */
bad_stack:
	negu	v0				# error
	S_GREG	v0, PT_R0(sp)
	S_GREG	v0, PT_R2(sp)
	li	t0, 1				# set error flag
	S_GREG	t0, PT_R7(sp)
	j	ret_from_sys_call

	/*
	 * The system call does not exist in this kernel
	 */
illegal_syscall:
	li	v0, ENOSYS			# error
	S_GREG	v0, PT_R2(sp)
	li	t0, 1				# set error flag
	S_GREG	t0, PT_R7(sp)
	j	ret_from_sys_call
	END(handle_sys)

	LEAF(mips_atomic_set)
	andi	v0, a1, 3			# must be word aligned
	bnez	v0, bad_alignment

	lw	v1, THREAD_CURDS($28)		# in legal address range?
	addiu	a0, a1, 4
	or	a0, a0, a1
	and	a0, a0, v1
	bltz	a0, bad_address

#ifdef CONFIG_CPU_HAS_LLSC
	/* Ok, this is the ll/sc case.  World is sane :-)  */
1:	ll	v0, (a1)
	move	a0, a2
2:	sc	a0, (a1)
	beqz	a0, 1b

	.section __ex_table,"a"
	PTR	1b, bad_stack
	PTR	2b, bad_stack
	.previous
#else
	sw	a1, 16(sp)
	sw	a2, 20(sp)

	move	a0, sp
	move	a2, a1
	li	a1, 1
	jal	do_page_fault

	lw	a1, 16(sp)
	lw	a2, 20(sp)

	/*
	 * At this point the page should be readable and writable unless
	 * there was no more memory available.
	 */
	CLI
1:	lw	v0, (a1)
2:	sw	a2, (a1)
	STI

	.section __ex_table,"a"
	PTR	1b, no_mem
	PTR	2b, no_mem
	.previous
#endif

	S_GREG	v0, PT_R2(sp)		# result
1:

	/* Success, so skip usual error handling garbage.  */
	lw	t0, TASK_PTRACE($28)	# syscall tracing enabled?
	andi	t0, PT_TRACESYS
	bnez	t0, 1f
	b	o32_ret_from_sys_call

1:	SAVE_STATIC
	jal	syscall_trace
	li	a3, 0			# success
	j	ret_from_sys_call

no_mem:	li	v0, -ENOMEM
	jr	ra

bad_address:
	li	v0, -EFAULT
	jr	ra

bad_alignment:
	li	v0, -EINVAL
	jr	ra
	END(mips_atomic_set)	

	LEAF(mips_atomic_compare_and_set)
	andi	v0, a1, 3			# must be word aligned
	bnez	v0, bad_alignment

	lw	v1, THREAD_CURDS($28)		# in legal address range?
	addiu	a0, a1, 4
	or	a0, a0, a1
	and	a0, a0, v1
	bltz	a0, bad_address

#ifdef CONFIG_CPU_HAS_LLSC
	/* Ok, this is the ll/sc case.  World is sane :-)  */
1:	ll	a0, (a1)
	move	v0, zero
	bne	a0, a2, 3f
	move	v0, a3
2:	sc	v0, (a1)
	beqz	v0, 1b
3:
	.section __ex_table,"a"
	PTR	1b, bad_stack
	PTR	2b, bad_stack
	.previous
#else
	sw	a1, 16(sp)
	sw	a2, 20(sp)
	sw	a3, 24(sp)

	move	a0, sp
	move	a2, a1
	li	a1, 1
	jal	do_page_fault

	lw	a1, 16(sp)
	lw	a2, 20(sp)
	lw	a3, 24(sp)

	/*
	 * At this point the page should be readable and writable unless
	 * there was no more memory available.
	 */
	CLI
1:	lw	v0, (a1)
	move	v0, zero
	bne	v0, a2, 3f
2:	sw	a3, (a1)
	li	v0, 1
3:
	STI

	.section __ex_table,"a"
	PTR	1b, no_mem
	PTR	2b, no_mem
	.previous
#endif

	sw	v0, PT_R2(sp)		# result
1:

	/* Success, so skip usual error handling garbage.  */
	lw	t0, TASK_PTRACE($28)	# syscall tracing enabled?
	andi	t0, PT_TRACESYS
	bnez	t0, 1f
	b	o32_ret_from_sys_call

1:	SAVE_STATIC
	jal	syscall_trace
	li	a3, 0			# success
	j	ret_from_sys_call

	END(mips_atomic_compare_and_set)

	LEAF(sys_sysmips)
	beq	a0, MIPS_ATOMIC_SET, mips_atomic_set
	beq	a0, MIPS_ATOMIC_COMPARE_AND_SWAP, mips_atomic_compare_and_set
	j	_sys_sysmips
	END(sys_sysmips)
