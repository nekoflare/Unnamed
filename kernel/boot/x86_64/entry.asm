bits 64

section .text
    global _start
    extern kernel_main

_start:
    ; align the stack to 64 bytes
    and rsp, -64
    call kernel_main

    ; in case kernel returns hang myself
    cli
    hlt