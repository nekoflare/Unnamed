//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_PMA_HPP
#define KERNEL_PMA_HPP

#include <cstddef>
#include <cstdint>
#include <limine.h>

#include "lib/ptr_array_wrapper.hpp"

#define PAGE_SIZE (4096)

namespace memory {
    struct FreeBlock {
        std::size_t size;
        FreeBlock *next;
    };

    class FreeListAllocator {
    public:
        void init(const PtrArrayWrapper<limine_memmap_entry> &entries);
        [[nodiscard]] std::uintptr_t alloc(size_t size) noexcept;
        void free(std::uintptr_t ptr, size_t size) noexcept;

    private:
        FreeBlock *m_head = nullptr;

        void coalesce();
    };

    void init_pma();

    std::uintptr_t alloc_page();
    std::uintptr_t alloc_pages_continuous(std::size_t page_count);
    void dealloc_page(std::uintptr_t addr);
    void dealloc_pages_continuous(std::uintptr_t addr, std::size_t page_count);
} // namespace memory

#endif // KERNEL_PMA_HPP
