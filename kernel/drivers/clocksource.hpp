//
// Created by neko on 11/2/25.
//

#ifndef KERNEL_CLOCKSOURCE_HPP
#define KERNEL_CLOCKSOURCE_HPP
#include <utility>

#include "lib/string.hpp"

namespace clocksource {
    class Clocksource {
    public:
        enum class Flags : uint32_t {
            NONE = 0,
            CONTINUOUS = 1 << 0,
            HIGH_RES = 1 << 1,
        };

    private:
    protected:
        String name_;
        uint32_t rating_;
        uint64_t mask_;
        uint32_t shift_;
        Flags flags_;

    public:
        Clocksource(String name, const uint32_t rating, const uint64_t mask,
                    const uint32_t shift, const Flags flags = Flags::NONE) :
            name_(std::move(name)), rating_(rating), mask_(mask), shift_(shift),
            flags_(flags) {}

        virtual ~Clocksource() = default;

        // Pure virtual function for reading current ticks
        virtual uint64_t read() const ;

        [[nodiscard]] const String &name() const { return name_; }
        [[nodiscard]] uint32_t rating() const { return rating_; }
        [[nodiscard]] uint64_t mask() const { return mask_; }
        [[nodiscard]] uint32_t shift() const { return shift_; }
        [[nodiscard]] Flags flags() const { return flags_; }

        [[nodiscard]] bool isContinuous() const {
            return static_cast<bool>(static_cast<uint32_t>(flags_) &
                                     static_cast<uint32_t>(Flags::CONTINUOUS));
        }
        [[nodiscard]] bool isHighRes() const {
            return static_cast<bool>(static_cast<uint32_t>(flags_) &
                                     static_cast<uint32_t>(Flags::HIGH_RES));
        }
    };

    void init();
    void register_clocksources();
} // namespace clocksource

#endif // KERNEL_CLOCKSOURCE_HPP
