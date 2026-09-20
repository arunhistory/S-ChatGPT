#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CLANGXX="${CLANGXX:-clang++}"

"$CLANGXX" --target=wasm32-unknown-unknown -std=c++20 -O3 \
  -nostdlib -ffreestanding -fno-exceptions -fno-rtti \
  -Wl,--no-entry -Wl,--export-memory \
  -Wl,--export=slot_reset \
  -Wl,--export=slot_set_setting \
  -Wl,--export=slot_state_json \
  -Wl,--export=slot_spin_normal_json \
  -Wl,--export=slot_spin_at_json \
  -Wl,--export=slot_force_outcome_json \
  -Wl,--export=slot_apply_reel_payout_json \
  -Wl,--initial-memory=262144 -Wl,--max-memory=16777216 \
  -o "$ROOT/web/slot.wasm" "$ROOT/src/slot_web.cpp"

cp "$ROOT/tools/slot-loader.js" "$ROOT/web/slot.js"

# GitHub connector / static-host friendly text distribution for the WASM binary.
# The browser loader reads the manifest, joins all chunks, decodes Base64,
# and instantiates the exact same WebAssembly bytes.
rm -f "$ROOT"/web/slot.wasm.b64.*
BASE64_TMP="$ROOT/web/.slot.wasm.b64.tmp"
base64 "$ROOT/web/slot.wasm" | tr -d '\n' | fold -w 5000 > "$BASE64_TMP"
chunk_count=0
while IFS= read -r chunk || [ -n "$chunk" ]; do
  chunk_count=$((chunk_count + 1))
  printf '%s' "$chunk" > "$ROOT/web/slot.wasm.b64.$chunk_count"
done < "$BASE64_TMP"
printf '%s' "$chunk_count" > "$ROOT/web/slot.wasm.b64.manifest"
rm -f "$BASE64_TMP"

echo "Built web/slot.wasm + Base64 chunks + web/slot.js directly with clang; GitHub Actions/Emscripten are not used."
