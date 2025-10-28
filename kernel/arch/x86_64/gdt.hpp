//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_GDT_HPP
#define KERNEL_GDT_HPP

#include <cstdint>

namespace x86_64 {
    struct [[gnu::packed]] GdtRegister {
        uint16_t size; // sizeof(gdt) - 1
        uint64_t base;
    };

    struct [[gnu::packed]] GdtSegmentDescriptor {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t limit_high : 4;
        uint8_t flags : 4;
        uint8_t base_high;
    };

    enum class Access : uint8_t {
        Accessed        = 1 << 0,
        ReadWrite       = 1 << 1,
        Direction       = 1 << 2,
        Executable      = 1 << 3,
        DescriptorType  = 1 << 4,
        Dpl0            = 0,
        Dpl1            = 1 << 5,
        Dpl2            = 1 << 6,
        Dpl3            = (1 << 5) | (1 << 6),
        Present         = 1 << 7
    };

    enum class Flags : uint8_t {
        None        = 0,
        LongMode    = 1 << 1,
        Size        = 1 << 2,
        Granularity = 1 << 3
    };

    constexpr Access operator|(Access lhs, Access rhs) noexcept {
        return static_cast<Access>(
            static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
        );
    }

    constexpr Flags operator|(Flags lhs, Flags rhs) noexcept {
        return static_cast<Flags>(
            static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
        );
    }

    constexpr bool operator&(Access lhs, Access rhs) noexcept {
        return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
    }

    constexpr bool operator&(Flags lhs, Flags rhs) noexcept {
        return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
    }

    [[nodiscard]]
    constexpr GdtSegmentDescriptor create_descriptor(
        uint64_t base,
        uint64_t limit,
        Access access,
        Flags flags
    ) noexcept {
        GdtSegmentDescriptor desc{};

        desc.limit_low  = static_cast<uint16_t>(limit & 0xFFFF);
        desc.base_low   = static_cast<uint16_t>(base & 0xFFFF);
        desc.base_mid   = static_cast<uint8_t>((base >> 16) & 0xFF);
        desc.access     = static_cast<uint8_t>(access);
        desc.limit_high = static_cast<uint8_t>((limit >> 16) & 0xF);
        desc.flags      = static_cast<uint8_t>(flags);
        desc.base_high  = static_cast<uint8_t>((base >> 24) & 0xFF);

        return desc;
    }

    void init_gdt();
}

#endif //KERNEL_GDT_HPP
