.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $4, %rsp
	movl $4, -4(%rbp)
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.globl other_func
other_func:
	pushq %rbp
	movq %rsp, %rbp
	subq $0, %rsp
	movl $1, %eax
	movq %rbp, %rsp
	popq %rbp
	ret
	movl $0, %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
