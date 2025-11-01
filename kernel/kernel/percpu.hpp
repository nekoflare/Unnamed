//
// Created by neko on 10/29/25.
//

#ifndef KERNEL_PERCPU_HPP
#define KERNEL_PERCPU_HPP

#include <cstdint>

#include "error.hpp"

namespace core {
    using CpuId = uint32_t;

    struct percpu {
        ErrorCode error;
    };

    void setup_bsp_percpu();
    percpu *get_percpu();

    void set_error(ErrorCode code);
} // namespace core

#endif // KERNEL_PERCPU_HPP
