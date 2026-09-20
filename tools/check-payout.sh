#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CXX="${CXX:-c++}"
GAMES="${1:-5000000}"
OUT="${TMPDIR:-/tmp}/s-chatgpt-payout-sim"

"$CXX" -std=c++20 -O3 -I"$ROOT/src" \
  "$ROOT/src/slot_engine.cpp" "$ROOT/src/sim_main.cpp" \
  -o "$OUT"

for setting in 1 2 3 4 5 6 7; do
  echo "=== setting $setting ==="
  for seed in 0x1001 0x1002 0x1003 0x1004 0x1005; do
    "$OUT" "$GAMES" "$setting" "$seed" \
      | grep -E '^(setting|seed|target_payout_ratio|actual_games|net_diff|payout_ratio)='
    echo
  done
done
