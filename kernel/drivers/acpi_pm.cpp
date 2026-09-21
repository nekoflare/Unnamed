#include "acpi_pm.hpp"

#include <uacpi/acpi.h>
#include <uacpi/tables.h>

#include "arch/x86_64/io.hpp"
#include "kernel/logger.hpp"

namespace {
    constexpr uint32_t ACPI_PM_FREQ_HZ = 3579545;

    class AcpiPmClocksource final : public clocksource::Clocksource {
        uint16_t port_;

    public:
        AcpiPmClocksource(const uint16_t port, const uint64_t mask) :
            Clocksource("acpi_pm", 100, mask,
                        clocksource::compute_ms(ACPI_PM_FREQ_HZ)),
            port_(port) {}

        [[nodiscard]] uint64_t read() const override {
            return x86_64::inl(port_);
        }
    };
} // namespace

void acpi_pm::init() {
    struct acpi_fadt *fadt = nullptr;
    auto ret = uacpi_table_fadt(&fadt);
    if (uacpi_unlikely_error(ret) || !fadt) {
        logger::debug("acpi_pm: no FADT table\n");
        return;
    }

    const uint16_t port = static_cast<uint16_t>(fadt->pm_tmr_blk);
    if (port == 0) {
        logger::debug("acpi_pm: pm_tmr_blk is zero\n");
        return;
    }

    const uint8_t width = fadt->pm_tmr_len;
    const uint64_t mask = (width == 4) ? 0xFFFFFFFF : 0xFFFFFF;

    logger::debug("acpi_pm: port=0x%x, width=%u bits\n", port, width * 8);

    auto *cs = new AcpiPmClocksource(port, mask);
    clocksource::register_clocksource(cs);
}
