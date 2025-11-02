//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_ERROR_HPP
#define KERNEL_ERROR_HPP

namespace core {
    enum class ErrorCode {
        MemoryMisaligned,
        IncorrectParameter,
        OutOfMemory,
        MapFail
    };
}

#endif // KERNEL_ERROR_HPP
