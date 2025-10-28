section .text
    global flush_idt

flush_idt:
    lidt [rdi]
    ret
