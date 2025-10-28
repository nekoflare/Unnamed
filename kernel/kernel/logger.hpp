//
// Created by neko on 10/28/25.
//

#ifndef KERNEL_LOGGER_HPP
#define KERNEL_LOGGER_HPP

#include <string_view>

namespace logger {

    void debug(const char *fmt, ...) noexcept
            __attribute__((format(printf, 1, 2)));

}

#endif // KERNEL_LOGGER_HPP
