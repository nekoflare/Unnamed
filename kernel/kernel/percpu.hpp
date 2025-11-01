//
// Created by neko on 10/29/25.
//

#ifndef KERNEL_PERCPU_HPP
#define KERNEL_PERCPU_HPP

#include <cstdint>

namespace core {
    using CpuId = uint32_t;

    struct percpu {};

    void setup_bsp_percpu();
    percpu *get_percpu();
} // namespace core

#endif // KERNEL_PERCPU_HPP
