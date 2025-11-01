//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_CPU_HPP
#define KERNEL_CPU_HPP
#include "kernel/percpu.hpp"

namespace x86_64 {
    inline void hcf() {
        asm volatile("cli");
        asm volatile("hlt");
    }
} // namespace x86_64

#endif // KERNEL_CPU_HPP
