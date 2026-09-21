#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/.build-slot-v2"

rm -rf "$BUILD"
cmake -S "$ROOT" -B "$BUILD"
cmake --build "$BUILD" --target slot_v2_stop_test slot_v2_special_test slot_v2_navigation_test slot_v2_assist_test slot_v2_bell_navigation_test slot_v2_progress_test slot_v2_state_modules_test slot_v2_reel_candidate_test slot_v2_machine_state_test slot_v2_at_lottery_test slot_v2_feature_lottery_test
"$BUILD/slot_v2_stop_test"
"$BUILD/slot_v2_special_test"
"$BUILD/slot_v2_navigation_test"
"$BUILD/slot_v2_assist_test"
"$BUILD/slot_v2_bell_navigation_test"
"$BUILD/slot_v2_progress_test"
"$BUILD/slot_v2_state_modules_test"
"$BUILD/slot_v2_reel_candidate_test"
"$BUILD/slot_v2_machine_state_test"
"$BUILD/slot_v2_at_lottery_test"
"$BUILD/slot_v2_feature_lottery_test"

echo "slot v2 native tests passed."
