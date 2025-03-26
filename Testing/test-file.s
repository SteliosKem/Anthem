	.globl var
	.data
	.align 4
var:
	.long 1
	.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $1, -4(%rbp)
	movl var(%rip), %r10d
	movl %r10d, -12(%rbp)
	movl -12(%rbp), %r10d
	addl -4(%rbp), %r10d
	movl %r10d, -12(%rbp)
	movl -12(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret


