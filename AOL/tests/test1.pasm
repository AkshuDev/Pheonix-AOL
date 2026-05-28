	:align 8
:section .rodata
	__aol_entry_dbg!ubyte[] = "DBG: Entry!", 10
	str_0!ubyte[] = "Hello World!"
	:align 8
:section .data
	:align 8
:section .bss
	:align 16
:section .text
	:global __aol_main__

__aol_main__:
	mov %rdi, %rsp
	mov %rsi, %rdi
	lea %rdi, [__aol_entry_dbg]
	mov %rsi, 12
	call __aol_print
	call $main
	mov %rax, 60
	mov %rdi, 0
	syscall
	ret
__aol_print:
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rax, 1
	mov %rdi, 1
	syscall
	ret

.func nextfunc
	push %rbp
	mov %rbp, %rsp
	sub %rsp, 8
	mov [%rbp - 8], str_0
__aol_if__:
	cmp [%rbp - 8], 0
	je $nextfunc.__aol_else__
	mov %rax, [%rbp - 8]

	ret

__aol_else__:
__aol_endif__:
	mov %rsp, %rbp
	pop %rbp
	ret
.endfunc

.func main
	push %rbp
	mov %rbp, %rsp
	sub %rsp, 8
	mov [%rbp - 8], 5
	mov %rsp, %rbp
	pop %rbp
	ret
.endfunc

