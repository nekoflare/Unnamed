//
// Created by neko on 11/2/25.
//

#ifndef KERNEL_ACPI_HPP
#define KERNEL_ACPI_HPP

#include <cstdint>

namespace acpi {
    std::uintptr_t get_rsdp_physical_pointer();
    void init_stage_1();
} // namespace acpi

#endif // KERNEL_ACPI_HPP
