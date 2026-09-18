#!/usr/bin/env bash

set -euox pipefail

cd kernel/
mkdir -p build
cd build
cmake -DKERNEL_ARCHITECTURE=x86_64 ..
make -j"$(nproc)"
cd ../../ 