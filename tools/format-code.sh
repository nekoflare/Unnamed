#!/usr/bin/env bash

set -euox pipefail

PATH_TO_KERNEL="../kernel"

find "$PATH_TO_KERNEL/" -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) \
    -not -path "$PATH_TO_KERNEL/lib/freestnd*/*" \
    -not -path "$PATH_TO_KERNEL/lib/nanoprintf/*" \
    -not -path "$PATH_TO_KERNEL/lib/dlmalloc/*" \
    -not -path "$PATH_TO_KERNEL/build/*" \
    -not -path "$PATH_TO_KERNEL/acpi/uacpi/*" \
    -exec sh -c 'echo "Formatting: $1"; clang-format -i "$1"' _ {} \;
