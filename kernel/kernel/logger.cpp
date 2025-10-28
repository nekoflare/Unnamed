//
// Created by neko on 10/28/25.
//

#define NANOPRINTF_IMPLEMENTATION
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 0

#include <lib/nanoprintf/nanoprintf.h>

#include "arch/x86_64/io.hpp"
#include "logger.hpp"

static void log_to_e9(const char *buf, size_t bufsz) {
    for (size_t i = 0; (i < bufsz) && (buf[i]); i++) {
        outb(0xE9, buf[i]);
    }
}

void logger::debug(const char *fmt, ...) noexcept {
    va_list args;
    va_start(args, fmt);

    char buf[1024];
    npf_vsnprintf(buf, sizeof(buf), fmt, args);
    buf[sizeof(buf) - 1] = '\0';

    log_to_e9(buf, 1024);
    log_to_e9("\n", 1);

    va_end(args);
}
