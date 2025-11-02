//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_UTILS_HPP
#define KERNEL_UTILS_HPP

#include <type_traits>

template<typename T>
constexpr T align_up(T value, std::size_t alignment) noexcept {
    static_assert(std::is_integral_v<T>, "align_up requires an integral type");
    return (value + static_cast<T>(alignment - 1)) &
           ~static_cast<T>(alignment - 1);
}

#endif // KERNEL_UTILS_HPP
