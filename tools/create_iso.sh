#!/usr/bin/env bash

set -euox pipefail

ISO_DIR="iso"
KERNEL_FOLDER="../kernel"
PROJECT_ROOT=".."
REQUIRED_SUFFIX="/tools"
ISO_NAME="Unnamed.iso"

if [[ "$PWD" != *${REQUIRED_SUFFIX} ]]; then
    echo "ERROR: Incorrect current working directory. Expected to run from a directory ending in ${REQUIRED_SUFFIX}. Current: $PWD" >&2
    exit 1
fi

mkdir -p "${ISO_DIR}/boot/limine"
mkdir -p "${ISO_DIR}/EFI/BOOT"

cp -v "./Limine/limine-bios.sys" "${ISO_DIR}/boot/limine"
cp -v "./Limine/limine-bios-cd.bin" "${ISO_DIR}/boot/limine"
cp -v "./Limine/limine-uefi-cd.bin" "${ISO_DIR}/boot/limine"
cp -v "./Limine/BOOTX64.EFI" "${ISO_DIR}/EFI/BOOT"
cp -v "./Limine/BOOTIA32.EFI" "${ISO_DIR}/EFI/BOOT"
cp -v "${KERNEL_FOLDER}/build/kernel" "${ISO_DIR}/kernel"
cp -v "${PROJECT_ROOT}/limine.conf" "${ISO_DIR}/limine.conf"

xorriso -as mkisofs \
  -b boot/limine/limine-bios-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  --efi-boot boot/limine/limine-uefi-cd.bin \
  -efi-boot-part --efi-boot-image --protective-msdos-label \
  -r -J -joliet-long -V "Unnamed" "${ISO_DIR}" -o "${ISO_NAME}"

./Limine/limine bios-install "${ISO_NAME}"
rm -rf "${ISO_DIR}"
