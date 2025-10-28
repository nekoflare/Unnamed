//
// Created by neko on 10/28/25.
//

#include "pma.hpp"
#include <kernel/assert.hpp>
#include <lib/utils.hpp>
#include "vma.hpp"

static volatile limine_memmap_request memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = LIMINE_API_REVISION,
        .response = nullptr};

void memory::FreeListAllocator::init(
        const PtrArrayWrapper<limine_memmap_entry> &entries) {
    for (const auto &[base, length, type]: entries) {
        if (type != LIMINE_MEMMAP_USABLE)
            continue;

        logger::debug("Adding usable memory: 0x%lx -> 0x%lx", base,
                      base + length);

        std::uintptr_t virt_start = get_memory_slide() + base;
        auto *block = reinterpret_cast<FreeBlock *>(virt_start);
        block->size = length;
        block->next = m_head;
        m_head = block;
    }
}

std::uintptr_t memory::FreeListAllocator::alloc(size_t size) noexcept {
    ASSERT((size % PAGE_SIZE) == 0);

    size = align_up(size, PAGE_SIZE);
    FreeBlock **current = &m_head;

    while (*current) {
        auto addr = reinterpret_cast<std::uintptr_t>(*current);
        auto aligned_addr = align_up(addr + sizeof(FreeBlock), PAGE_SIZE);
        auto total_needed = aligned_addr - addr + size;

        if ((*current)->size >= total_needed) {
            if ((*current)->size > total_needed + sizeof(FreeBlock)) {
                auto *next_block =
                        reinterpret_cast<FreeBlock *>(addr + total_needed);
                next_block->size = (*current)->size - total_needed;
                next_block->next = (*current)->next;
                *current = next_block;
            } else {
                *current = (*current)->next;
            }

            return aligned_addr - get_memory_slide();
        }

        current = &((*current)->next);
    }

    return 0;
}

void memory::FreeListAllocator::free(std::uintptr_t ptr, size_t size) noexcept {
    ASSERT((size % PAGE_SIZE) == 0);

    auto addr = ptr - get_memory_slide();
    auto *block = reinterpret_cast<FreeBlock *>(addr);
    block->size = size;

    block->next = m_head;
    m_head = block;

    coalesce();
}

void memory::FreeListAllocator::coalesce() {
    for (FreeBlock *a = m_head; a; a = a->next) {
        FreeBlock *b = a;
        while (b->next) {
            auto a_end = reinterpret_cast<std::uintptr_t>(a) + a->size;
            auto b_addr = reinterpret_cast<std::uintptr_t>(b->next);
            if (a_end == b_addr) {
                a->size += b->next->size;
                b->next = b->next->next;
            } else {
                b = b->next;
            }
        }
    }
}

static memory::FreeListAllocator free_list_allocator;

void memory::init_pma() {
    ASSERT(memmap_request.response != nullptr);

    auto entries = PtrArrayWrapper<limine_memmap_entry>{
            memmap_request.response->entries};
    free_list_allocator.init(entries);
}
std::uintptr_t memory::alloc_page() {
    return free_list_allocator.alloc(PAGE_SIZE);
}

std::uintptr_t memory::alloc_pages_continuous(std::size_t page_count) {
    return free_list_allocator.alloc(page_count * PAGE_SIZE);
}

void memory::dealloc_page(std::uintptr_t addr) {
    free_list_allocator.free(addr, PAGE_SIZE);
}

void memory::dealloc_pages_continuous(std::uintptr_t addr,
                                      std::size_t page_count) {
    free_list_allocator.free(addr, page_count * PAGE_SIZE);
}
