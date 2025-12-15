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
	jmp __aol_exit
	ret
__aol_exit:
    mov %rax, 60
    syscall
__aol_print:
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rax, 1
	mov %rdi, 1
	syscall
	ret
