//
// Created by neko on 11/1/25.
//

#include "spinlock.hpp"

void Spinlock::lock() noexcept {
    while (locked_.test_and_set(std::memory_order_acquire)) {
        // busy
    }
}

void Spinlock::unlock() noexcept {
    locked_.clear(std::memory_order_release);
}