//
// Created by neko on 11/2/25.
//

#include "acpi.hpp"
#include <limine.h>

#include "arch/x86_64/cpu.hpp"
#include "kernel/logger.hpp"
#include "mm/vmm.hpp"
#include "uacpi/status.h"
#include "uacpi/types.h"
#include "uacpi/uacpi.h"

volatile limine_rsdp_request rsdp_request = {.id = LIMINE_RSDP_REQUEST,
                                             .revision = LIMINE_API_REVISION,
                                             .response = nullptr};

std::uintptr_t acpi::get_rsdp_physical_pointer() {
    return reinterpret_cast<std::uintptr_t>(rsdp_request.response->address) -
           memory::get_memory_slide();
}

void acpi::init_stage_1() {
    uacpi_status ret = uacpi_initialize(0);
    if (uacpi_unlikely_error(ret)) {
        logger::debug("uacpi_initialize error: %s\n",
                      uacpi_status_to_string(ret));
        x86_64::hcf();
    }
}
