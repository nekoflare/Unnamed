#!/usr/bin/env bash

set -euox pipefail

echo "Getting limine (yummy)"
curl -sL -o limine-binary.tar.gz https://github.com/limine-bootloader/limine/releases/download/v12.9.0/limine-binary.tar.gz
mkdir -p Limine
tar -xzf limine-binary.tar.gz -C Limine --strip-components=1
rm limine-binary.tar.gz

echo "Getting UEFI (yucky)"
git clone https://github.com/ilobilo/ovmf-binaries --depth=1