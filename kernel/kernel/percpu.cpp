//
// Created by neko on 10/29/25.
//

#include "percpu.hpp"

#include "arch/x86_64/msr.hpp"

namespace core {
    static percpu bsp_percpu;

    inline void set_gsbase(void* v) {
        x86_64::wrmsr(x86_64::MSR_GS_BASE, reinterpret_cast<uint64_t>(v));
    }
}

void core::setup_bsp_percpu() {
    __builtin_memset(&bsp_percpu, 0, sizeof(bsp_percpu));
    set_gsbase(&bsp_percpu);
}

core::percpu* core::get_percpu() {
    return reinterpret_cast<percpu *>(x86_64::rdmsr(x86_64::MSR_GS_BASE));
}