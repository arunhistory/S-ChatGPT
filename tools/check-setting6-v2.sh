#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${BUILD_DIR:-$ROOT/build-v2-setting6}"

# Explicit, local-only native CMake tests. Never start a GitHub Actions loop.
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release
targets=(
  slot_v2_setting_change_test
  slot_v2_normal_flow_test
  slot_v2_at_lottery_test
  slot_v2_at_cycle_test
  slot_v2_chain_zone_test
  slot_v2_setting6_rates_test
  slot_v2_runtime_chain_zone_test
  slot_v2_runtime_special_zone_test
  slot_v2_runtime_bonus_test
  slot_v2_runtime_normal_route_test
)
cmake --build "$BUILD" --target "${targets[@]}" --parallel 2
for target in "${targets[@]}"; do
    printf '\n=== %s ===\n' "$target"
    "$BUILD/$target"
done

echo "Native v2 tests completed. This is NOT a payout simulation or regulatory test."
