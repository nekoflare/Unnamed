//
// Created by neko on 11/1/25.
//

#include "memory.hpp"

extern "C"
{

    void* memset(void* dest, int ch, std::size_t count) {
        if (!dest || count == 0) return dest;
        auto* ptr = static_cast<unsigned char*>(dest);
        for (std::size_t i = 0; i < count; i++) ptr[i] = static_cast<unsigned char>(ch);
        return dest;
    }

}