.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $4, %rsp
	movl $5, %r11d
	cmpl $1, %r11d
	movl $0, -4(%rbp)
	setg -4(%rbp)
	movl -4(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
