//
// Created by neko on 11/2/25.
//

#ifndef KERNEL_UACPI_IO_HPP
#define KERNEL_UACPI_IO_HPP
#include "optional"
#include "uacpi/types.h"

class IoMap {
public:
    explicit IoMap(uacpi_io_addr base, uacpi_size len);

    [[nodiscard]] std::optional<uint8_t> read8(uacpi_size offset) const;
    [[nodiscard]] std::optional<uint16_t> read16(uacpi_size offset) const;
    [[nodiscard]] std::optional<uint32_t> read32(uacpi_size offset) const;

    [[nodiscard]] uacpi_status write8(uacpi_size offset, uint8_t value) const;
    [[nodiscard]] uacpi_status write16(uacpi_size offset, uint16_t value) const;
    [[nodiscard]] uacpi_status write32(uacpi_size offset, uint32_t value) const;

private:
    uacpi_io_addr m_base;
    uacpi_size m_len;
};

#endif // KERNEL_UACPI_IO_HPP
