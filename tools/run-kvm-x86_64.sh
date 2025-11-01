!/usr/bin/env bash

set -euox pipefail

ISO_PATH="./Unnamed.iso"

qemu-system-x86_64                                                                                \
                    -m 4G                                                                         \
                    -M q35,smm=off                                                                \
                    -smp cores=6,sockets=1                                                        \
                    -cpu host,+invtsc                                                             \
                    -enable-kvm                                                                   \
                    -d int                                                                        \
                    -no-reboot                                                                    \
                    -no-shutdown                                                                  \
                    -debugcon stdio                                                               \
                    -cdrom ${ISO_PATH}                                                            \
                    -drive if=pflash,unit=0,format=raw,file=ovmf-binaries/OVMF_X64.fd,readonly=on \
