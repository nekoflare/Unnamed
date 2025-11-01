//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_REGISTERS_HPP
#define KERNEL_REGISTERS_HPP

#include <cstdint>

namespace x86_64 {
    struct [[gnu::packed]] Registers {
        uint64_t es, ds, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx,
                rcx, rbx, rax, interrupt_number, error_code, rip, cs, rflags,
                original_rsp, ss;
    };
}

#endif // KERNEL_REGISTERS_HPP
