//
// Created by neko on 10/28/25.
//

#include "vmm.hpp"
#include <limine.h>

static volatile limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST,
                                                    .revision =
                                                            LIMINE_API_REVISION,
                                                    .response = nullptr};

uintptr_t memory::get_memory_slide() { return hhdm_request.response->offset; }
