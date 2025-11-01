//
// Created by neko on 10/29/25.
//

#ifndef KERNEL_MSR_HPP
#define KERNEL_MSR_HPP

#include <cstdint>

namespace x86_64 {
    constexpr uint32_t MSR_GS_BASE = 0xC0000101;

    inline uint64_t rdmsr(uint32_t msr) {
        uint32_t low, high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
        return (static_cast<uint64_t>(high) << 32) | low;
    }

    inline void wrmsr(uint32_t msr, uint64_t value) {
        auto low = static_cast<uint32_t>(value);
        auto high = static_cast<uint32_t>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
    }
}

#endif //KERNEL_MSR_HPP
