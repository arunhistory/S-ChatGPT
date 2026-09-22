cmake_minimum_required(VERSION 3.20)
project(SChatGPTSlot LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Native simulator/reference engine.
add_library(slot_engine src/slot_engine.cpp)
target_include_directories(slot_engine PUBLIC src)

add_executable(slot_sim src/sim_main.cpp)
target_link_libraries(slot_sim PRIVATE slot_engine)

# Browser WASM intentionally does NOT use GitHub Actions or Emscripten.
# Build it directly with: tools/build-wasm.sh


# Slot v2: Edge Function-style C++ programs.
# The old slot_engine stays intact until the v2 control path is complete.
add_library(slot_v2_core
  src/slot-v2/shared/rng.cpp
  src/slot-v2/special-lottery/index.cpp
  src/slot-v2/special-result/index.cpp
  src/slot-v2/special-apply/index.cpp
  src/slot-v2/main-lottery/index.cpp
  src/slot-v2/lever/index.cpp
  src/slot-v2/reel-strip/index.cpp
  src/slot-v2/reel-validator/index.cpp
  src/slot-v2/preflight/index.cpp
  src/slot-v2/reel-read/index.cpp
  src/slot-v2/reel-candidate/index.cpp
  src/slot-v2/stop-candidate/index.cpp
  src/slot-v2/stop-rules/common.cpp
  src/slot-v2/stop-rules/bell.cpp
  src/slot-v2/stop-rules/replay.cpp
  src/slot-v2/stop-rules/cherry.cpp
  src/slot-v2/stop-rules/watermelon.cpp
  src/slot-v2/stop-rules/fallback.cpp
  src/slot-v2/stop-rules/router.cpp
  src/slot-v2/stop-rules/penguin.cpp
  src/slot-v2/stop-rules/strong-chance.cpp
  src/slot-v2/stop-rules/weak-chance.cpp
  src/slot-v2/stop-rules/one-medal.cpp
  src/slot-v2/stop-rules/miss.cpp
  src/slot-v2/role-policy/index.cpp
  src/slot-v2/navigation/index.cpp
  src/slot-v2/bell-navigation/index.cpp
  src/slot-v2/assist-integrity/index.cpp
  src/slot-v2/assist-target/index.cpp
  src/slot-v2/stop-first/index.cpp
  src/slot-v2/stop-second/index.cpp
  src/slot-v2/stop-third/index.cpp
  src/slot-v2/stop-controller/index.cpp
  src/slot-v2/freeze/index.cpp
  src/slot-v2/session/index.cpp
  src/slot-v2/line/index.cpp
  src/slot-v2/acquisition/index.cpp
  src/slot-v2/normal-progress/index.cpp
  src/slot-v2/normal-at-trigger/index.cpp
  src/slot-v2/normal-hit-entry/index.cpp
  src/slot-v2/cz-cycle/index.cpp
  src/slot-v2/cz-finalize/index.cpp
  src/slot-v2/cz-reward/index.cpp
  src/slot-v2/normal-state/index.cpp
  src/slot-v2/normal-progress-view/index.cpp
  src/slot-v2/ceiling-catalog/index.cpp
  src/slot-v2/game-finalize/index.cpp
  src/slot-v2/pending-event/index.cpp
  src/slot-v2/runtime/index.cpp
  src/slot-v2/point-ledger/index.cpp
  src/slot-v2/accounting/index.cpp
  src/slot-v2/machine-state/index.cpp
  src/slot-v2/cz-state/index.cpp
  src/slot-v2/bonus-state/index.cpp
  src/slot-v2/bonus-cycle/index.cpp
  src/slot-v2/bonus-transition/index.cpp
  src/slot-v2/bonus-entry-policy/index.cpp
  src/slot-v2/at-state/index.cpp
  src/slot-v2/at-lottery/index.cpp
  src/slot-v2/progress-event/index.cpp
  src/slot-v2/at-cycle/index.cpp
  src/slot-v2/cz-lottery/index.cpp
  src/slot-v2/special-ceiling/index.cpp
  src/slot-v2/normal-mode/index.cpp
  src/slot-v2/at-resolution/index.cpp
  src/slot-v2/at-single-transition/index.cpp
  src/slot-v2/section-flow/index.cpp
  src/slot-v2/at-window/index.cpp
  src/slot-v2/section-transition/index.cpp
  src/slot-v2/at-pending/index.cpp
  src/slot-v2/at-event/index.cpp
  src/slot-v2/special-zone/index.cpp
  src/slot-v2/special-zone-pending/index.cpp
  src/slot-v2/upper-comeback/index.cpp
  src/slot-v2/upper-comeback-cycle/index.cpp
  src/slot-v2/revival-cycle/index.cpp
  src/slot-v2/revival-lottery/index.cpp
  src/slot-v2/revival-state/index.cpp
  src/slot-v2/bonus-upgrade/index.cpp
  src/slot-v2/stock-lottery/index.cpp
  src/slot-v2/stock-count-lottery/index.cpp
  src/slot-v2/stock/index.cpp
  src/slot-v2/section/index.cpp
  src/slot-v2/section-reward/index.cpp
)
target_include_directories(slot_v2_core PUBLIC src/slot-v2)

add_executable(slot_v2_sim src/slot-v2/sim_main.cpp)
target_link_libraries(slot_v2_sim PRIVATE slot_v2_core)

# Browser v2 WASM remains a direct clang build to match the existing project:
#   tools/build-wasm-v2.sh


add_executable(slot_v2_stop_test src/slot-v2/stop_test.cpp)
target_link_libraries(slot_v2_stop_test PRIVATE slot_v2_core)


add_executable(slot_v2_special_test src/slot-v2/special_test.cpp)
target_link_libraries(slot_v2_special_test PRIVATE slot_v2_core)


add_executable(slot_v2_navigation_test src/slot-v2/navigation_test.cpp)
target_link_libraries(slot_v2_navigation_test PRIVATE slot_v2_core)


add_executable(slot_v2_assist_test src/slot-v2/assist_test.cpp)
target_link_libraries(slot_v2_assist_test PRIVATE slot_v2_core)


add_executable(slot_v2_bell_navigation_test src/slot-v2/bell_navigation_test.cpp)
target_link_libraries(slot_v2_bell_navigation_test PRIVATE slot_v2_core)


add_executable(slot_v2_progress_test src/slot-v2/progress_test.cpp)
target_link_libraries(slot_v2_progress_test PRIVATE slot_v2_core)


add_executable(slot_v2_state_modules_test src/slot-v2/state_modules_test.cpp)
target_link_libraries(slot_v2_state_modules_test PRIVATE slot_v2_core)


add_executable(slot_v2_reel_candidate_test src/slot-v2/reel_candidate_test.cpp)
target_link_libraries(slot_v2_reel_candidate_test PRIVATE slot_v2_core)


add_executable(slot_v2_machine_state_test src/slot-v2/machine_state_test.cpp)
target_link_libraries(slot_v2_machine_state_test PRIVATE slot_v2_core)


add_executable(slot_v2_at_lottery_test src/slot-v2/at_lottery_test.cpp)
target_link_libraries(slot_v2_at_lottery_test PRIVATE slot_v2_core)


add_executable(slot_v2_feature_lottery_test src/slot-v2/feature_lottery_test.cpp)
target_link_libraries(slot_v2_feature_lottery_test PRIVATE slot_v2_core)


add_executable(slot_v2_special_apply_test src/slot-v2/special_apply_test.cpp)
target_link_libraries(slot_v2_special_apply_test PRIVATE slot_v2_core)


add_executable(slot_v2_stock_count_lottery_test src/slot-v2/stock_count_lottery_test.cpp)
target_link_libraries(slot_v2_stock_count_lottery_test PRIVATE slot_v2_core)


add_executable(slot_v2_preflight_test src/slot-v2/preflight_test.cpp)
target_link_libraries(slot_v2_preflight_test PRIVATE slot_v2_core)


add_executable(slot_v2_game_finalize_test src/slot-v2/game_finalize_test.cpp)
target_link_libraries(slot_v2_game_finalize_test PRIVATE slot_v2_core)


add_executable(slot_v2_at_cycle_test src/slot-v2/at_cycle_test.cpp)
target_link_libraries(slot_v2_at_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_progress_event_test src/slot-v2/progress_event_test.cpp)
target_link_libraries(slot_v2_progress_event_test PRIVATE slot_v2_core)


add_executable(slot_v2_flow_modules_test src/slot-v2/flow_modules_test.cpp)
target_link_libraries(slot_v2_flow_modules_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_progress_test src/slot-v2/runtime_progress_test.cpp)
target_link_libraries(slot_v2_runtime_progress_test PRIVATE slot_v2_core)


add_executable(slot_v2_catalog_test src/slot-v2/catalog_test.cpp)
target_link_libraries(slot_v2_catalog_test PRIVATE slot_v2_core)


add_executable(slot_v2_normal_cz_cycle_test src/slot-v2/normal_cz_cycle_test.cpp)
target_link_libraries(slot_v2_normal_cz_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_section_reward_test src/slot-v2/section_reward_test.cpp)
target_link_libraries(slot_v2_section_reward_test PRIVATE slot_v2_core)


add_executable(slot_v2_flow_pending_test src/slot-v2/flow_pending_test.cpp)
target_link_libraries(slot_v2_flow_pending_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_accounting_test src/slot-v2/runtime_accounting_test.cpp)
target_link_libraries(slot_v2_runtime_accounting_test PRIVATE slot_v2_core)


add_executable(slot_v2_cz_finalize_test src/slot-v2/cz_finalize_test.cpp)
target_link_libraries(slot_v2_cz_finalize_test PRIVATE slot_v2_core)

add_executable(slot_v2_cz_reward_test src/slot-v2/cz_reward_test.cpp)
target_link_libraries(slot_v2_cz_reward_test PRIVATE slot_v2_core)


add_executable(slot_v2_section_transition_test src/slot-v2/section_transition_test.cpp)
target_link_libraries(slot_v2_section_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_gate_test src/slot-v2/runtime_gate_test.cpp)
target_link_libraries(slot_v2_runtime_gate_test PRIVATE slot_v2_core)


add_executable(slot_v2_bonus_cycle_test src/slot-v2/bonus_cycle_test.cpp)
target_link_libraries(slot_v2_bonus_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_upper_comeback_cycle_test src/slot-v2/upper_comeback_cycle_test.cpp)
target_link_libraries(slot_v2_upper_comeback_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_bonus_test src/slot-v2/runtime_bonus_test.cpp)
target_link_libraries(slot_v2_runtime_bonus_test PRIVATE slot_v2_core)


add_executable(slot_v2_at_single_transition_test src/slot-v2/at_single_transition_test.cpp)
target_link_libraries(slot_v2_at_single_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_normal_at_trigger_test src/slot-v2/normal_at_trigger_test.cpp)
target_link_libraries(slot_v2_normal_at_trigger_test PRIVATE slot_v2_core)


add_executable(slot_v2_bonus_entry_policy_test src/slot-v2/bonus_entry_policy_test.cpp)
target_link_libraries(slot_v2_bonus_entry_policy_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_normal_scope_test src/slot-v2/runtime_normal_scope_test.cpp)
target_link_libraries(slot_v2_runtime_normal_scope_test PRIVATE slot_v2_core)


add_executable(slot_v2_special_zone_pending_test src/slot-v2/special_zone_pending_test.cpp)
target_link_libraries(slot_v2_special_zone_pending_test PRIVATE slot_v2_core)


add_executable(slot_v2_normal_hit_entry_test src/slot-v2/normal_hit_entry_test.cpp)
target_link_libraries(slot_v2_normal_hit_entry_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_normal_hit_test src/slot-v2/runtime_normal_hit_test.cpp)
target_link_libraries(slot_v2_runtime_normal_hit_test PRIVATE slot_v2_core)


add_executable(slot_v2_bonus_transition_test src/slot-v2/bonus_transition_test.cpp)
target_link_libraries(slot_v2_bonus_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_special_zone_test src/slot-v2/runtime_special_zone_test.cpp)
target_link_libraries(slot_v2_runtime_special_zone_test PRIVATE slot_v2_core)


add_executable(slot_v2_revival_test src/slot-v2/revival_test.cpp)
target_link_libraries(slot_v2_revival_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_revival_test src/slot-v2/runtime_revival_test.cpp)
target_link_libraries(slot_v2_runtime_revival_test PRIVATE slot_v2_core)
