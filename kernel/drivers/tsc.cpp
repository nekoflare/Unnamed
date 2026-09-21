#include "tsc.hpp"

#include <limine.h>

#include "arch/x86_64/msr.hpp"
#include "kernel/logger.hpp"

namespace {
    volatile limine_tsc_frequency_request tsc_freq_request = {
            .id = LIMINE_TSC_FREQUENCY_REQUEST_ID,
            .revision = 0,
            .response = nullptr,
    };

    class TscClocksource final : public clocksource::Clocksource {
    public:
        explicit TscClocksource(const uint32_t freq_hz) :
            Clocksource("tsc", 300, 0xFFFFFFFFFFFFFFFF,
                        clocksource::compute_ms(freq_hz),
                        Flags::CONTINUOUS | Flags::HIGH_RES) {}

        [[nodiscard]] uint64_t read() const override {
            uint32_t lo, hi;
            asm volatile("lfence\n\t"
                         "rdtsc"
                         : "=a"(lo), "=d"(hi)
                         :
                         : "memory");
            return (static_cast<uint64_t>(hi) << 32) | lo;
        }
    };
} // namespace

void tsc::init() {
    if (!tsc_freq_request.response) {
        logger::debug("tsc: no TSC frequency from bootloader\n");
        return;
    }

    const uint32_t freq_hz =
            static_cast<uint32_t>(tsc_freq_request.response->frequency);
    if (freq_hz == 0) {
        logger::debug("tsc: TSC frequency is zero\n");
        return;
    }

    logger::debug("tsc: frequency = %u Hz\n", freq_hz);
    auto *cs = new TscClocksource(freq_hz);
    clocksource::register_clocksource(cs);
}
