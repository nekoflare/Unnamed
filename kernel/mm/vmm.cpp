//
// Created by neko on 10/28/25.
//

#include "vmm.hpp"
#include <kernel/helpers.hpp>
#include <kernel/percpu.hpp>
#include <lib/memory.hpp>
#include <limine.h>

#include "arch/x86_64/cpu.hpp"
#include "utility"

static volatile limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST,
                                                    .revision =
                                                            LIMINE_API_REVISION,
                                                    .response = nullptr};

static volatile limine_kernel_address_request kernel_address_request = {
        .id = LIMINE_KERNEL_ADDRESS_REQUEST,
        .revision = LIMINE_API_REVISION,
        .response = nullptr};

constexpr std::uintptr_t va_offset(std::uintptr_t va) noexcept {
    return va & 0xFFFULL;
}

constexpr std::uintptr_t va_pt_index(std::uintptr_t va) noexcept {
    return va >> 12 & 0x1FFULL;
}

constexpr std::uintptr_t va_pd_index(std::uintptr_t va) noexcept {
    return va >> 21 & 0x1FFULL;
}

constexpr std::uintptr_t va_pdp_index(std::uintptr_t va) noexcept {
    return va >> 30 & 0x1FFULL;
}

constexpr std::uintptr_t va_pml4_index(std::uintptr_t va) noexcept {
    return va >> 39 & 0x1FFULL;
}

constexpr std::uintptr_t va_canonical_bits(std::uintptr_t va) noexcept {
    return va >> 48 & 0xFFFFULL;
}

std::uintptr_t memory::get_memory_slide() {
    return hhdm_request.response->offset;
}

std::uintptr_t memory::get_kernel_base_address() {
    return kernel_address_request.response->virtual_base;
}

bool memory::map_virtual(const std::uintptr_t pagemap_virtual,
                         const std::uintptr_t virt, const std::uintptr_t phys,
                         const MapFlags flags) {
    const auto pml4_i = (virt >> 39) & 0x1FFULL;
    const auto pdpt_i = (virt >> 30) & 0x1FFULL;
    const auto pd_i = (virt >> 21) & 0x1FFULL;
    const auto pt_i = (virt >> 12) & 0x1FFULL;

    if (!virt || !core::check_aligned(virt, PAGE_SIZE)) {
        core::set_error(core::ErrorCode::IncorrectParameter);
        return false;
    }

    if (!phys || !core::check_aligned(phys, PAGE_SIZE)) {
        core::set_error(core::ErrorCode::IncorrectParameter);
        return false;
    }

    // root PML4 table
    auto *const pml4 = reinterpret_cast<std::uint64_t *>(pagemap_virtual);
    if (!pml4) {
        core::set_error(core::ErrorCode::IncorrectParameter);
        return false;
    }

    // allocate a table and zero it out
    auto alloc_table = []() -> std::pair<std::uint64_t *, std::uintptr_t> {
        const auto phys = alloc_page();
        if (!phys)
            return {nullptr, 0};

        const auto virt = phys + get_memory_slide();
        memset(reinterpret_cast<void *>(virt), 0, 0x1000);
        return {reinterpret_cast<std::uint64_t *>(virt), phys};
    };

    // ensure if table exists, if not allocate it
    auto ensure_table = [&](std::uint64_t &entry) -> std::uint64_t * {
        if (!(entry & static_cast<uint64_t>(PageBits::Present))) {
            auto [virt_table, phys_table] = alloc_table();
            if (!virt_table)
                return nullptr;

            entry = (phys_table & ppn_mask) |
                    static_cast<uint64_t>(PageBits::Present) |
                    static_cast<uint64_t>(PageBits::ReadWrite) |
                    static_cast<uint64_t>(PageBits::User);

            return virt_table;
        }
        return reinterpret_cast<std::uint64_t *>((entry & ppn_mask) +
                                                 get_memory_slide());
    };

    // walk the plank
    auto *pdpt = ensure_table(pml4[pml4_i]);
    if (!pdpt)
        return false;

    auto *pd = ensure_table(pdpt[pdpt_i]);
    if (!pd)
        return false;

    auto *pt = ensure_table(pd[pd_i]);
    if (!pt)
        return false;

    // finalize
    uint64_t entry_flags = static_cast<uint64_t>(PageBits::Present);

    if (core::is_flag_set(flags, MapFlags::Write))
        entry_flags |= static_cast<uint64_t>(PageBits::ReadWrite);
    if (core::is_flag_set(flags, MapFlags::User))
        entry_flags |= static_cast<uint64_t>(PageBits::User);
    if (core::is_flag_set(flags, MapFlags::WriteThrough))
        entry_flags |= static_cast<uint64_t>(PageBits::WriteThrough);
    if (core::is_flag_set(flags, MapFlags::CacheDisable))
        entry_flags |= static_cast<uint64_t>(PageBits::CacheDisable);
    if (!core::is_flag_set(flags, MapFlags::Execute))
        entry_flags |= static_cast<uint64_t>(PageBits::NoExecute);

    // write back to pt
    pt[pt_i] = (phys & ppn_mask) | entry_flags;

    // invalidate if cr3 == pagemap physical
    if (x86_64::get_cr3() == (pagemap_virtual - get_memory_slide()))
        x86_64::invlpg(reinterpret_cast<void *>(virt));

    return true;
}
