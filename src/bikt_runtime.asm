section .data
    newline db 10

section .bss
    input_buf resb 32        ; buffer for reading input

section .text
global bikt_print
global bikt_input

; ============================================
; bikt_print(rdi = integer to print)
; Prints an integer followed by a newline
; to stdout using write syscall
; ============================================
bikt_print:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    mov rax, rdi             ; number to print

    ; handle negative numbers
    test rax, rax
    jns .positive

    push rax
    mov byte [rbp - 32], '-'
    mov rax, 1               ; write syscall
    mov rdi, 1               ; stdout fd
    lea rsi, [rbp - 32]
    mov rdx, 1               ; 1 byte
    syscall
    pop rax
    neg rax                  ; make positive

.positive:
    ; convert integer to decimal string on the stack
    ; build it right to left
    lea rdi, [rbp - 1]
    mov byte [rbp - 1], 10   ; newline at end
    mov rbx, 10              ; divisor
    mov rcx, 1               ; byte count (starts at 1 for newline)

.convert_loop:
    xor rdx, rdx
    div rbx                  ; rax = quotient, rdx = remainder
    add dl, '0'              ; remainder to ascii
    dec rdi
    mov [rdi], dl            ; store digit
    inc rcx
    test rax, rax
    jnz .convert_loop        ; loop until quotient is 0

    ; write the number string
    mov rax, 1               ; write syscall
    mov rsi, rdi             ; pointer to start of number string
    mov rdi, 1               ; stdout fd
    mov rdx, rcx             ; length
    syscall

    leave
    ret

; ============================================
; bikt_input() -> rax = integer read from stdin
; Reads a line from stdin and parses it as
; a signed integer
; Returns the parsed integer in rax
; ============================================
bikt_input:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    ; read from stdin into input_buf
    mov rax, 0               ; read syscall
    mov rdi, 0               ; stdin fd
    lea rsi, [input_buf]
    mov rdx, 32              ; max bytes to read
    syscall

    ; parse the string into an integer
    lea rsi, [input_buf]

    ; check for negative sign
    xor r8, r8               ; r8 = negative flag
    cmp byte [rsi], '-'
    jne .parse_digits
    mov r8, 1                ; mark as negative
    inc rsi                  ; skip '-'

.parse_digits:
    xor rax, rax             ; accumulator
    mov rbx, 10              ; multiplier

.parse_loop:
    movzx rcx, byte [rsi]
    cmp rcx, 10              ; newline = end
    je .parse_done
    cmp rcx, 0               ; null = end
    je .parse_done
    cmp rcx, '0'             ; sanity check
    jl .parse_done
    cmp rcx, '9'
    jg .parse_done

    sub rcx, '0'             ; ascii to digit
    imul rax, rbx            ; rax *= 10
    add rax, rcx             ; rax += digit
    inc rsi
    jmp .parse_loop

.parse_done:
    ; apply negative sign if needed
    test r8, r8
    jz .done
    neg rax

.done:
    leave
    ret
