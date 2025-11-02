//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_BIT_HELPERS_HPP
#define KERNEL_BIT_HELPERS_HPP

#include <type_traits>

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T operator|(T a, T b) noexcept {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T operator&(T a, T b) noexcept {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(a) & static_cast<U>(b));
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T operator^(T a, T b) noexcept {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(a) ^ static_cast<U>(b));
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T operator~(T a) noexcept {
    using U = std::underlying_type_t<T>;
    return static_cast<T>(~static_cast<U>(a));
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T &operator|=(T &a, T b) noexcept {
    return a = a | b;
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T &operator&=(T &a, T b) noexcept {
    return a = a & b;
}

template<typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
constexpr T &operator^=(T &a, T b) noexcept {
    return a = a ^ b;
}

#endif // KERNEL_BIT_HELPERS_HPP
