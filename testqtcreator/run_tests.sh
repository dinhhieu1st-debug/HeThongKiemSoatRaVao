#!/usr/bin/env bash

set -euo pipefail

THU_MUC_TEST="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
THU_MUC_BUILD="$THU_MUC_TEST/build"

cmake \
    -S "$THU_MUC_TEST" \
    -B "$THU_MUC_BUILD" \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DQt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6

cmake --build "$THU_MUC_BUILD" --parallel

ctest \
    --test-dir "$THU_MUC_BUILD" \
    --output-on-failure
