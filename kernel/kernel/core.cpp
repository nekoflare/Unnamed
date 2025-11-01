#include "arch/x86_64/gdt.hpp"
#include "arch/x86_64/idt.hpp"
#include "logger.hpp"
#include "mm/pma.hpp"
#include "mm/vma.hpp"
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

    memory::VAddrAllocator<1024> virtualMemoryAllocator;
    virtualMemoryAllocator.init(0xffffffff80000000, 0x10000000, PAGE_SIZE);
    auto addr1 = virtualMemoryAllocator.alloc_pages(16).value();
    auto addr2 = virtualMemoryAllocator.alloc_pages(16).value();
    auto addr3 = virtualMemoryAllocator.alloc_pages(16).value();
    auto addr4 = virtualMemoryAllocator.alloc_pages(16).value();

    logger::debug("%lx %lx %lx %lx", addr1, addr2, addr3, addr4);

    while (true) {
        asm volatile("cli; hlt");
    }
}
