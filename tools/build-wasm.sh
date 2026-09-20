#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CLANGXX="${CLANGXX:-clang++}"

"$CLANGXX" --target=wasm32-unknown-unknown -std=c++20 -O3 \
  -nostdlib -ffreestanding -fno-exceptions -fno-rtti \
  -Wl,--no-entry -Wl,--export-memory \
  -Wl,--export=slot_reset \
  -Wl,--export=slot_state_json \
  -Wl,--export=slot_spin_normal_json \
  -Wl,--export=slot_spin_at_json \
  -Wl,--export=slot_apply_reel_payout_json \
  -Wl,--initial-memory=262144 -Wl,--max-memory=16777216 \
  -o "$ROOT/web/slot.wasm" "$ROOT/src/slot_web.cpp"

cp "$ROOT/tools/slot-loader.js" "$ROOT/web/slot.js"

echo "Built web/slot.wasm + web/slot.js directly with clang; GitHub Actions/Emscripten are not used."
