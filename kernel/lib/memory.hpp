//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_MEMORY_HPP
#define KERNEL_MEMORY_HPP

#include <cstddef>
#include <cstdlib>

extern "C" {
void *memset(void *dest, int ch, std::size_t count);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memmove(void *dest, const void *src, size_t n);
}

#endif // KERNEL_MEMORY_HPP
