#!/usr/bin/env bash
set -euo pipefail

if command -v clang-format-19 &>/dev/null; then
    FMT=clang-format-19
else
    echo "warning: clang-format-19 not found, using $(clang-format --version); CI uses v19" >&2
    FMT=clang-format
fi

find src \( -name '*.cpp' -o -name '*.h' \) -print0 | xargs -0 "$FMT" -i
