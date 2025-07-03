.intel_syntax noprefix

.globl unwind

unwind:
	push rbp
	mov rbp, rsp
	mov QWORD PTR [rsp - 30], 0x78300000 #0x, little endian
	mov QWORD PTR [rsp - 8], 0x0A #\n
	mov r9, 0
	loop_unwind:
	mov rax, [rbp + 8]
	mov r8, 15

	loop_byte_to_hex:
	movzx rcx, al
	and cl, 0xF
	cmp rcx, 0xA
	jae above_15
	add rcx, 0x30
	jmp done_ascii_conversion
	above_15:
	add rcx, 0x37 # 'A' - 0xa
	done_ascii_conversion:
	mov BYTE PTR [rsp - 24 + r8], cl
	shr rax, 4
	sub r8, 1
	cmp r8, 0
	jge loop_byte_to_hex
	
	mov rax, 1
	mov rdi, 0
	mov rsi, rsp
	sub rsi, 30
	mov rdx, 30
	syscall #print

	mov rbp, QWORD PTR [rbp]
	cmp rbp, 1
	jne loop_unwind
	mov rax, 0
	pop rbp
	ret
	
	
