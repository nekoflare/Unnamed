//
// Created by neko on 11/2/25.
//

#ifndef KERNEL_EXCEPTION_HPP
#define KERNEL_EXCEPTION_HPP

#define EXCEPTION(_s)                                                          \
    do {                                                                       \
        logger::debug("%s", _s);                                               \
        x86_64::hcf();                                                         \
    } while (0);

#endif // KERNEL_EXCEPTION_HPP
