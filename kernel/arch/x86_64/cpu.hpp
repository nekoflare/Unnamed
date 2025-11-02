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

    inline std::uintptr_t get_cr3() {
        std::uintptr_t cr3;
        asm volatile("mov %%cr3, %0" : "=r"(cr3));
        return cr3;
    }

    // Invalidate a single virtual page
    inline void invlpg(void *addr) {
        asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
    }
} // namespace x86_64

#endif // KERNEL_CPU_HPP
