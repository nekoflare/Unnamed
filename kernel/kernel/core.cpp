#include "arch/x86_64/gdt.hpp"
#include "logger.hpp"
#include "arch/x86_64/idt.hpp"
extern "C" {
extern void (*__init_array[])(void);
extern void (*__init_array_end[])(void);
extern void (*__fini_array[])(void);
extern void (*__fini_array_end[])(void);
}

static inline void run_constructors() {
    for (auto ctor = __init_array; ctor < __init_array_end; ++ctor)
        (*ctor)();
}

extern "C" void kernel_main() {
    run_constructors();
    x86_64::init_gdt();
    x86_64::init_idt();
    logger::debug("wsg bro");

    asm volatile ("int $0x69");

    while (true) {
        asm volatile("cli; hlt");
    }
}
