section .text
global _start
	extern bikt_print

add: 
	push rbp
	mov rbp, rsp
	sub rsp, 128
	mov [rbp - 8], rdi
	mov [rbp - 16], rsi
	mov rax, [rbp - 8]
	push rax
	mov rax, [rbp - 16]
	mov rbx, rax
	pop rax
	add rax, rbx
	mov [rbp - 24], rax
	mov rax, [rbp - 24]
	leave
	ret
	leave
	ret

main: 
	push rbp
	mov rbp, rsp
	sub rsp, 128
	mov rax, 18
	mov [rbp - 32], rax
	mov rax, [rbp - 32]
	push rax
	mov rax, 20
	mov rbx, rax
	pop rax
	cmp rax, 0
	je .L0_else
	mov rax, 5
	push rax
	mov rax, [rbp - 32]
	mov rbx, rax
	pop rax
	add rax, rbx
	mov [rbp - 40], rax
	jmp .L0_end
.L0_else: 
.L0_end:
	leave
	ret

_start:
	call main
	mov rax, 60
	mov rdi, 0
	syscall
