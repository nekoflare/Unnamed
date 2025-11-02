//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_SPINLOCK_HPP
#define KERNEL_SPINLOCK_HPP

#include <atomic>

class Spinlock {
private:
    std::atomic_flag locked_ = ATOMIC_FLAG_INIT;

public:
    void lock() noexcept;
    void unlock() noexcept;
};

#endif // KERNEL_SPINLOCK_HPP
