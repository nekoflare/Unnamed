//
// Created by neko on 11/2/25.
//

#include "irqs.hpp"

#include "arch/x86_64/cpu.hpp"
#include "kernel/logger.hpp"
#include "lib/vector.hpp"
#include "uacpi/acpi.h"
#include "uacpi/tables.h"

namespace irqs {
    uacpi_table madt_table;
    Vector<acpi_madt_lapic_nmi> lapic_nmi_table;
    Vector<acpi_madt_ioapic> ioapic_table;
    Vector<acpi_madt_lapic> lapic_table;
    Vector<acpi_madt_interrupt_source_override> interrupt_source_override_table;

    void load_entries();
} // namespace irqs


void irqs::load_entries() {
    auto table_entries_size =
            madt_table.hdr->length - sizeof(*madt_table.hdr) - sizeof(uint64_t);
    size_t read = 0;

    auto handle_lapic = [](acpi_entry_hdr *e) {
        auto *lapic = reinterpret_cast<acpi_madt_lapic *>(e);
        lapic_table.push_back(*lapic);
        logger::debug("Found LAPIC: uid=%u, id=%u, flags=%x\n", lapic->uid,
                      lapic->id, lapic->flags);
    };

    auto handle_lapic_nmi = [](acpi_entry_hdr *e) {
        auto *nmi = reinterpret_cast<acpi_madt_lapic_nmi *>(e);
        lapic_nmi_table.push_back(*nmi);
        logger::debug("Found LAPIC NMI: uid=%u, flags=%x, lint=%u\n", nmi->uid,
                      nmi->flags, nmi->lint);
    };

    auto handle_interrupt_source_override = [](acpi_entry_hdr *e) {
        auto *iso = reinterpret_cast<acpi_madt_interrupt_source_override *>(e);
        interrupt_source_override_table.push_back(*iso);
        logger::debug("Found Interrupt Source Override: bus=%u, source=%u, "
                      "gsi=%u, flags=%x\n",
                      iso->bus, iso->source, iso->gsi, iso->flags);
    };

    auto handle_ioapic = [](acpi_entry_hdr *e) {
        auto *ioapic = reinterpret_cast<acpi_madt_ioapic *>(e);
        ioapic_table.push_back(*ioapic);
        logger::debug("Found IOAPIC: id=%u, address=%x, gsi_base=%u\n",
                      ioapic->id, ioapic->address, ioapic->gsi_base);
    };

    auto handle_unknown = [](const acpi_entry_hdr *e) {
        logger::debug("Found unhandled MADT entry: %d.\n", e->type);
    };

    while (table_entries_size > read) {
        auto *e = reinterpret_cast<acpi_entry_hdr *>(madt_table.virt_addr +
                                                     sizeof(acpi_madt) + read);

        switch (e->type) {
            case ACPI_MADT_ENTRY_TYPE_LAPIC:
                handle_lapic(e);
                break;
            case ACPI_MADT_ENTRY_TYPE_LAPIC_NMI:
                handle_lapic_nmi(e);
                break;
            case ACPI_MADT_ENTRY_TYPE_INTERRUPT_SOURCE_OVERRIDE:
                handle_interrupt_source_override(e);
                break;
            case ACPI_MADT_ENTRY_TYPE_IOAPIC:
                handle_ioapic(e);
                break;
            default:
                handle_unknown(e);
                break;
        }

        read += e->length;
    }
}

void irqs::init() {
    auto ret = uacpi_table_find_by_signature("APIC", &madt_table);

    if (uacpi_unlikely_error(ret)) {
        logger::debug("Failed to get APIC table.\n");
        x86_64::hcf();
        __builtin_unreachable();
    }

    if (!madt_table.virt_addr) {
        logger::debug("Failed to get APIC table.\n");
        x86_64::hcf();
        __builtin_unreachable();
    }

    load_entries();
}
