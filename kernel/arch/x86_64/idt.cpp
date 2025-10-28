//
// Created by neko on 10/28/25.
//

#include "idt.hpp"

#include "kernel/logger.hpp"

static x86_64::IdtEntry idt[256]{};

extern "C" uintptr_t exception_handler_pointers[];

static void init_idt_table() {
    for (std::size_t i = 0; i < 256; i++) {
        idt[i] = x86_64::create_idt_entry(
                (uintptr_t) (exception_handler_pointers[i]), 0 /* ist */,
                x86_64::GateType::Interrupt, x86_64::Dpl::Ring0,
                true /* present */, 0x8 /* kernel segment */);
    }
}

void x86_64::init_idt() {
    IdtRegister idt_register = {.size = sizeof(idt) - 1,
                                .address = reinterpret_cast<uintptr_t>(&idt)};

    init_idt_table();
    flush_idt(&idt_register);
}

void x86_64::interrupt_handler(RegistersX64 *regs) {
    (void) regs;

    logger::debug("Got an interrupt. Number: %lx", regs->interrupt_number);
}
