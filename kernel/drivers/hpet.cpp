#include "hpet.hpp"

#include <limine.h>
#include <uacpi/acpi.h>
#include <uacpi/tables.h>

#include "kernel/logger.hpp"
#include "mm/vmm.hpp"

namespace {
    constexpr uint64_t HPET_GENERAL_CAPS = 0x000;
    constexpr uint64_t HPET_GENERAL_CONFIG = 0x010;
    constexpr uint64_t HPET_MAIN_COUNTER = 0x0F0;

    constexpr uint64_t HPET_CFG_ENABLE = (1 << 0);

    constexpr uint64_t HPET_FEMTOSEC_PER_SEC = 1000000000000000ULL;

    class HpetClocksource final : public clocksource::Clocksource {
        volatile uint32_t *base_;

        volatile uint32_t &reg(uint64_t offset) const {
            return *reinterpret_cast<volatile uint32_t *>(
                    reinterpret_cast<uintptr_t>(base_) + offset);
        }

    public:
        HpetClocksource(volatile uint32_t *base, const uint32_t freq_hz) :
            Clocksource("hpet", 200, 0xFFFFFFFF,
                        clocksource::compute_ms(freq_hz), Flags::CONTINUOUS),
            base_(base) {
            const uint32_t cfg = reg(HPET_GENERAL_CONFIG);
            reg(HPET_GENERAL_CONFIG) = cfg | HPET_CFG_ENABLE;
        }

        [[nodiscard]] uint64_t read() const override {
            return reg(HPET_MAIN_COUNTER);
        }
    };

    uint32_t hpet_period_fs(volatile uint32_t *base) {
        (void) base[HPET_GENERAL_CAPS / 4];
        return base[HPET_GENERAL_CAPS / 4 + 1];
    }
} // namespace

void hpet::init() {
    uacpi_table table;
    auto ret = uacpi_table_find_by_signature(ACPI_HPET_SIGNATURE, &table);
    if (uacpi_unlikely_error(ret) || !table.virt_addr) {
        logger::debug("hpet: no HPET ACPI table\n");
        return;
    }

    auto *hpet = reinterpret_cast<struct acpi_hpet *>(table.virt_addr);

    if (hpet->address.address_space_id != ACPI_AS_ID_SYS_MEM) {
        logger::debug("hpet: address space is not system memory (id=%u)\n",
                      hpet->address.address_space_id);
        return;
    }

    const auto phys = static_cast<uintptr_t>(hpet->address.address);
    auto *base = reinterpret_cast<volatile uint32_t *>(
            phys + memory::get_memory_slide());

    const uint32_t period_fs = hpet_period_fs(base);
    if (period_fs == 0) {
        logger::debug("hpet: zero period in capabilities\n");
        return;
    }

    const uint32_t freq_hz = HPET_FEMTOSEC_PER_SEC / period_fs;
    logger::debug("hpet: period=%u fs, freq=%u Hz\n", period_fs, freq_hz);

    auto *cs = new HpetClocksource(base, freq_hz);
    clocksource::register_clocksource(cs);
}
