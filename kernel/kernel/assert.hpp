// Created by neko on 10/28/25.

#ifndef KERNEL_ASSERT_HPP
#define KERNEL_ASSERT_HPP

#include "arch/x86_64/cpu.hpp"
#include "logger.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define ASSERT(expr) core::assert((expr), __FILE__ ":" TOSTRING(__LINE__))

namespace core {

    inline void assert(bool expr, const char *location) {
        if (!expr) {
            logger::debug("Assertion failed at %s!", location);
            x86_64::hcf();
        }
    }

} // namespace core

#endif // KERNEL_ASSERT_HPP
