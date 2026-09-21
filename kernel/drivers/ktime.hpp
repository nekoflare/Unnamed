#ifndef KERNEL_KTIME_HPP
#define KERNEL_KTIME_HPP

#include <cstdint>

#include "clocksource.hpp"

struct ktime {
    int64_t ns;

    static ktime from_cycles(uint64_t cycles,
                             const clocksource::Clocksource &cs) {
        return {static_cast<int64_t>(
                clocksource::cycles_to_ns(cycles, cs.mult(), cs.shift()))};
    }

    static ktime now() {
        auto *cs = clocksource::get_best();
        uint64_t raw = cs->read();
        return from_cycles(raw, *cs);
    }

    ktime operator+(const ktime o) const { return {ns + o.ns}; }
    ktime operator-(const ktime o) const { return {ns - o.ns}; }
    ktime operator+(const int64_t delta_ns) const { return {ns + delta_ns}; }
    ktime operator-(const int64_t delta_ns) const { return {ns - delta_ns}; }
    ktime &operator+=(const ktime o) {
        ns += o.ns;
        return *this;
    }
    ktime &operator-=(const ktime o) {
        ns -= o.ns;
        return *this;
    }

    bool operator<(const ktime o) const { return ns < o.ns; }
    bool operator>(const ktime o) const { return ns > o.ns; }
    bool operator<=(const ktime o) const { return ns <= o.ns; }
    bool operator>=(const ktime o) const { return ns >= o.ns; }
    bool operator==(const ktime o) const { return ns == o.ns; }
    bool operator!=(const ktime o) const { return ns != o.ns; }
};

#endif // KERNEL_KTIME_HPP
