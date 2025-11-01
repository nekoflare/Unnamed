//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_VMM_HPP
#define KERNEL_VMM_HPP

#include <cstdint>
#include <kernel/bit_helpers.hpp>
#include "pma.hpp"

namespace memory {
    enum class MapFlags : uint32_t {
        None = 0,
        Read = 1 << 0,
        Write = 1 << 1,
        Execute = 1 << 2,
        User = 1 << 3,
        CacheDisable = 1 << 4,
        WriteThrough = 1 << 5,
    };

    enum class PageBits : uint64_t {
        Present      = 1ULL << 0,
        ReadWrite    = 1ULL << 1, // If set then page is read-write
        User         = 1ULL << 2,
        WriteThrough = 1ULL << 3,
        CacheDisable = 1ULL << 4,
        PAT          = 1ULL << 7,
        NoExecute    = 1ULL << 63,
    };

    constexpr uint64_t ppn_mask = 0x000FFFFFFFFFF000;

    std::uintptr_t get_memory_slide();
    std::uintptr_t get_kernel_base_address();

    // We assume that pagemap is virtual.
    [[nodiscard]] bool map_virtual(
        std::uintptr_t pagemap_virtual,
        std::uintptr_t virt,
        std::uintptr_t phys,
        MapFlags flags);
}

#endif // KERNEL_VMA_HPP
