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
  -Wl,--export=slot_v2_bonus_cycle \
  -Wl,--export=slot_v2_bonus_state \
  -Wl,--export=slot_v2_upper_comeback \
  -Wl,--export=slot_v2_at_single_transition \
  -Wl,--export=slot_v2_section_reward \
  -Wl,--export=slot_v2_at_window \
  -Wl,--export=slot_v2_section_transition \
  -Wl,--export=slot_v2_special_zone \
  -Wl,--export=slot_v2_cz_cycle \
  -Wl,--export=slot_v2_cz_finalize \
  -Wl,--export=slot_v2_normal_display_games \
  -Wl,--export=slot_v2_normal_actual_games \
  -Wl,--export=slot_v2_ceiling_at \
  -Wl,--export=slot_v2_ceiling_count \
  -Wl,--export=slot_v2_normal_progress \
  -Wl,--export=slot_v2_pending_events \
  -Wl,--export=slot_v2_at_cycle \
  -Wl,--export=slot_v2_normal_mode \
  -Wl,--export=slot_v2_at_resolution \
  -Wl,--export=slot_v2_bell_navigation_correct \
  -Wl,--export=slot_v2_bell_navigation_next \
  -Wl,--export=slot_v2_bell_navigation \
  -Wl,--export=slot_v2_point_count \
  -Wl,--export=slot_v2_stock_count \
  -Wl,--export=slot_v2_section_count \
  -Wl,--export=slot_v2_section_minimum \
  -Wl,--export=slot_v2_section_diff \
  -Wl,--export=slot_v2_machine_area \
  -Wl,--export=slot_v2_special_committed \
  -Wl,--export=slot_v2_at_games_left \
  -Wl,--export=slot_v2_at_tier \
  -Wl,--export=slot_v2_at_active \
  -Wl,--export=slot_v2_last_role \
  -Wl,--export=slot_v2_freeze_active \
  -Wl,--export=slot_v2_stop \
  -Wl,--export=slot_v2_stopped_position \
  -Wl,--export=slot_v2_stop_sequence \
  -Wl,--export=slot_v2_acquisition \
  -Wl,--export=slot_v2_validate_left \
  -Wl,--export=slot_v2_preflight \
  -Wl,--export=slot_v2_reel_ready_mask \
  -Wl,--export=slot_v2_validate_reel \
  -Wl,--export=slot_v2_assist_failure_mask \
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
  "$SRC/special-apply/index.cpp" \
  "$SRC/main-lottery/index.cpp" \
  "$SRC/lever/index.cpp" \
  "$SRC/reel-strip/index.cpp" \
  "$SRC/reel-validator/index.cpp" \
  "$SRC/preflight/index.cpp" \
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
  "$SRC/navigation/index.cpp" \
  "$SRC/bell-navigation/index.cpp" \
  "$SRC/assist-integrity/index.cpp" \
  "$SRC/assist-target/index.cpp" \
  "$SRC/stop-first/index.cpp" \
  "$SRC/stop-second/index.cpp" \
  "$SRC/stop-third/index.cpp" \
  "$SRC/stop-controller/index.cpp" \
  "$SRC/freeze/index.cpp" \
  "$SRC/acquisition/index.cpp" \
  "$SRC/normal-progress/index.cpp" \
  "$SRC/cz-cycle/index.cpp" \
  "$SRC/cz-finalize/index.cpp" \
  "$SRC/normal-state/index.cpp" \
  "$SRC/normal-progress-view/index.cpp" \
  "$SRC/ceiling-catalog/index.cpp" \
  "$SRC/game-finalize/index.cpp" \
  "$SRC/pending-event/index.cpp" \
  "$SRC/runtime/index.cpp" \
  "$SRC/point-ledger/index.cpp" \
  "$SRC/accounting/index.cpp" \
  "$SRC/machine-state/index.cpp" \
  "$SRC/cz-state/index.cpp" \
  "$SRC/bonus-state/index.cpp" \
  "$SRC/bonus-cycle/index.cpp" \
  "$SRC/at-state/index.cpp" \
  "$SRC/at-lottery/index.cpp" \
  "$SRC/progress-event/index.cpp" \
  "$SRC/at-cycle/index.cpp" \
  "$SRC/cz-lottery/index.cpp" \
  "$SRC/special-ceiling/index.cpp" \
  "$SRC/normal-mode/index.cpp" \
  "$SRC/at-resolution/index.cpp" \
  "$SRC/at-single-transition/index.cpp" \
  "$SRC/section-flow/index.cpp" \
  "$SRC/at-window/index.cpp" \
  "$SRC/section-transition/index.cpp" \
  "$SRC/at-pending/index.cpp" \
  "$SRC/at-event/index.cpp" \
  "$SRC/special-zone/index.cpp" \
  "$SRC/upper-comeback/index.cpp" \
  "$SRC/upper-comeback-cycle/index.cpp" \
  "$SRC/bonus-upgrade/index.cpp" \
  "$SRC/stock-lottery/index.cpp" \
  "$SRC/stock-count-lottery/index.cpp" \
  "$SRC/stock/index.cpp" \
  "$SRC/section/index.cpp" \
  "$SRC/section-reward/index.cpp" \
  "$SRC/line/index.cpp" \
  "$SRC/session/index.cpp" \
  "$SRC/wasm/index.cpp"

echo "Built web/slot-v2.wasm from Edge Function-style C++ programs."
