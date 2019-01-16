/* arch_test_ptrace.c COPYRIGHT FUJITSU LIMITED 2016 */
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <linux/elf.h>
#include <sys/uio.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <errno.h>
#include "arch_test_ptrace.h"
#include "arch_test_fpregs.h"
#include "arch_test_mck.h"

unsigned long get_or_do_inst(int mode)
{
	unsigned long addr = -1;

	switch (mode) {
	case MODE_GET_INST_ADDR:
		asm volatile(
			"adr x10, 1f\n"
			"str x10, [%0]\n"
			: /* nothing */
			: "r"(&addr)
			: "x10"
		);
		break;

	case MODE_DO_INST:
		asm volatile(
			"nop\n"
			"nop\n"
			"1:\n"
			"nop\n"
			"nop\n"
			: /* nothing */
			: /* nothing */
		);
		addr = 0;
		break;

	default:
		break;
	}
	return addr;
}

unsigned long get_or_do_store_inst(int mode, unsigned long *buf, unsigned long change)
{
	unsigned long addr = -1;

	switch (mode) {
	case MODE_GET_INST_ADDR:
		asm volatile(
			"adr x10, 2f\n"
			"str x10, [%0]\n"
			: /* nothing */
			: "r"(&addr)
			: "x10"
		);
		break;

	case MODE_DO_STORE_INST:
		if (!buf) {
			break;
		}
		asm volatile(
			"mov x10, %0\n"
			"2:\n"
			"str x10, [%1]\n"
			: /* nothing */
			: "r"(change), "r"(buf)
			: "x10"
		);
		addr = 0;
		break;

	default:
		break;
	}
	return addr;
}

void pre_getregset(void)
{
	struct fpregs fpregs;

	/* write gregs */
	register_write();

	/* write fpregs */
	memcpy(&fpregs, FPREGS_TEST_DATA, sizeof(struct fpregs));
	fpregs_write(&fpregs);

	return;
}

int getregset_check(pid_t cpid)
{
	int ret = -1;
	int remain = 0;
	int i = 0;
	struct iovec iov;
	struct user_pt_regs gregs;
	struct user_fpsimd_state fregs;
	unsigned long tls_val;
	struct user_hwdebug_state bpoint_regs;
	struct user_hwdebug_state wpoint_regs;
	int syscallno;

	/* read child regs (REGSET_GPR) */
	iov.iov_base = &gregs;
	iov.iov_len = sizeof(gregs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_PRSTATUS, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_PRSTATUS)");
		goto out;
	}

	printf("===== DUMP REGSET_GPR =====\n");
	remain = sizeof(gregs.regs) / sizeof(gregs.regs[0]);
	for (i = 0; i < remain; i++) {
		printf("  x%02d = 0x%016llx\n", i, gregs.regs[i]);
	}

	printf("  sp  = 0x%016llx\n", gregs.sp);
	printf("  pc  = 0x%016llx\n", gregs.pc);
	printf("  pstate = 0x%016llx\n", gregs.pstate);
	printf("\n");

	/* read child regs (REGSET_FPR) */
	iov.iov_base = &fregs;
	iov.iov_len = sizeof(fregs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_PRFPREG, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_PRFPREG)");
		goto out;
	}

	printf("===== DUMP REGSET_FPR =====\n");
	remain = sizeof(fregs.vregs) / sizeof(fregs.vregs[0]);
	for (i = 0; i < remain; i++) {
		printf("  q%02d = 0x%016lx %016lx\n",
			i, (unsigned long)(fregs.vregs[i] >> 64), (unsigned long)fregs.vregs[i]);
	}

	printf("  fpsr = 0x%08x\n", fregs.fpsr);
	printf("  fpcr = 0x%08x\n", fregs.fpcr);
	printf("\n");

	/* read child regs (REGSET_TLS) */
	iov.iov_base = &tls_val;
	iov.iov_len = sizeof(tls_val);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_TLS, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_TLS)");
		goto out;
	}

	printf("===== DUMP REGSET_TLS =====\n");
	printf("  tls = 0x%016lx\n", tls_val);
	printf("\n");

	/* read child regs (REGSET_HW_BREAK) */
	iov.iov_base = &bpoint_regs;
	iov.iov_len = sizeof(bpoint_regs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_HW_BREAK, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_HW_BREAK)");
		goto out;
	}

	printf("===== DUMP REGSET_HW_BREAK =====\n");
	printf("  dbg_info = 0x%08x\n", bpoint_regs.dbg_info);

	remain = sizeof(bpoint_regs.dbg_regs) / sizeof(bpoint_regs.dbg_regs[0]);
	for (i = 0; i < remain; i++) {
		printf("  dbg_regs[%02d].addr = 0x%016llx\n", i, bpoint_regs.dbg_regs[i].addr);
		printf("  dbg_regs[%02d].ctrl = 0x%08x\n", i, bpoint_regs.dbg_regs[i].ctrl);
	}
	printf("\n");

	/* read child regs (REGSET_HW_WATCH) */
	iov.iov_base = &wpoint_regs;
	iov.iov_len = sizeof(wpoint_regs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_HW_WATCH, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_HW_WATCH)");
		goto out;
	}

	printf("===== DUMP REGSET_HW_WATCH =====\n");
	printf("  dbg_info = 0x%08x\n", wpoint_regs.dbg_info);

	remain = sizeof(wpoint_regs.dbg_regs) / sizeof(wpoint_regs.dbg_regs[0]);
	for (i = 0; i < remain; i++) {
		printf("  dbg_regs[%02d].addr = 0x%016llx\n", i, wpoint_regs.dbg_regs[i].addr);
		printf("  dbg_regs[%02d].ctrl = 0x%08x\n", i, wpoint_regs.dbg_regs[i].ctrl);
	}
	printf("\n");

#define NT_ARM_SYSTEM_CALL 0x404
	/* read child regs (REGSET_SYSTEM_CALL) */
	iov.iov_base = &syscallno;
	iov.iov_len = sizeof(syscallno);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_SYSTEM_CALL, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_SYSTEM_CALL)");
		goto out;
	}

	printf("===== DUMP REGSET_SYSTEM_CALL =====\n");
	printf("  syscallno = 0x%08x\n", syscallno);
	printf("\n");

	/* success */
	ret = 0;
out:
	return ret;
}

int setregset_check(pid_t cpid, unsigned long *gregno, unsigned long *fregno)
{
	int ret = -1;
	int remain = 0;
	int i = 0;
	struct iovec iov;
	struct user_pt_regs gregs;
	struct user_fpsimd_state fregs;
	unsigned long write_tls_val = 0x1234567890abcdef;
	unsigned long org_tls_val;
	unsigned long tmp_tls_val;
	int write_sysno = 0xfedcba09;
	int org_sysno;
	int tmp_sysno;

	/* read child regs (REGSET_GPR) */
	iov.iov_base = &gregs;
	iov.iov_len = sizeof(gregs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_PRSTATUS, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_PRSTATUS)");
		goto out;
	}

	remain = sizeof(gregs.regs) / sizeof(gregs.regs[0]);
	for (i = remain - 1; 0 <= i; i--) {
		if (i == gregs.regs[i]) {
			break;
		}
	}

	if (i == -1) {
		printf("REGSET_GPR why there is no magicno ???\n");
		goto out;
	}

	/* set magic */
	gregs.regs[i] = ~(unsigned long)i;
	*gregno = (unsigned long)i;

	/* write child regs (REGSET_GPR) */
	iov.iov_base = &gregs;
	iov.iov_len = sizeof(gregs);
	if (ptrace(PTRACE_SETREGSET, cpid, NT_PRSTATUS, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_PRSTATUS)");
		goto out;
	}

	/* read child regs (REGSET_FPR) */
	iov.iov_base = &fregs;
	iov.iov_len = sizeof(fregs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_PRFPREG, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_PRFPREG)");
		goto out;
	}

	/* set magic */
	fregs.vregs[16] = ~(__uint128_t)16;
	*fregno = 16;

	/* read child regs (REGSET_FPR) */
	iov.iov_base = &fregs;
	iov.iov_len = sizeof(fregs);
	if (ptrace(PTRACE_SETREGSET, cpid, NT_PRFPREG, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_PRFPREG)");
		goto out;
	}

	/* read child regs (REGSET_TLS) */
	iov.iov_base = &org_tls_val;
	iov.iov_len = sizeof(org_tls_val);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_TLS, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_TLS)");
		goto out;
	}

	/* write child regs (REGSET_TLS) */
	iov.iov_base = &write_tls_val;
	iov.iov_len = sizeof(write_tls_val);
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_TLS, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_TLS)");
		goto out;
	}

	/* read child regs (REGSET_TLS) */
	iov.iov_base = &tmp_tls_val;
	iov.iov_len = sizeof(tmp_tls_val);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_TLS, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_TLS)");
		goto out;
	}

	/* check */
	if (tmp_tls_val == write_tls_val) {
		printf("ptrace(PTRACE_SETREGSET, NT_ARM_TLS) OK.\n");
	} else {
		printf("ptrace(PTRACE_SETREGSET, NT_ARM_TLS) failed.\n");
		goto out;
	}

	/* rewrite child regs (REGSET_TLS) */
	iov.iov_base = &org_tls_val;
	iov.iov_len = sizeof(org_tls_val);
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_TLS, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_TLS)");
		goto out;
	}

#ifndef NT_ARM_SYSTEM_CALL
#define NT_ARM_SYSTEM_CALL 0x404
#endif /* !NT_ARM_SYSTEM_CALL */
	/* read child regs (REGSET_SYSTEM_CALL) */
	iov.iov_base = &org_sysno;
	iov.iov_len = sizeof(org_sysno);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_SYSTEM_CALL, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_SYSTEM_CALL)");
		goto out;
	}

	/* write child regs (REGSET_SYSTEM_CALL) */
	iov.iov_base = &write_sysno;
	iov.iov_len = sizeof(write_sysno);
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_SYSTEM_CALL, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_SYSTEM_CALL)");
		goto out;
	}

	/* read child regs (REGSET_SYSTEM_CALL) */
	iov.iov_base = &tmp_sysno;
	iov.iov_len = sizeof(tmp_sysno);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_SYSTEM_CALL, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_SYSTEM_CALL)");
		goto out;
	}

	/* check */
	if (tmp_sysno == write_sysno) {
		printf("ptrace(PTRACE_SETREGSET, NT_ARM_SYSTEM_CALL) OK.\n");
	} else {
		printf("ptrace(PTRACE_SETREGSET, NT_ARM_SYSTEM_CALL) failed.\n");
		goto out;
	}

	/* rewrite child regs (REGSET_SYSTEM_CALL) */
	iov.iov_base = &org_sysno;
	iov.iov_len = sizeof(org_sysno);
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_SYSTEM_CALL, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_SYSTEM_CALL)");
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int setregset_child_check(unsigned long gregno, unsigned long fregno, void *gregp)
{
	int ret = -1;
	struct fpregs fpregs;
	struct user_pt_regs *gregs = (struct user_pt_regs *)gregp;

	/* check */
	if (gregs->regs[gregno] == ~gregno) {
		printf("ptrace(PTRACE_SETREGSET, NT_PRSTATUS) OK.\n");
	} else {
		printf("gregs[%ld] != %lx\n", gregno, ~gregno);
		goto out;
	}

	/* read fpregister */
	fpregs_read(&fpregs);

	/* check */
	if (fpregs.qdata[fregno] == ~(__uint128_t)fregno) {
		printf("ptrace(PTRACE_SETREGSET, NT_PRFPREG) OK.\n");
	} else {
		printf("fpregs[%ld] != %lx\n", fregno, ~fregno);
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int set_hw_watchpt(pid_t cpid, unsigned long addr)
{
	int ret = -1;
	int num_reg = 0;
	struct iovec iov;
	struct user_hwdebug_state wpoint_regs;

	memset(&wpoint_regs, 0, sizeof(wpoint_regs));

	/* read child regs (REGSET_HW_WATCH) */
	iov.iov_base = &wpoint_regs;
	iov.iov_len = sizeof(wpoint_regs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_HW_WATCH, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_HW_WATCH)");
		goto out;
	}

	/* get support reg num */
	num_reg = wpoint_regs.dbg_info & 0xff;

	/* setting watchpoint reg */
	wpoint_regs.dbg_regs[0].addr = addr;
	wpoint_regs.dbg_regs[0].ctrl = (2 << 3);		/* hw_write */
	wpoint_regs.dbg_regs[0].ctrl |= ((1 << 8) - 1) << 5;	/* len = 8 */
	wpoint_regs.dbg_regs[0].ctrl |= (2 << 1) | 1;		/* enable at el0 */

	/* write child regs (REGSET_HW_WATCH) */
	iov.iov_base = &wpoint_regs;
	iov.iov_len = offsetof(struct user_hwdebug_state, dbg_regs) + sizeof(wpoint_regs.dbg_regs[0]) * num_reg;
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_HW_WATCH, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_HW_WATCH)");
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int clear_hw_watchpt(pid_t cpid)
{
	int ret = -1;
	int num_reg = 0;
	int i = 0;
	struct iovec iov;
	struct user_hwdebug_state wpoint_regs;

	/* read child regs (REGSET_HW_WATCH) */
	iov.iov_base = &wpoint_regs;
	iov.iov_len = sizeof(wpoint_regs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_HW_WATCH, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_HW_WATCH)");
		goto out;
	}

	/* get support reg num */
	num_reg = wpoint_regs.dbg_info & 0xff;

	/* all clear */
	for (i = 0; i < num_reg; i++) {
		wpoint_regs.dbg_regs[i].addr = 0;
		wpoint_regs.dbg_regs[i].ctrl = 0;
	}

	/* write child regs (REGSET_HW_WATCH) */
	iov.iov_base = &wpoint_regs;
	iov.iov_len = offsetof(struct user_hwdebug_state, dbg_regs) + sizeof(wpoint_regs.dbg_regs[0]) * num_reg;
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_HW_WATCH, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_HW_WATCH)");
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int check_trap_addr(pid_t cpid, unsigned long addr)
{
	int ret = -1;
	struct iovec iov;
	struct user_pt_regs gregs;

	/* read child regs (REGSET_GPR) */
	iov.iov_base = &gregs;
	iov.iov_len = sizeof(gregs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_PRSTATUS, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_PRSTATUS)");
		goto out;
	}

	/* check program counter */
	if (gregs.pc == addr) {
		printf("child is stopped change value addr.\n");
	} else {
		printf("child is stopped not change value addr.\n");
		printf("expection:0x%lx, info.si_addr:0x%lx\n", addr, (unsigned long)gregs.pc);
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int set_hw_breakpt(pid_t cpid, unsigned long addr)
{
	int ret = -1;
	int num_reg = 0;
	struct iovec iov;
	struct user_hwdebug_state bpoint_regs;

	memset(&bpoint_regs, 0, sizeof(bpoint_regs));

	/* read child regs (REGSET_HW_BREAK) */
	iov.iov_base = &bpoint_regs;
	iov.iov_len = sizeof(bpoint_regs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_HW_BREAK, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_HW_BREAK)");
		goto out;
	}

	/* get support reg num */
	num_reg = bpoint_regs.dbg_info & 0xff;

	/* setting watchpoint reg */
	bpoint_regs.dbg_regs[0].addr = addr;
	bpoint_regs.dbg_regs[0].ctrl = (0 << 3);		/* hw_execute */
	bpoint_regs.dbg_regs[0].ctrl |= ((1 << 4) - 1) << 5;	/* len = 4 */
	bpoint_regs.dbg_regs[0].ctrl |= (2 << 1) | 1;		/* enable at el0 */

	/* write child regs (REGSET_HW_BREAK) */
	iov.iov_base = &bpoint_regs;
	iov.iov_len = offsetof(struct user_hwdebug_state, dbg_regs) + sizeof(bpoint_regs.dbg_regs[0]) * num_reg;
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_HW_BREAK, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_HW_BREAK)");
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int clear_hw_breakpt(pid_t cpid)
{
	int ret = -1;
	int num_reg = 0;
	int i = 0;
	struct iovec iov;
	struct user_hwdebug_state bpoint_regs;

	/* read child regs (REGSET_HW_BREAK) */
	iov.iov_base = &bpoint_regs;
	iov.iov_len = sizeof(bpoint_regs);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_HW_BREAK, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_HW_BREAK)");
		goto out;
	}

	/* get support reg num */
	num_reg = bpoint_regs.dbg_info & 0xff;

	/* all clear */
	for (i = 0; i < num_reg; i++) {
		bpoint_regs.dbg_regs[i].addr = 0;
		bpoint_regs.dbg_regs[i].ctrl = 0;
	}

	/* write child regs (REGSET_HW_BREAK) */
	iov.iov_base = &bpoint_regs;
	iov.iov_len = offsetof(struct user_hwdebug_state, dbg_regs) + sizeof(bpoint_regs.dbg_regs[0]) * num_reg;
	if (ptrace(PTRACE_SETREGSET, cpid, NT_ARM_HW_BREAK, &iov)) {
		perror("ptrace(PTRACE_SETREGSET, NT_ARM_HW_BREAK)");
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int get_check_syscallno(pid_t cpid, long expno)
{
	int ret = -1;
	long sysno = 0;
	struct iovec iov;

	/* read child regs (REGSET_SYSTEM_CALL) */
	iov.iov_base = &sysno;
	iov.iov_len = sizeof(sysno);
	if (ptrace(PTRACE_GETREGSET, cpid, NT_ARM_SYSTEM_CALL, &iov)) {
		perror("ptrace(PTRACE_GETREGSET, NT_ARM_SYSTEM_CALL)");
		goto out;
	}

	/* check */
	if (sysno == expno) {
		printf("sysno check OK.\n");
	} else {
		printf("sysno check NG.\n");
		printf("expection:0x%lx readsysno:0x%lx\n", expno, sysno);
		goto out;
	}

	/* success */
	ret = 0;
out:
	return ret;
}

int rewrite_brk_inst(pid_t cpid, void *inst_addr)
{
	unsigned long addr = 0;
	const unsigned long inst = ((NOP_INST << 32UL) | NOP_INST);

	/* read child brk address */
	addr = ptrace(PTRACE_PEEKDATA, cpid, inst_addr, NULL);
	if ((addr == -1) && errno) {
		perror("ptrace(PTRACE_PEEKDATA)");
		return -1;
	}

	/* write nop instruction */
	if (ptrace(PTRACE_POKETEXT, cpid, addr, inst)) {
		perror("ptrace(PTRACE_POKETEXT)");
		return -1;
	}
	return 0;
}
