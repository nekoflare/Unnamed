section .text
    global flush_gdt

flush_gdt:
    lgdt [rdi]
.reload_segments:
    push rsi
    lea rax, [rel .reload_cs]
    push rax
    retfq
.reload_cs:
    mov ax, dx
    mov ds, ax
    mov es, ax
    mov ss, ax
    ret