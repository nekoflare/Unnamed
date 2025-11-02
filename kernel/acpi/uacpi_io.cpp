//
// Created by neko on 11/2/25.
//

#include "uacpi_io.hpp"

#include "arch/x86_64/io.hpp"

IoMap::IoMap(uacpi_io_addr base, uacpi_size len) : m_base(base), m_len(len) {}

std::optional<uint8_t> IoMap::read8(uacpi_size offset) const {
    if (offset >= m_len)
        return std::nullopt;
    return x86_64::inb(m_base + offset);
}

std::optional<uint16_t> IoMap::read16(uacpi_size offset) const {
    if (offset + 1 >= m_len)
        return std::nullopt;
    return x86_64::inw(m_base + offset);
}

std::optional<uint32_t> IoMap::read32(uacpi_size offset) const {
    if (offset + 3 >= m_len)
        return std::nullopt;
    return x86_64::inl(m_base + offset);
}

uacpi_status IoMap::write8(uacpi_size offset, uint8_t value) const {
    if (offset >= m_len)
        return UACPI_STATUS_INVALID_ARGUMENT;
    x86_64::outb(m_base + offset, value);
    return UACPI_STATUS_OK;
}

uacpi_status IoMap::write16(uacpi_size offset, uint16_t value) const {
    if (offset + 1 >= m_len)
        return UACPI_STATUS_INVALID_ARGUMENT;
    x86_64::outw(m_base + offset, value);
    return UACPI_STATUS_OK;
}

uacpi_status IoMap::write32(uacpi_size offset, uint32_t value) const {
    if (offset + 3 >= m_len)
        return UACPI_STATUS_INVALID_ARGUMENT;
    x86_64::outl(m_base + offset, value);
    return UACPI_STATUS_OK;
}
