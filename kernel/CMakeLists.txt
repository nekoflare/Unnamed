#
# Options to pass to the CMake:
#  KERNEL_TUNE_FOR_NATIVE - Compile the kernel and use whatever your host machine got
#  KERNEL_USE_OPTIMIZATIONS - Compile the kernel and use whatever optimizes but not necessarily using SIMD
#  KERNEL_USE_DEBUG - Compile with debug symbols and don't omit any frame pointers.
#  KERNEL_USE_SANITIZERS - Compile with UBSAN and enable it.
#

cmake_minimum_required(VERSION 4.0.2)
project(kernel VERSION 1.0 LANGUAGES C CXX ASM)

if (NOT DEFINED KERNEL_ARCHITECTURE)
    message(FATAL_ERROR "KERNEL_ARCHITECTURE is undefined. please set it. Valid options: x86_64")
endif ()

if (NOT KERNEL_ARCHITECTURE STREQUAL "x86_64")
    message(FATAL_ERROR "KERNEL_ARCHITECTURE is unknown. Got: ${KERNEL_ARCHITECTURE}")
endif ()

enable_language(ASM_NASM)

set(CMAKE_C_STANDARD 23)
set(CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_NASM_COMPILER nasm)

# here cmake includes
if (KERNEL_ARCHITECTURE STREQUAL "x86_64")
    include(cmake/x86_64-flags.cmake)
endif ()

set(CMAKE_C_FLAGS "${SHARED_C_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -fno-exceptions -fno-rtti -fno-threadsafe-statics")
set(CMAKE_EXE_LINKER_FLAGS "-static -nostdlib -z text -T ${CMAKE_SOURCE_DIR}/${KERNEL_ARCHITECTURE}_linker.ld -z max-page-size=0x1000 -fno-lto -fno-PIE -fno-PIC -nostartfiles")
set(CMAKE_ASM_NASM_FLAGS "-f elf64 -g -F dwarf")

# here includes for the kernel
include_directories(lib/freestnd-c-hdrs/${KERNEL_ARCHITECTURE}/include)
include_directories(lib/freestnd-cxx-hdrs/${KERNEL_ARCHITECTURE}/include)
include_directories(lib/nanoprintf/)
include_directories(./) # include this whole place as a whole.
include_directories(${CMAKE_SOURCE_DIR}/../tools/Limine/) # whoops, kinda whacky

# here sources
set(KERNEL_SOURCE
        kernel/core.cpp
        kernel/logger.hpp
        kernel/logger.cpp
        mm/pma.cpp
        mm/pma.hpp
        kernel/assert.hpp
        arch/x86_64/cpu.hpp
        lib/ptr_array_wrapper.hpp
        mm/vmm.cpp
        mm/vmm.hpp
        lib/utils.hpp
        kernel/percpu.cpp
        kernel/percpu.hpp
        arch/x86_64/msr.hpp
        mm/vma.cpp
        mm/vma.hpp
        lib/spinlock.cpp
        lib/spinlock.hpp
        lib/memory.cpp
        kernel/panic.cpp
        mm/heap.cpp
        mm/heap.hpp
        lib/cxx-runtime/nothrow.cpp
        kernel/error.hpp
        kernel/helpers.hpp
        lib/memory.hpp
        kernel/bit_helpers.hpp

)

# add architecture specific flags

if (KERNEL_ARCHITECTURE STREQUAL "x86_64")
    list(APPEND KERNEL_SOURCE
            "boot/x86_64/entry.asm"
            "arch/x86_64/io.hpp"
            "arch/x86_64/idt.cpp"
            "arch/x86_64/idt.hpp"
            "arch/x86_64/gdt.cpp"
            "arch/x86_64/gdt.hpp"
            "arch/x86_64/gdt.asm"
            "arch/x86_64/idt.asm"
            "arch/x86_64/exception_handlers.asm"
            "arch/x86_64/registers.hpp")
else ()
    message(FATAL_ERROR "Unknown architecture.")
endif ()

add_executable(kernel
        ${KERNEL_SOURCE})

# here link libraries

