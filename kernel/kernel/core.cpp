extern "C" void kernel_main() {
    while (true) {
        asm volatile ("cli; hlt");
    }
}