#!/usr/bin/env bash

set -euox pipefail

cd Limine
make -j$(nproc)
cd ..
