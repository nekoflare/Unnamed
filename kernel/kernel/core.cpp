#include "acpi/acpi.hpp"
#include "arch/x86_64/cpu.hpp"
#include "arch/x86_64/gdt.hpp"
#include "arch/x86_64/idt.hpp"
#include "irqs/irqs.hpp"
#include "logger.hpp"
#include "mm/heap.hpp"
#include "mm/pma.hpp"
#include "mm/vma.hpp"
#include "mm/vmm.hpp"
#include "percpu.hpp"

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
    core::setup_bsp_percpu();
    x86_64::init_gdt();
    x86_64::init_idt();
    memory::init_pma();
    memory::init_kernel_virtual_allocator();
    memory::init_heap();
    acpi::init_stage_1();
    irqs::init();

    while (true) {
        asm volatile("cli; hlt");
    }
}
