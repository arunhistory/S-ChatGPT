#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/.build-slot-v2"

rm -rf "$BUILD"
cmake -S "$ROOT" -B "$BUILD"
cmake --build "$BUILD" --target slot_v2_stop_test slot_v2_special_test
"$BUILD/slot_v2_stop_test"
"$BUILD/slot_v2_special_test"

echo "slot v2 native tests passed."
