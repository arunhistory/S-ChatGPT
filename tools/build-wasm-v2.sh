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
  -Wl,--export=slot_v2_stop \
  -Wl,--export=slot_v2_stopped_position \
  -Wl,--export=slot_v2_acquisition \
  -Wl,--export=slot_v2_validate_left \
  -Wl,--export=slot_v2_complete_special \
  -Wl,--export=slot_v2_special_result \
  -Wl,--export=slot_v2_phase \
  -Wl,--export=slot_v2_visible_symbol \
  -Wl,--export=slot_v2_symbol_at \
  -Wl,--initial-memory=131072 -Wl,--max-memory=16777216 \
  -o "$ROOT/web/slot-v2.wasm" \
  "$SRC/shared/memory.cpp" \
  "$SRC/shared/rng.cpp" \
  "$SRC/special-lottery/index.cpp" \
  "$SRC/special-result/index.cpp" \
  "$SRC/main-lottery/index.cpp" \
  "$SRC/lever/index.cpp" \
  "$SRC/reel-strip/index.cpp" \
  "$SRC/reel-validator/index.cpp" \
  "$SRC/reel-read/index.cpp" \
  "$SRC/stop-candidate/index.cpp" \
  "$SRC/stop-rules/common.cpp" \
  "$SRC/stop-rules/bell.cpp" \
  "$SRC/stop-rules/replay.cpp" \
  "$SRC/stop-rules/cherry.cpp" \
  "$SRC/stop-rules/watermelon.cpp" \
  "$SRC/stop-rules/fallback.cpp" \
  "$SRC/stop-rules/router.cpp" \
  "$SRC/stop-rules/penguin.cpp" \
  "$SRC/stop-rules/strong-chance.cpp" \
  "$SRC/stop-rules/weak-chance.cpp" \
  "$SRC/stop-rules/one-medal.cpp" \
  "$SRC/stop-rules/miss.cpp" \
  "$SRC/role-policy/index.cpp" \
  "$SRC/stop-first/index.cpp" \
  "$SRC/stop-second/index.cpp" \
  "$SRC/stop-third/index.cpp" \
  "$SRC/stop-controller/index.cpp" \
  "$SRC/freeze/index.cpp" \
  "$SRC/acquisition/index.cpp" \
  "$SRC/line/index.cpp" \
  "$SRC/session/index.cpp" \
  "$SRC/wasm/index.cpp"

echo "Built web/slot-v2.wasm from Edge Function-style C++ programs."
