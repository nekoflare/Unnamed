//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_HEAP_HPP
#define KERNEL_HEAP_HPP

#ifdef __cplusplus
#    include <cstdint>
#    include <cstddef>
#else
#    include <stdint.h>
#    include <stddef.h>
#endif

#define MAP_PRIVATE 1
#define MAP_ANONYMOUS 2
#define PROT_READ 1
#define PROT_WRITE 2

#ifdef __cplusplus
extern "C" {
#endif

    void* mmap(void* addr, size_t length, int prot, int flags, int fd, intptr_t offset);
    int munmap(void* addr, size_t length);
    long unsigned int sbrk(ptrdiff_t length);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace memory {
    void init_heap();
}
#endif

#endif // KERNEL_HEAP_HPP
