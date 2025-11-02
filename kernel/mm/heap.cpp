//
// Created by neko on 11/1/25.
//

#include "heap.hpp"
#include <cstdint>
#include <new>

#include "kernel/helpers.hpp"
#include "lib/dlmalloc/malloc.h"
#include "vma.hpp"
#include "vmm.hpp"

namespace memory {
    static bool is_initialized = false;
    static std::uintptr_t heap_base = 0;
    static std::uintptr_t heap_size = 0;
} // namespace memory

#ifndef __cplusplus
extern "C" {
#endif

void *mmap(void *addr, std::size_t length, int prot, int flags, int fd,
           intptr_t offset) {
    (void) addr;
    (void) prot;
    (void) offset;
    (void) fd;

    if (!core::is_flag_set(flags, MAP_PRIVATE) ||
        !core::is_flag_set(flags, MAP_ANONYMOUS)) {
        logger::debug("Only MAP_PRIVATE|MAP_ANONYMOUS is supported.\n");
        return reinterpret_cast<void *>(-1ULL);
    }

    if (!length) {
        return reinterpret_cast<void *>(-1ULL);
    }

    length = core::align_up(length, PAGE_SIZE);

    auto base = memory::allocate_virtual_memory(length);
    if (!base.has_value()) {
        return reinterpret_cast<void *>(-1ULL);
    }

    size_t page_count = length / PAGE_SIZE;
    for (size_t i = 0; i < page_count; i++) {
        auto page = memory::alloc_page();
        if (!page) {
            core::set_error(core::ErrorCode::OutOfMemory);
            memory::deallocate_virtual_memory(base.value(), length);
            return reinterpret_cast<void *>(-1ULL);
        }

        auto result = memory::map_virtual(
                x86_64::get_cr3() + memory::get_memory_slide(),
                base.value() + i * PAGE_SIZE, page,
                memory::MapFlags::Read | memory::MapFlags::Write);

        if (!result) {
            core::set_error(core::ErrorCode::MapFail);
            return reinterpret_cast<void *>(-1ULL);
        }
    }

    return reinterpret_cast<void *>(base.value());
}

int munmap(void *addr, size_t length) {
    (void) addr;
    (void) length;
    logger::debug("munmap() called but not implemented.\n");
    x86_64::hcf();

    return 0;
}

void *sbrk(ptrdiff_t increment) {
    if (!memory::is_initialized)
        return 0;

    static std::uintptr_t heap_top = memory::heap_base;

    if (increment == 0) {
        return reinterpret_cast<void *>(heap_top);
    }

    if (increment < 0) {
        logger::debug("sbrk: Shrinking heap not supported.\n");
        x86_64::hcf();
    }

    std::uintptr_t old_heap_top = heap_top;

    std::size_t aligned_increment =
            core::align_up(static_cast<std::size_t>(increment), PAGE_SIZE);
    std::size_t page_count = aligned_increment / PAGE_SIZE;

    if (heap_top + aligned_increment > memory::heap_base + memory::heap_size) {
        core::set_error(core::ErrorCode::OutOfMemory);
        return 0;
    }

    for (std::size_t i = 0; i < page_count; i++) {
        auto phys = memory::alloc_page();
        if (!phys) {
            core::set_error(core::ErrorCode::OutOfMemory);
            return 0;
        }

        auto result = memory::map_virtual(
                x86_64::get_cr3() + memory::get_memory_slide(),
                heap_top + i * PAGE_SIZE, phys,
                memory::MapFlags::Read | memory::MapFlags::Write);

        if (!result) {
            core::set_error(core::ErrorCode::MapFail);
            return 0;
        }
    }

    heap_top += aligned_increment;
    return reinterpret_cast<void *>(old_heap_top);
}

#ifndef __cplusplus
}
#endif

void memory::init_heap() {
    heap_size = 32 * 1024 * 1024ULL; // 32GB of heap shall suffice
    auto address = allocate_virtual_memory(heap_size);

    if (address.has_value()) {
        heap_base = address.value();
    } else {
        core::set_error(core::ErrorCode::OutOfMemory);
        logger::debug("Out of virtual memory!\n");
        x86_64::hcf();
    }

    is_initialized = true;
}

void *operator new(std::size_t size) { return malloc(size); }

void *operator new[](std::size_t size, const std::nothrow_t &) noexcept {
    return malloc(size);
}

void *operator new(std::size_t size, const std::nothrow_t &) noexcept {
    return malloc(size);
}

void operator delete(void *ptr, std::size_t size) {
    (void) size;
    free(ptr);
}
