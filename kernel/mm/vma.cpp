//
// Created by neko on 11/1/25.
//

#include "vma.hpp"

namespace memory {
    static VAddrAllocator<2048> allocator;
}

void memory::init_kernel_virtual_allocator() noexcept {
    allocator.init();
}