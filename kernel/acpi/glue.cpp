//
// Created by neko on 11/2/25.
//

#include <kernel/logger.hpp>
#include <uacpi/kernel_api.h>
#include <uacpi/platform/arch_helpers.h>
#include <uacpi/status.h>

#include "acpi.hpp"
#include "lib/dlmalloc/malloc.h"
#include "lib/spinlock.hpp"
#include "mm/vmm.hpp"
#include "uacpi_io.hpp"

#define UNIMPLEMENTED                                                          \
    do {                                                                       \
        logger::debug("Function %s is unimplemented.\n", __PRETTY_FUNCTION__); \
    } while (0);

extern "C" {

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address) {
    if (out_rsdp_address)
        *out_rsdp_address = acpi::get_rsdp_physical_pointer();

    return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
    (void) len;
    return reinterpret_cast<void *>(addr + memory::get_memory_slide());
}

void uacpi_kernel_unmap(void *addr, uacpi_size len) {
    (void) addr;
    (void) len;
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char *message) {
    (void) level;
    (void) message;

    logger::debug("%s", message);
}

#ifdef UACPI_FORMATTED_LOGGING
void uacpi_kernel_vlog(uacpi_log_level level, const uacpi_char *format,
                       uacpi_va_list vlist) {
    (void) level;
    (void) format;
    (void) vlist;
    UNIMPLEMENTED
}
#endif // UACPI_FORMATTED_LOGGING

#ifdef UACPI_KERNEL_INITIALIZATION
uacpi_status uacpi_kernel_initialize(uacpi_init_level current_init_lvl) {
    UNIMPLEMENTED;
    (void) current_init_lvl;
    return UACPI_STATUS_UNIMPLEMENTED;
}

void uacpi_kernel_deinitialize(void) { UNIMPLEMENTED; }
#endif // UACPI_KERNEL_INITIALIZATION

#ifndef UACPI_BAREBONES_MODE

uacpi_status uacpi_kernel_pci_device_open(uacpi_pci_address address,
                                          uacpi_handle *out_handle) {
    UNIMPLEMENTED;
    (void) address;
    if (out_handle)
        *out_handle = NULL;
    return UACPI_STATUS_UNIMPLEMENTED;
}

void uacpi_kernel_pci_device_close(uacpi_handle handle) {
    UNIMPLEMENTED;
    (void) handle;
}

uacpi_status uacpi_kernel_pci_read8(uacpi_handle device, uacpi_size offset,
                                    uacpi_u8 *value) {
    UNIMPLEMENTED;
    (void) device;
    (void) offset;
    if (value)
        *value = 0xFF;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_read16(uacpi_handle device, uacpi_size offset,
                                     uacpi_u16 *value) {
    UNIMPLEMENTED;
    (void) device;
    (void) offset;
    if (value)
        *value = 0xFFFF;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_read32(uacpi_handle device, uacpi_size offset,
                                     uacpi_u32 *value) {
    UNIMPLEMENTED;
    (void) device;
    (void) offset;
    if (value)
        *value = 0xFFFFFFFF;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_write8(uacpi_handle device, uacpi_size offset,
                                     uacpi_u8 value) {
    UNIMPLEMENTED;
    (void) device;
    (void) offset;
    (void) value;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_write16(uacpi_handle device, uacpi_size offset,
                                      uacpi_u16 value) {
    UNIMPLEMENTED;
    (void) device;
    (void) offset;
    (void) value;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_write32(uacpi_handle device, uacpi_size offset,
                                      uacpi_u32 value) {
    UNIMPLEMENTED;
    (void) device;
    (void) offset;
    (void) value;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len,
                                 uacpi_handle *out_handle) {
    if (!out_handle)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = new IoMap(base, len);
    *out_handle = reinterpret_cast<uacpi_handle>(map);
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) {
    if (!handle)
        return;
    delete static_cast<IoMap *>(handle);
}

uacpi_status uacpi_kernel_io_read8(uacpi_handle handle, uacpi_size offset,
                                   uacpi_u8 *out_value) {
    if (!handle || !out_value)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = static_cast<IoMap *>(handle);
    auto val = map->read8(offset);
    if (!val.has_value())
        return UACPI_STATUS_INVALID_ARGUMENT;
    *out_value = val.value();
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read16(uacpi_handle handle, uacpi_size offset,
                                    uacpi_u16 *out_value) {
    if (!handle || !out_value)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = static_cast<IoMap *>(handle);
    auto val = map->read16(offset);
    if (!val.has_value())
        return UACPI_STATUS_INVALID_ARGUMENT;
    *out_value = val.value();
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read32(uacpi_handle handle, uacpi_size offset,
                                    uacpi_u32 *out_value) {
    if (!handle || !out_value)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = static_cast<IoMap *>(handle);
    auto val = map->read32(offset);
    if (!val.has_value())
        return UACPI_STATUS_INVALID_ARGUMENT;
    *out_value = val.value();
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_write8(uacpi_handle handle, uacpi_size offset,
                                    uacpi_u8 in_value) {
    if (!handle)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = static_cast<IoMap *>(handle);
    return map->write8(offset, in_value);
}

uacpi_status uacpi_kernel_io_write16(uacpi_handle handle, uacpi_size offset,
                                     uacpi_u16 in_value) {
    if (!handle)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = static_cast<IoMap *>(handle);
    return map->write16(offset, in_value);
}

uacpi_status uacpi_kernel_io_write32(uacpi_handle handle, uacpi_size offset,
                                     uacpi_u32 in_value) {
    if (!handle)
        return UACPI_STATUS_INVALID_ARGUMENT;
    auto map = static_cast<IoMap *>(handle);
    return map->write32(offset, in_value);
}


void *uacpi_kernel_alloc(uacpi_size size) { return malloc(size); }

#ifdef UACPI_NATIVE_ALLOC_ZEROED
void *uacpi_kernel_alloc_zeroed(uacpi_size size) {
    UNIMPLEMENTED;
    (void) size;
    return NULL;
}
#endif // UACPI_NATIVE_ALLOC_ZEROED

#ifndef UACPI_SIZED_FREES
void uacpi_kernel_free(void *mem) {
    if (mem)
        free(mem);
}
#else
void uacpi_kernel_free(void *mem, uacpi_size size_hint) {
    UNIMPLEMENTED;
    (void) mem;
    (void) size_hint;
}
#endif // UACPI_SIZED_FREES

uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
    UNIMPLEMENTED;
    return 0;
}

void uacpi_kernel_stall(uacpi_u8 usec) {
    UNIMPLEMENTED;
    (void) usec;
}

void uacpi_kernel_sleep(uacpi_u64 msec) {
    UNIMPLEMENTED;
    (void) msec;
}

uacpi_handle uacpi_kernel_create_mutex(void) {
    auto spinlock = new Spinlock;
    return spinlock;
}

void uacpi_kernel_free_mutex(uacpi_handle handle) {
    UNIMPLEMENTED;
    (void) handle;
}

uacpi_handle uacpi_kernel_create_event(void) {
    UNIMPLEMENTED;
    return NULL;
}

void uacpi_kernel_free_event(uacpi_handle handle) {
    UNIMPLEMENTED;
    (void) handle;
}

uacpi_thread_id uacpi_kernel_get_thread_id(void) {
    return UACPI_THREAD_ID_NONE;
}

uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle handle,
                                        uacpi_u16 timeout) {
    (void) timeout;

    auto spinlock = reinterpret_cast<Spinlock *>(handle);
    spinlock->lock();
    return UACPI_STATUS_OK;
}

void uacpi_kernel_release_mutex(uacpi_handle handle) {
    auto spinlock = reinterpret_cast<Spinlock *>(handle);
    spinlock->unlock();
}

uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle handle, uacpi_u16 timeout) {
    UNIMPLEMENTED;
    (void) handle;
    (void) timeout;
    return UACPI_FALSE;
}

void uacpi_kernel_signal_event(uacpi_handle handle) {
    UNIMPLEMENTED;
    (void) handle;
}

void uacpi_kernel_reset_event(uacpi_handle handle) {
    UNIMPLEMENTED;
    (void) handle;
}

uacpi_status
uacpi_kernel_handle_firmware_request(uacpi_firmware_request *request) {
    UNIMPLEMENTED;
    (void) request;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_install_interrupt_handler(
        uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx,
        uacpi_handle *out_irq_handle) {
    UNIMPLEMENTED;
    (void) irq;
    (void) handler;
    (void) ctx;
    if (out_irq_handle)
        *out_irq_handle = NULL;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status
uacpi_kernel_uninstall_interrupt_handler(uacpi_interrupt_handler handler,
                                         uacpi_handle irq_handle) {
    UNIMPLEMENTED;
    (void) handler;
    (void) irq_handle;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_handle uacpi_kernel_create_spinlock(void) {
    auto spinlock = new Spinlock;
    return spinlock;
}

void uacpi_kernel_free_spinlock(uacpi_handle handle) {
    UNIMPLEMENTED;
    (void) handle;
}

uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle handle) {
    auto spinlock = reinterpret_cast<Spinlock *>(handle);
    spinlock->lock();
    return 0;
}

void uacpi_kernel_unlock_spinlock(uacpi_handle handle, uacpi_cpu_flags flags) {
    (void) flags;
    auto spinlock = reinterpret_cast<Spinlock *>(handle);
    spinlock->unlock();
}

uacpi_status uacpi_kernel_schedule_work(uacpi_work_type type,
                                        uacpi_work_handler handler,
                                        uacpi_handle ctx) {
    UNIMPLEMENTED;
    (void) type;
    (void) handler;
    (void) ctx;
    return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_wait_for_work_completion(void) {
    UNIMPLEMENTED;
    return UACPI_STATUS_UNIMPLEMENTED;
}

#endif // !UACPI_BAREBONES_MODE

} // extern "C"
