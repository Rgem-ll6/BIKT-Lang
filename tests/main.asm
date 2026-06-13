extern bikt_print
extern bikt_input
section .text
global _start
	extern bikt_print

main: 
	push rbp
	mov rbp, rsp
	sub rsp, 128
	mov rax, 6
	mov [rbp - 8], rax
	mov rax, 10
	mov [rbp - 16], rax
	mov rax, [rbp - 8]
	push rax
	mov rax, [rbp - 16]
	mov rbx, rax
	pop rax
	add rax, rbx
	mov [rbp - 8], rax
	mov rax, [rbp - 8]
	mov rdi, rax
	call bikt_print
	mov rax, 0
	leave
	ret
	leave
	ret

_start:
	call main
	mov rax, 60
	mov rdi, 0
	syscall
