/* hello_world.S COPYRIGHT FUJITSU LIMITED 2015 */
	.text
	.globl _start
_start:
	mov	x0, #1		//stdout file descriptor(arg0)
	adr	x1, message	//get message VA(arg1)
	mov	x2, #12		//string length(arg2)
	mov	x8, #0x40	//0x40 = __NR_write
	svc	#0x0		//send write syscall

	mov	x0, xzr		//clear x0(arg0)
	mov	x8, #0x5e	//0x5e = __NR_exit_group
	svc	#0x0		//send exit_group syscall

	.align	8
message:
	.ascii	"Hello World\n"
