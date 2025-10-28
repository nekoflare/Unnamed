//
// Created by neko on 10/28/25.
//

#include "gdt.hpp"

// null, kernel code, kernel data, user code32, user data, user code64
static x86_64::GdtSegmentDescriptor segments[6] {};

void x86_64::init_gdt() {
    segments[0] = create_descriptor(0, 0, static_cast<Access>(0), static_cast<Flags>(0)); // NULL
    segments[1] = create_descriptor(0, 0, Access::Present | Access::Dpl0 | Access::DescriptorType | Access::ReadWrite | Access::Executable, Flags::LongMode | Flags::Granularity); // Kernel code
    segments[2] = create_descriptor(0, 0, Access::Present | Access::Dpl0 | Access::DescriptorType | Access::ReadWrite, Flags::Granularity | Flags::Size); // User data
    segments[3] = create_descriptor(0, 0, static_cast<Access>(0), static_cast<Flags>(0)); // User code32
    segments[4] = create_descriptor(0, 0, Access::Present | Access::Dpl3 | Access::DescriptorType | Access::ReadWrite, Flags::Granularity | Flags::Size); // Usert data
    segments[5] = create_descriptor(0, 0, Access::Present | Access::Dpl3 | Access::DescriptorType | Access::ReadWrite | Access::Executable, Flags::LongMode | Flags::Granularity); // User code64
}