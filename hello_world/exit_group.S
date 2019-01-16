/* exit_group.S COPYRIGHT FUJITSU LIMITED 2015 */
	.text
	.globl _start
_start:
	mov	x0, #0x0
	mov	x8, #0x5e
	svc	#0x0		//send exit_group syscall
