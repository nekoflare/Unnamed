#ifndef KERNEL_CLOCKSOURCE_HPP
#define KERNEL_CLOCKSOURCE_HPP

#include <cstdint>
#include <utility>

#include "kernel/bit_helpers.hpp"
#include "lib/string.hpp"
#include "lib/vector.hpp"

namespace clocksource {
    struct mult_shift {
        uint32_t mult;
        uint32_t shift;
    };

    class Clocksource {
    public:
        enum class Flags : uint32_t {
            NONE = 0,
            CONTINUOUS = 1 << 0,
            HIGH_RES = 1 << 1,
        };

    protected:
        String name_;
        uint32_t rating_;
        uint64_t mask_;
        uint32_t mult_;
        uint32_t shift_;
        uint64_t cycle_last_;
        Flags flags_;

    public:
        Clocksource(String name, const uint32_t rating, const uint64_t mask,
                    const mult_shift ms, const Flags flags = Flags::NONE) :
            name_(std::move(name)), rating_(rating), mask_(mask),
            mult_(ms.mult), shift_(ms.shift), cycle_last_(0), flags_(flags) {}

        virtual ~Clocksource() = default;

        virtual uint64_t read() const = 0;

        [[nodiscard]] const String &name() const { return name_; }
        [[nodiscard]] uint32_t rating() const { return rating_; }
        [[nodiscard]] uint64_t mask() const { return mask_; }
        [[nodiscard]] uint32_t mult() const { return mult_; }
        [[nodiscard]] uint32_t shift() const { return shift_; }
        [[nodiscard]] uint64_t cycle_last() const { return cycle_last_; }
        [[nodiscard]] Flags flags() const { return flags_; }

        [[nodiscard]] bool isContinuous() const {
            return static_cast<bool>(static_cast<uint32_t>(flags_) &
                                     static_cast<uint32_t>(Flags::CONTINUOUS));
        }
        [[nodiscard]] bool isHighRes() const {
            return static_cast<bool>(static_cast<uint32_t>(flags_) &
                                     static_cast<uint32_t>(Flags::HIGH_RES));
        }

        void update_cycle_last(const uint64_t cycles) { cycle_last_ = cycles; }

        [[nodiscard]] uint64_t read_delta() const {
            const uint64_t now = read();
            const uint64_t last = cycle_last_;
            return (now - last) & mask_;
        }
    };

    uint64_t cycles_to_ns(uint64_t cycles, uint32_t mult, uint32_t shift);
    uint32_t compute_shift();
    mult_shift compute_ms(uint32_t freq_hz, uint32_t shift = 0);

    void register_clocksource(Clocksource *cs);
    Clocksource *get_best();
    void init();
} // namespace clocksource

#endif // KERNEL_CLOCKSOURCE_HPP
