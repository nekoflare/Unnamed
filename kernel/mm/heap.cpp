//
// Created by neko on 11/1/25.
//

#include "heap.hpp"
#include <cstdint>
#include <new>

namespace memory {
    bool is_initialized = false;
}

void memory::init_heap() {}

void *operator new[](std::size_t size, const std::nothrow_t &) noexcept {
    (void) size;
    return nullptr;
}

void *operator new(std::size_t size, const std::nothrow_t &) noexcept {
    (void) size;
    return nullptr;
}
