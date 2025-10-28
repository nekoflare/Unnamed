#!/usr/bin/env bash

set -euox pipefail

echo "Getting limine (yummy)"
git clone https://codeberg.org/Limine/Limine.git --branch=v10.x-binary --depth=1

echo "Getting UEFI (yucky)"
git clone https://github.com/ilobilo/ovmf-binaries --depth=1