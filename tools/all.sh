#!/usr/bin/env bash

set -euox pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

RUN_AFTER_BUILD=1
for arg in "$@"; do
    case "${arg}" in
        --no-run) RUN_AFTER_BUILD=0 ;;
        *)
            echo "Unknown option: ${arg}" >&2
            echo "Usage: $0 [--no-run]" >&2
            exit 1
            ;;
    esac
done

cd "${SCRIPT_DIR}"

if [[ ! -d Limine ]] || [[ ! -d ovmf-binaries ]]; then
    ./download-required.sh
fi

./compile-limine.sh

cd "${ROOT_DIR}"
./tools/compile-kernel.sh

cd "${SCRIPT_DIR}"
./create_iso.sh

if [[ "${RUN_AFTER_BUILD}" -eq 1 ]]; then
    ./run-kvm-x86_64.sh
fi
