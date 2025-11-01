//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_IDT_HPP
#define KERNEL_IDT_HPP

#include <cstdint>
#include "registers.hpp"

namespace x86_64 {

    struct [[gnu::packed]] IdtRegister {
        uint16_t size;
        uintptr_t address;
    };

    struct [[gnu::packed]] IdtEntry {
        uint16_t offset0;
        uint16_t segment;
        uint8_t ist : 3;
        uint8_t rsvd0 : 5;
        uint8_t gate_type : 4;
        uint8_t zero : 1;
        uint8_t dpl : 2;
        uint8_t present : 1;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t rsvd1;
    };

    enum class GateType : uint8_t { Interrupt = 0xE, Trap = 0xF };

    enum class Dpl : uint8_t { Ring0 = 0, Ring1 = 1, Ring2 = 2, Ring3 = 3 };

    [[nodiscard]]
    constexpr IdtEntry
    create_idt_entry(uintptr_t offset, uint8_t ist, GateType gate_type, Dpl dpl,
                     bool present, uint16_t segment) noexcept {
        IdtEntry entry{};
        entry.offset0 = static_cast<uint16_t>(offset & 0xFFFF);
        entry.offset1 = static_cast<uint16_t>((offset >> 16) & 0xFFFF);
        entry.offset2 = static_cast<uint32_t>((offset >> 32) & 0xFFFFFFFF);
        entry.ist = ist & 0x7;
        entry.gate_type = static_cast<uint8_t>(gate_type);
        entry.dpl = static_cast<uint8_t>(dpl);
        entry.present = present ? 1 : 0;
        entry.segment = segment;
        return entry;
    }

    void init_idt();
    extern "C" void interrupt_handler(Registers *regs);
    extern "C" void flush_idt(const IdtRegister *reg);

} // namespace x86_64

#endif // KERNEL_IDT_HPP
