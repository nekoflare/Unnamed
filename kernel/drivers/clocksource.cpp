#include "clocksource.hpp"

#include "acpi_pm.hpp"
#include "hpet.hpp"
#include "kernel/logger.hpp"
#include "tsc.hpp"

static Vector<clocksource::Clocksource *> sources;
static clocksource::Clocksource *best = nullptr;

uint64_t clocksource::cycles_to_ns(const uint64_t cycles, const uint32_t mult,
                                   const uint32_t shift) {
    return (cycles * mult) >> shift;
}

uint32_t clocksource::compute_shift() { return 32; }

clocksource::mult_shift clocksource::compute_ms(const uint32_t freq_hz,
                                                uint32_t shift) {
    if (shift == 0)
        shift = compute_shift();
    const uint64_t sec = 1ULL << shift;
    const uint64_t mult = (sec * 1000000000ULL) / freq_hz;
    return {static_cast<uint32_t>(mult), shift};
}

void clocksource::register_clocksource(Clocksource *cs) {
    sources.push_back(cs);
    logger::debug("clocksource: registered \"%s\" (rating=%u)\n",
                  cs->name().c_str(), cs->rating());
}

clocksource::Clocksource *clocksource::get_best() { return best; }

void clocksource::init() {
    tsc::init();
    hpet::init();
    acpi_pm::init();

    if (sources.empty()) {
        logger::debug("clocksource: no clocksources registered\n");
        return;
    }

    Clocksource *winner = sources[0];
    for (size_t i = 1; i < sources.size(); ++i) {
        if (sources[i]->rating() > winner->rating())
            winner = sources[i];
    }

    best = winner;
    logger::debug("clocksource: selected \"%s\" (rating=%u)\n",
                  best->name().c_str(), best->rating());
}
