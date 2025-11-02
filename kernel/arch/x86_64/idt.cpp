//
// Created by neko on 10/28/25.
//

#include "idt.hpp"

#include "cpu.hpp"
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

void print_registers(const x86_64::Registers *regs) {
    logger::debug("=== CPU Registers ===\n");
    logger::debug(
            "RAX: 0x%016lx  RBX: 0x%016lx  RCX: 0x%016lx  RDX: 0x%016lx\n",
            regs->rax, regs->rbx, regs->rcx, regs->rdx);
    logger::debug(
            "RSI: 0x%016lx  RDI: 0x%016lx  RBP: 0x%016lx  RSP: 0x%016lx\n",
            regs->rsi, regs->rdi, regs->rbp, regs->original_rsp);
    logger::debug(
            "R8 : 0x%016lx  R9 : 0x%016lx  R10: 0x%016lx  R11: 0x%016lx\n",
            regs->r8, regs->r9, regs->r10, regs->r11);
    logger::debug(
            "R12: 0x%016lx  R13: 0x%016lx  R14: 0x%016lx  R15: 0x%016lx\n",
            regs->r12, regs->r13, regs->r14, regs->r15);
    logger::debug("ES : 0x%016lx  DS : 0x%016lx\n", regs->es, regs->ds);
    logger::debug("CS : 0x%016lx  SS : 0x%016lx  RFLAGS: 0x%016lx\n", regs->cs,
                  regs->ss, regs->rflags);
    logger::debug("RIP: 0x%016lx  INT#: 0x%lx  ERR: 0x%lx\n", regs->rip,
                  regs->interrupt_number, regs->error_code);
    logger::debug("====================\n");
}

void x86_64::interrupt_handler(Registers *regs) {
    (void) regs;

    logger::debug("Got an interrupt. Number: %lx\n", regs->interrupt_number);

    print_registers(regs);

    if (regs->interrupt_number < 32) {
        x86_64::hcf();
    }
}
