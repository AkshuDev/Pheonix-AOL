:section .rodata
:align 8
:section .data
:align 8
:section .bss
:align 8
:section .text
:align 16
:global __aol_main__

__aol_main__:
	mov %rax, %rsp
	mov %rbx, %rdi
	call $main
.func main
	push %rbp
	push 0
	mov %rbp, %rsp
		mov [%rbp - 8], 55
	pop %rbx
	pop %rbp
	ret
.endfunc

