//
// Created by neko on 11/1/25.
//

#ifndef KERNEL_SPINLOCK_HPP
#define KERNEL_SPINLOCK_HPP

#include <atomic>

class Spinlock {
public:
    std::atomic_flag locked_ = ATOMIC_FLAG_INIT;
private:
    void lock() noexcept;
    void unlock() noexcept;
};

#endif //KERNEL_SPINLOCK_HPP
