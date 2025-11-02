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

    void* memcpy(void* dest, const void* src, size_t n) {
        char* d = static_cast<char*>(dest);
        const char* s = static_cast<const char*>(src);
        for (size_t i = 0; i < n; i++) d[i] = s[i];
        return dest;
    }

    int memcmp(const void* s1, const void* s2, size_t n) {
        const auto *p1 = static_cast<const unsigned char*>(s1), *p2 = static_cast<const unsigned char*>(s2);
        for (size_t i = 0; i < n; i++)
            if (p1[i] != p2[i]) return p1[i] - p2[i];
        return 0;
    }

    void* memmove(void* dest, const void* src, size_t n) {
        if (dest < src) {
            return memcpy(dest, src, n);
        } else if (dest > src) {
            auto  d = static_cast<char*>(dest);
            const auto* s = static_cast<const char*>(src);
            for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
            return dest;
        }
        return dest;
    }
}