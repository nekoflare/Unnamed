//
// Created by neko on 11/1/25.
//

#include "vma.hpp"
#include "kernel/helpers.hpp"
#include "vmm.hpp"

namespace memory {
    static VAddrAllocator<2048> allocator;
}

void memory::init_kernel_virtual_allocator() noexcept {
    // allocate the base and length of free virtual memory
    std::uintptr_t range_start = core::align_down(
            get_memory_slide() + get_highest_physical_address(), PAGE_SIZE);
    std::uintptr_t range_end =
            core::align_down(get_kernel_base_address(), PAGE_SIZE);
    std::uintptr_t range_length = range_end - range_start;

    allocator.init(range_start, range_length);

    logger::debug("Free virtual memory: %lu (B)\n", range_length);
}

std::optional<std::uintptr_t>
memory::allocate_virtual_memory(std::size_t length) {
    if (!core::check_aligned(length, PAGE_SIZE)) {
        core::set_error(core::ErrorCode::MemoryMisaligned);
        return std::nullopt;
    }

    return allocator.alloc_pages(length / PAGE_SIZE);
}

void memory::deallocate_virtual_memory(std::uintptr_t address,
                                       std::size_t length) {
    allocator.free(address, length / PAGE_SIZE);
}
