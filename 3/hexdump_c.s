.intel_syntax noprefix

.globl _start

# *r13 -- file
# *r14 -- ascii representation
# *r15 -- hex representation of byte
# r10 -- byte counter

nybble_to_character:
	cmp r12b, 10
	jae a_through_f
	add r12b, 0x30
	movzx rax, r12b
	ret
	a_through_f:
	add r12b, 0x57 # 'a' - 0xa
	movzx rax, r12b
	ret

byte_to_hex:
	push r12
	mov r12b, BYTE PTR [rdi]
	shr r12b, 4
	call nybble_to_character
	push rax
	mov r12b, BYTE PTR [rdi]
	and r12b, 0x0F
	call nybble_to_character
	mov r12b, al
	pop rax
	shl rax, 8
	or rax, r12
	pop r12
	ret

byte_to_ascii:
	cmp BYTE PTR [r13], 0x20 # 0x0 -> 0x20 are all empty characters
	jae write_as_is
	write_period:
	mov rax, 0x2E # 0x2E == '.'
	ret
	write_as_is:
	cmp BYTE PTR [r13], 0x7E
	ja write_period
	movzx rax, BYTE PTR [r13]
	ret


print_as_is:
	mov BYTE PTR [rsp - 16], dil
	mov BYTE PTR [rsp - 8], 0
	mov rax, 1
	mov rdi, 1
	mov rsi, rsp
	sub rsi, 16
	mov rdx, 1
	syscall
	ret


print_byte:
	# find which ascii values correspond to which numbers and put the right ascii value in the r12 register
	call byte_to_hex
	push rax
	shr rax, 8
	mov dil, al
	call print_as_is
	pop rax
	mov dil, al
	call print_as_is
	ret

open_file:
	mov rax, 2
	mov rdi, QWORD PTR [r10]
	mov rsi, 0
	mov rdx, 0
	syscall
	ret

print_space:
	mov dil, 0x20
	call print_as_is
	ret

fill_extra_bytes:
	push rdi
	call print_space
	pop rdi
	sub rdi, 1
	cmp rdi, 0
	ja fill_extra_bytes
	ret

_start:
	mov rax, 0
	mov r12, 0
	cmp QWORD PTR [rsp], 1
	add rsp, 16
	mov r10, rsp # r10 is now where the arguments are
	mov r9, rsp
	sub rsp, 0x12
	sub rsp, 16
	mov r13, rsp
	add r13, 0x10
	cmp QWORD PTR [r10 - 16], 1
	je skip_open_file
	call open_file
	push rax
	jmp read_file
	skip_open_file:
	push rax # located @ r10 - 0x10 - 16
	push 0
	push 0
	jmp read_chunk
	read_file:
	mov rdi, rax
	mov rax, 0
	mov rsi, r13
	mov rdx, 0x10
	syscall

#	cmp QWORD PTR [r9 - 0x2A], 0
#	jne skip_stdin_case
#	push 0x0FFF
#	push 0
#	jmp read_chunk
	skip_stdin_case:
	cmp rax, 0
	je end
	push rax
	push 0
	

	display_line_number:
	mov r15, 3
	#cmp QWORD PTR [r9 - 0x2A], 0
	loop_line_num:
	mov r14, rsp
	add r14, r15
	mov rdi, r14
	call print_byte
	dec r15
	cmp r15, -1
	jne loop_line_num
	call print_space
	call print_space

	mov rbx, 0
	pop rcx
	add rcx, 2
	push rcx

	loop_display_byte:
	push rbx
	mov rdi, r13
	call print_byte
	call print_space
	inc r13
	pop rbx
	inc rbx
	cmp rbx, 8
	je print_extra_space
	cmp rbx, 16
	je cleanup_display_bytes
	pop rcx
	pop rax
	inc rcx
	cmp rcx, rax
	push rax
	push rcx
	ja fill_db_empty_space
	jmp loop_display_byte

	fill_db_empty_space:
	mov rdi, 16
	sub rdi, rbx
	mov rax, rdi
	add rdi, rax
	add rdi, rax
	cmp rbx, 8
	jne no_add_space
	add rdi, 1
	no_add_space:
	call fill_extra_bytes
	cleanup_display_bytes:
	sub r13, rbx
	pop rcx
	sub rcx, rbx
	push rcx

	print_extra_space:
	call print_space
	cmp rbx, 8
	je loop_display_byte
	mov dil, 0x7C # |
	call print_as_is

	mov rbx, 0
	pop rcx
	add rcx, 1
	push rcx
	loop_display_ascii:
	push rbx
	call byte_to_ascii
	mov dil, al
	call print_as_is
	pop rbx
	inc rbx
	inc r13
	cmp rbx, 16
	je done_display_ascii

	pop rcx
	pop rax
	inc rcx
	cmp rcx, rax
	push rax
	push rcx
	jbe loop_display_ascii
	mov dil, 0x2E 
	call print_as_is
	done_display_ascii:
	mov dil, 0x7C # |
	call print_as_is
	mov dil, 0x0A # \n
	call print_as_is
	sub r13, rbx

	test:
	pop rbx
	pop rax
	cmp rbx, rax
	push rax
	push rbx
	jl display_line_number

	mov r12, 0
	read_chunk:
	mov rdi, QWORD PTR [r9 - 0x2A]
	add r13, r12
	mov rsi, r13
	mov rax, 0
	mov rdx, 0x10
	sub rdx, r12
	syscall # read next 2 lines
	sub r13, r12
	pop rbx
	pop rcx
	add rcx, rax
	push rcx
	push rbx

	add rax, r12
	cmp rax, 0x10
	je display_line_number

	
	cmp QWORD PTR [r9 - 0x2A], 0
	mov rdx, r12
	mov r12, rax
	je read_chunk
	mov r12, rdx

	add r10, 8
	cmp QWORD PTR [r10], 0
	jne check_for_spare_bytes
	sub r10, 8
	
	cmp rax, 0
	ja display_line_number
	jmp line_num_final
	
	check_for_spare_bytes:	
	mov r12, rax
	call open_file
	mov QWORD PTR [r9 - 0x2A], rax
	cmp rax, -1
	je line_num_final
	jmp read_chunk


	pop rbx
	sub rbx, 1
	push rbx
	line_num_final:
	mov r15, 3
	loop_line_num_final: # there's gotta be a better way but i don't know how to not repeat here
	mov r14, rsp
	add r14, r15
	mov rdi, r14
	call print_byte
	dec r15
	cmp r15, -1
	jne loop_line_num_final

	mov rbx, 0
	pop rcx
	add rcx, 2
	push rcx

	mov rdi, 0x0A
	call print_as_is

	end:
	mov rax, 60
	mov rdi, 0
	syscall

