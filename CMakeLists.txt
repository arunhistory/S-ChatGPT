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
  src/slot-v2/cpp/shared/rng.cpp
  src/slot-v2/cpp/special/special_lottery.cpp
  src/slot-v2/cpp/special/special_result.cpp
  src/slot-v2/cpp/special/special_apply.cpp
  src/slot-v2/cpp/core/main_lottery.cpp
  src/slot-v2/cpp/core/lever.cpp
  src/slot-v2/cpp/reel/reel_strip.cpp
  src/slot-v2/cpp/reel/reel_validator.cpp
  src/slot-v2/cpp/core/preflight.cpp
  src/slot-v2/cpp/reel/reel_read.cpp
  src/slot-v2/cpp/reel/stop/reel_candidate.cpp
  src/slot-v2/cpp/reel/stop/stop_candidate.cpp
  src/slot-v2/cpp/reel/stop/rules/common_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/bell_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/replay_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/cherry_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/watermelon_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/fallback_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/rule_router.cpp
  src/slot-v2/cpp/reel/stop/rules/penguin_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/strong_chance_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/weak_chance_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/one_medal_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/miss_rule.cpp
  src/slot-v2/cpp/reel/stop/rules/entry_rule.cpp
  src/slot-v2/cpp/reel/stop/role_policy.cpp
  src/slot-v2/cpp/reel/stop/navigation.cpp
  src/slot-v2/cpp/reel/stop/bell_navigation.cpp
  src/slot-v2/cpp/reel/stop/assist_integrity.cpp
  src/slot-v2/cpp/reel/stop/assist_target.cpp
  src/slot-v2/cpp/reel/stop/stop_first.cpp
  src/slot-v2/cpp/reel/stop/stop_second.cpp
  src/slot-v2/cpp/reel/stop/stop_third.cpp
  src/slot-v2/cpp/reel/stop/stop_controller.cpp
  src/slot-v2/cpp/special/freeze_control.cpp
  src/slot-v2/cpp/core/session.cpp
  src/slot-v2/cpp/reel/reel_line.cpp
  src/slot-v2/cpp/reel/reel_acquisition.cpp
  src/slot-v2/cpp/normal/normal_progress.cpp
  src/slot-v2/cpp/normal/normal_at_trigger.cpp
  src/slot-v2/cpp/normal/normal_hit_entry.cpp
  src/slot-v2/cpp/entry/entry_gate.cpp
  src/slot-v2/cpp/entry/entry_gate_transition.cpp
  src/slot-v2/cpp/cz/cz_cycle.cpp
  src/slot-v2/cpp/cz/cz_finalize.cpp
  src/slot-v2/cpp/cz/cz_reward.cpp
  src/slot-v2/cpp/normal/normal_state.cpp
  src/slot-v2/cpp/normal/normal_progress_view.cpp
  src/slot-v2/cpp/normal/ceiling_catalog.cpp
  src/slot-v2/cpp/core/game_finalize.cpp
  src/slot-v2/cpp/core/pending_event.cpp
  src/slot-v2/cpp/core/runtime.cpp
  src/slot-v2/cpp/core/point_ledger.cpp
  src/slot-v2/cpp/core/accounting.cpp
  src/slot-v2/cpp/core/machine_state.cpp
  src/slot-v2/cpp/cz/cz_state.cpp
  src/slot-v2/cpp/bonus/bonus_state.cpp
  src/slot-v2/cpp/bonus/bonus_cycle.cpp
  src/slot-v2/cpp/bonus/bonus_transition.cpp
  src/slot-v2/cpp/bonus/bonus_entry_policy.cpp
  src/slot-v2/cpp/at/at_state.cpp
  src/slot-v2/cpp/at/at_cold.cpp
  src/slot-v2/cpp/at/at_lottery.cpp
  src/slot-v2/cpp/core/progress_event.cpp
  src/slot-v2/cpp/at/at_cycle.cpp
  src/slot-v2/cpp/cz/cz_lottery.cpp
  src/slot-v2/cpp/special/special_ceiling.cpp
  src/slot-v2/cpp/normal/normal_mode.cpp
  src/slot-v2/cpp/normal/normal_route.cpp
  src/slot-v2/cpp/normal/normal_ceiling.cpp
  src/slot-v2/cpp/normal/normal_ceiling_transition.cpp
  src/slot-v2/cpp/normal/normal_cycle_reset.cpp
  src/slot-v2/cpp/normal/normal_role_trigger.cpp
  src/slot-v2/cpp/core/setting_profile.cpp
  src/slot-v2/cpp/normal/normal_shortening.cpp
  src/slot-v2/cpp/normal/normal_high.cpp
  src/slot-v2/cpp/normal/normal_raw_lottery.cpp
  src/slot-v2/cpp/normal/normal_flow.cpp
  src/slot-v2/cpp/normal/normal_flow_transition.cpp
  src/slot-v2/cpp/at/at_resolution.cpp
  src/slot-v2/cpp/at/at_single_transition.cpp
  src/slot-v2/cpp/section/section_flow.cpp
  src/slot-v2/cpp/at/at_window.cpp
  src/slot-v2/cpp/at/at_table_transition.cpp
  src/slot-v2/cpp/at/at_stock_restart.cpp
  src/slot-v2/cpp/section/section_transition.cpp
  src/slot-v2/cpp/at/at_pending.cpp
  src/slot-v2/cpp/at/at_add_games.cpp
  src/slot-v2/cpp/at/at_internal_transition.cpp
  src/slot-v2/cpp/at/at_window_transition.cpp
  src/slot-v2/cpp/at/at_event.cpp
  src/slot-v2/cpp/special/special_zone.cpp
  src/slot-v2/cpp/special/special_zone_pending.cpp
  src/slot-v2/cpp/special/special_zone_transition.cpp
  src/slot-v2/cpp/special/upper_special.cpp
  src/slot-v2/cpp/special/upper_special_transition.cpp
  src/slot-v2/cpp/at/upper_comeback_state.cpp
  src/slot-v2/cpp/at/upper_comeback_cycle.cpp
  src/slot-v2/cpp/at/upper_comeback_transition.cpp
  src/slot-v2/cpp/revival/revival_cycle.cpp
  src/slot-v2/cpp/revival/revival_lottery.cpp
  src/slot-v2/cpp/revival/revival_state.cpp
  src/slot-v2/cpp/bonus/bonus_upgrade.cpp
  src/slot-v2/cpp/stock/stock_lottery.cpp
  src/slot-v2/cpp/stock/stock_count_lottery.cpp
  src/slot-v2/cpp/stock/stock_state.cpp
  src/slot-v2/cpp/section/section_state.cpp
  src/slot-v2/cpp/section/section_reward.cpp
)
target_include_directories(slot_v2_core PUBLIC src/slot-v2/cpp)

add_executable(slot_v2_sim src/slot-v2/cpp/app/simulator_main.cpp)
target_link_libraries(slot_v2_sim PRIVATE slot_v2_core)

# Browser v2 WASM remains a direct clang build to match the existing project:
#   tools/build-wasm-v2.sh


add_executable(slot_v2_stop_test src/slot-v2/cpp/tests/reel/stop_test.cpp)
target_link_libraries(slot_v2_stop_test PRIVATE slot_v2_core)


add_executable(slot_v2_special_test src/slot-v2/cpp/tests/special/special_test.cpp)
target_link_libraries(slot_v2_special_test PRIVATE slot_v2_core)


add_executable(slot_v2_navigation_test src/slot-v2/cpp/tests/reel/navigation_test.cpp)
target_link_libraries(slot_v2_navigation_test PRIVATE slot_v2_core)


add_executable(slot_v2_assist_test src/slot-v2/cpp/tests/reel/assist_test.cpp)
target_link_libraries(slot_v2_assist_test PRIVATE slot_v2_core)


add_executable(slot_v2_bell_navigation_test src/slot-v2/cpp/tests/reel/bell_navigation_test.cpp)
target_link_libraries(slot_v2_bell_navigation_test PRIVATE slot_v2_core)


add_executable(slot_v2_progress_test src/slot-v2/cpp/tests/core/progress_test.cpp)
target_link_libraries(slot_v2_progress_test PRIVATE slot_v2_core)


add_executable(slot_v2_state_modules_test src/slot-v2/cpp/tests/integration/state_modules_test.cpp)
target_link_libraries(slot_v2_state_modules_test PRIVATE slot_v2_core)


add_executable(slot_v2_reel_candidate_test src/slot-v2/cpp/tests/reel/reel_candidate_test.cpp)
target_link_libraries(slot_v2_reel_candidate_test PRIVATE slot_v2_core)


add_executable(slot_v2_machine_state_test src/slot-v2/cpp/tests/core/machine_state_test.cpp)
target_link_libraries(slot_v2_machine_state_test PRIVATE slot_v2_core)


add_executable(slot_v2_at_lottery_test src/slot-v2/cpp/tests/at/at_lottery_test.cpp)
target_link_libraries(slot_v2_at_lottery_test PRIVATE slot_v2_core)

add_executable(slot_v2_at_cold_test src/slot-v2/cpp/tests/at/at_cold_test.cpp)
target_link_libraries(slot_v2_at_cold_test PRIVATE slot_v2_core)


add_executable(slot_v2_feature_lottery_test src/slot-v2/cpp/tests/integration/feature_lottery_test.cpp)
target_link_libraries(slot_v2_feature_lottery_test PRIVATE slot_v2_core)


add_executable(slot_v2_special_apply_test src/slot-v2/cpp/tests/special/special_apply_test.cpp)
target_link_libraries(slot_v2_special_apply_test PRIVATE slot_v2_core)


add_executable(slot_v2_stock_count_lottery_test src/slot-v2/cpp/tests/stock/stock_count_lottery_test.cpp)
target_link_libraries(slot_v2_stock_count_lottery_test PRIVATE slot_v2_core)


add_executable(slot_v2_preflight_test src/slot-v2/cpp/tests/core/preflight_test.cpp)
target_link_libraries(slot_v2_preflight_test PRIVATE slot_v2_core)


add_executable(slot_v2_game_finalize_test src/slot-v2/cpp/tests/core/game_finalize_test.cpp)
target_link_libraries(slot_v2_game_finalize_test PRIVATE slot_v2_core)


add_executable(slot_v2_at_cycle_test src/slot-v2/cpp/tests/at/at_cycle_test.cpp)
target_link_libraries(slot_v2_at_cycle_test PRIVATE slot_v2_core)

add_executable(slot_v2_at_transition_test src/slot-v2/cpp/tests/at/at_transition_test.cpp)
target_link_libraries(slot_v2_at_transition_test PRIVATE slot_v2_core)

add_executable(slot_v2_at_stock_restart_test src/slot-v2/cpp/tests/at/at_stock_restart_test.cpp)
target_link_libraries(slot_v2_at_stock_restart_test PRIVATE slot_v2_core)


add_executable(slot_v2_progress_event_test src/slot-v2/cpp/tests/core/progress_event_test.cpp)
target_link_libraries(slot_v2_progress_event_test PRIVATE slot_v2_core)


add_executable(slot_v2_flow_modules_test src/slot-v2/cpp/tests/integration/flow_modules_test.cpp)
target_link_libraries(slot_v2_flow_modules_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_progress_test src/slot-v2/cpp/tests/integration/runtime_progress_test.cpp)
target_link_libraries(slot_v2_runtime_progress_test PRIVATE slot_v2_core)


add_executable(slot_v2_catalog_test src/slot-v2/cpp/tests/normal/catalog_test.cpp)
target_link_libraries(slot_v2_catalog_test PRIVATE slot_v2_core)

add_executable(slot_v2_normal_route_test src/slot-v2/cpp/tests/normal/normal_route_test.cpp)
target_link_libraries(slot_v2_normal_route_test PRIVATE slot_v2_core)

add_executable(slot_v2_normal_ceiling_transition_test src/slot-v2/cpp/tests/normal/normal_ceiling_transition_test.cpp)
target_link_libraries(slot_v2_normal_ceiling_transition_test PRIVATE slot_v2_core)

add_executable(slot_v2_normal_cycle_reset_test src/slot-v2/cpp/tests/normal/normal_cycle_reset_test.cpp)
target_link_libraries(slot_v2_normal_cycle_reset_test PRIVATE slot_v2_core)

add_executable(slot_v2_normal_role_trigger_test src/slot-v2/cpp/tests/normal/normal_role_trigger_test.cpp)
target_link_libraries(slot_v2_normal_role_trigger_test PRIVATE slot_v2_core)

add_executable(slot_v2_normal_flow_test src/slot-v2/cpp/tests/normal/normal_flow_test.cpp)
target_link_libraries(slot_v2_normal_flow_test PRIVATE slot_v2_core)

add_executable(slot_v2_runtime_normal_route_test src/slot-v2/cpp/tests/integration/runtime_normal_route_test.cpp)
target_link_libraries(slot_v2_runtime_normal_route_test PRIVATE slot_v2_core)


add_executable(slot_v2_normal_cz_cycle_test src/slot-v2/cpp/tests/normal/normal_cz_cycle_test.cpp)
target_link_libraries(slot_v2_normal_cz_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_section_reward_test src/slot-v2/cpp/tests/section/section_reward_test.cpp)
target_link_libraries(slot_v2_section_reward_test PRIVATE slot_v2_core)


add_executable(slot_v2_flow_pending_test src/slot-v2/cpp/tests/integration/flow_pending_test.cpp)
target_link_libraries(slot_v2_flow_pending_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_accounting_test src/slot-v2/cpp/tests/integration/runtime_accounting_test.cpp)
target_link_libraries(slot_v2_runtime_accounting_test PRIVATE slot_v2_core)


add_executable(slot_v2_cz_finalize_test src/slot-v2/cpp/tests/cz/cz_finalize_test.cpp)
target_link_libraries(slot_v2_cz_finalize_test PRIVATE slot_v2_core)

add_executable(slot_v2_cz_reward_test src/slot-v2/cpp/tests/cz/cz_reward_test.cpp)
target_link_libraries(slot_v2_cz_reward_test PRIVATE slot_v2_core)


add_executable(slot_v2_section_transition_test src/slot-v2/cpp/tests/section/section_transition_test.cpp)
target_link_libraries(slot_v2_section_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_gate_test src/slot-v2/cpp/tests/integration/runtime_gate_test.cpp)
target_link_libraries(slot_v2_runtime_gate_test PRIVATE slot_v2_core)


add_executable(slot_v2_bonus_cycle_test src/slot-v2/cpp/tests/bonus/bonus_cycle_test.cpp)
target_link_libraries(slot_v2_bonus_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_upper_comeback_cycle_test src/slot-v2/cpp/tests/at/upper_comeback_cycle_test.cpp)
target_link_libraries(slot_v2_upper_comeback_cycle_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_bonus_test src/slot-v2/cpp/tests/integration/runtime_bonus_test.cpp)
target_link_libraries(slot_v2_runtime_bonus_test PRIVATE slot_v2_core)


add_executable(slot_v2_at_single_transition_test src/slot-v2/cpp/tests/at/at_single_transition_test.cpp)
target_link_libraries(slot_v2_at_single_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_normal_at_trigger_test src/slot-v2/cpp/tests/normal/normal_at_trigger_test.cpp)
target_link_libraries(slot_v2_normal_at_trigger_test PRIVATE slot_v2_core)


add_executable(slot_v2_bonus_entry_policy_test src/slot-v2/cpp/tests/bonus/bonus_entry_policy_test.cpp)
target_link_libraries(slot_v2_bonus_entry_policy_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_normal_scope_test src/slot-v2/cpp/tests/integration/runtime_normal_scope_test.cpp)
target_link_libraries(slot_v2_runtime_normal_scope_test PRIVATE slot_v2_core)


add_executable(slot_v2_special_zone_pending_test src/slot-v2/cpp/tests/special/special_zone_pending_test.cpp)
target_link_libraries(slot_v2_special_zone_pending_test PRIVATE slot_v2_core)

add_executable(slot_v2_special_zone_transition_test src/slot-v2/cpp/tests/special/special_zone_transition_test.cpp)
target_link_libraries(slot_v2_special_zone_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_normal_hit_entry_test src/slot-v2/cpp/tests/normal/normal_hit_entry_test.cpp)
target_link_libraries(slot_v2_normal_hit_entry_test PRIVATE slot_v2_core)

add_executable(slot_v2_entry_gate_test src/slot-v2/cpp/tests/entry/entry_gate_test.cpp)
target_link_libraries(slot_v2_entry_gate_test PRIVATE slot_v2_core)

add_executable(slot_v2_runtime_entry_gate_test src/slot-v2/cpp/tests/integration/runtime_entry_gate_test.cpp)
target_link_libraries(slot_v2_runtime_entry_gate_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_normal_hit_test src/slot-v2/cpp/tests/integration/runtime_normal_hit_test.cpp)
target_link_libraries(slot_v2_runtime_normal_hit_test PRIVATE slot_v2_core)


add_executable(slot_v2_bonus_transition_test src/slot-v2/cpp/tests/bonus/bonus_transition_test.cpp)
target_link_libraries(slot_v2_bonus_transition_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_special_zone_test src/slot-v2/cpp/tests/integration/runtime_special_zone_test.cpp)
target_link_libraries(slot_v2_runtime_special_zone_test PRIVATE slot_v2_core)

add_executable(slot_v2_upper_special_test src/slot-v2/cpp/tests/special/upper_special_test.cpp)
target_link_libraries(slot_v2_upper_special_test PRIVATE slot_v2_core)


add_executable(slot_v2_revival_test src/slot-v2/cpp/tests/revival/revival_test.cpp)
target_link_libraries(slot_v2_revival_test PRIVATE slot_v2_core)


add_executable(slot_v2_runtime_revival_test src/slot-v2/cpp/tests/integration/runtime_revival_test.cpp)
target_link_libraries(slot_v2_runtime_revival_test PRIVATE slot_v2_core)
