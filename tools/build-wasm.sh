#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CLANGXX="${CLANGXX:-clang++}"
OUT="$ROOT/.build-slot"
mkdir -p "$OUT"

"$CLANGXX" --target=wasm32-unknown-unknown -std=c++20 -O3 -nostdlib -ffreestanding -fno-exceptions -fno-rtti \
  -Wl,--no-entry -Wl,--export-memory \
  -Wl,--export=slot_reset \
  -Wl,--export=slot_state_json \
  -Wl,--export=slot_spin_normal_json \
  -Wl,--export=slot_spin_at_json \
  -Wl,--export=slot_apply_reel_payout_json \
  -Wl,--initial-memory=262144 -Wl,--max-memory=16777216 \
  -o "$OUT/slot.wasm" "$ROOT/src/slot_web.cpp"

base64 -w0 "$OUT/slot.wasm" > "$OUT/slot.b64"
split -b 9000 -d -a 1 "$OUT/slot.b64" "$OUT/slot."
mkdir -p "$ROOT/web/wasm"
rm -f "$ROOT/web/wasm/slot."*.b64
i=0
for part in "$OUT"/slot.[0-9]; do
  cp "$part" "$ROOT/web/wasm/slot.$i.b64"
  i=$((i+1))
done
printf '%s\n' "$i" > "$ROOT/web/wasm/count.txt"
echo "Built browser WASM into $i base64 chunks without Emscripten or GitHub Actions"
