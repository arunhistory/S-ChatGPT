#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CLANGXX="${CLANGXX:-clang++}"
SRC="$ROOT/src/slot-v2"

"$CLANGXX" --target=wasm32-unknown-unknown -std=c++20 -O3 \
  -nostdlib -ffreestanding -fno-exceptions -fno-rtti \
  -Wl,--no-entry -Wl,--export-memory \
  -Wl,--export=slot_v2_reset \
  -Wl,--export=slot_v2_lever \
  -Wl,--export=slot_v2_last_special \
  -Wl,--export=slot_v2_last_role \
  -Wl,--export=slot_v2_freeze_active \
  -Wl,--initial-memory=131072 -Wl,--max-memory=16777216 \
  -o "$ROOT/web/slot-v2.wasm" \
  "$SRC/shared/memory.cpp" \
  "$SRC/shared/rng.cpp" \
  "$SRC/special-lottery/index.cpp" \
  "$SRC/main-lottery/index.cpp" \
  "$SRC/lever/index.cpp" \
  "$SRC/reel-strip/index.cpp" \
  "$SRC/stop-candidate/index.cpp" \
  "$SRC/freeze/index.cpp" \
  "$SRC/wasm/index.cpp"

echo "Built web/slot-v2.wasm from Edge Function-style C++ programs."
