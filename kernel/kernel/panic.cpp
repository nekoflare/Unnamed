//
// Created by neko on 11/1/25.
//

#include "arch/x86_64/cpu.hpp"
#include "logger.hpp"

namespace core {
    extern "C" void abort() {
        logger::debug("abort() has been called, halting.");
        x86_64::hcf();
    }
} // namespace core
