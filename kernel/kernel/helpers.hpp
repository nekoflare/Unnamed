//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_HELPERS_HPP
#define KERNEL_HELPERS_HPP
#include <cstdint>
#include <type_traits>

namespace core {
    inline bool check_aligned(const std::uintptr_t addr,
                              const std::size_t alignment) {
        return (addr & (alignment - 1)) == 0;
    }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    inline bool check_aligned(const T addr, const std::size_t alignment) {
        return check_aligned(static_cast<std::uintptr_t>(addr), alignment);
    }

    template<typename T>
    inline bool is_flag_set(T v, T bit) {
        return (v & bit) == bit;
    }

    template<typename T>
    constexpr T align_up(T value, T alignment) {
        static_assert(std::is_integral_v<T>, "align_up requires integral type");
        return (value + alignment - 1) & ~(alignment - 1);
    }

    // Align down to nearest multiple of alignment
    template<typename T>
    constexpr T align_down(T value, T alignment) {
        static_assert(std::is_integral_v<T>,
                      "align_down requires integral type");
        return value & ~(alignment - 1);
    }
} // namespace core

#endif // KERNEL_HELPERS_HPP
