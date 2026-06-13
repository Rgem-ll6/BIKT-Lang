section .data
    newline db 10

section .bss
    input_buf resb 32        

section .text
global bikt_print
global bikt_input

bikt_print:
    push rbp
    mov rbp, rsp
    
    mov rax, 4194304
    cmp rdi, rax
    jae .print_string_routine

    push rbx                 
    push r12                 
    mov rax, rdi

    test rax, rax
    jns .positive

    mov r12, rax             

    mov byte [rbp - 64], '-'
    mov rax, 1               
    mov rdi, 1               
    lea rsi, [rbp - 64]
    mov rdx, 1               
    syscall
    
    mov rax, r12             
    neg rax                  

.positive:
    lea rsi, [rbp - 1]       
    mov byte [rsi], 10       
    mov rbx, 10              
    mov rcx, 1               

.convert_loop:
    xor rdx, rdx
    div rbx                  
    add dl, '0'              
    dec rsi                  
    mov [rsi], dl            
    inc rcx                  
    test rax, rax
    jnz .convert_loop        

    mov rax, 1               
    mov rdi, 1               
    mov rdx, rcx             
    syscall

    pop r12                  
    pop rbx                  
    leave
    ret

.print_string_routine:
    mov rsi, rdi             
    
.string_loop:
    mov al, [rsi]            
    cmp al, 0                
    je .string_done

    mov rax, 1               
    mov rdi, 1               
    mov rdx, 1               
    syscall                  

    inc rsi                  
    jmp .string_loop

.string_done:
    mov rax, 1               
    mov rdi, 1               
    lea rsi, [newline]       
    mov rdx, 1
    syscall

    leave
    ret

bikt_input:
    push rbp
    mov rbp, rsp

    mov rax, 0               
    mov rdi, 0               
    lea rsi, [input_buf]
    mov rdx, 32              
    syscall

    lea rsi, [input_buf]

    xor r8, r8               
    cmp byte [rsi], '-'
    jne .parse_digits
    mov r8, 1                
    inc rsi                  

.parse_digits:
    xor rax, rax             
    mov rbx, 10              

.parse_loop:
    movzx rcx, byte [rsi]
    cmp rcx, 10              
    je .parse_done
    cmp rcx, 0               
    je .parse_done
    cmp rcx, '0'             
    jl .parse_done
    cmp rcx, '9'
    jg .parse_done

    sub rcx, '0'             
    imul rax, rbx            
    add rax, rcx             
    inc rsi
    jmp .parse_loop

.parse_done:
    test r8, r8
    jz .done
    neg rax

.done:
    leave
    ret
