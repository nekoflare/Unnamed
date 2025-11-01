//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_MEMORY_HPP
#define KERNEL_MEMORY_HPP

#include <cstdlib>
#include <cstddef>

extern "C"
{
    void* memset(void* dest, int ch, std::size_t count);
    void* memcpy(void* dest, const void* src, size_t n);
}

#endif //KERNEL_MEMORY_HPP
