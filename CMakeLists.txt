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
  src/slot-v2/main-lottery/index.cpp
  src/slot-v2/lever/index.cpp
  src/slot-v2/reel-strip/index.cpp
  src/slot-v2/reel-validator/index.cpp
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
  src/slot-v2/runtime/index.cpp
  src/slot-v2/cz-state/index.cpp
  src/slot-v2/bonus-state/index.cpp
  src/slot-v2/at-state/index.cpp
  src/slot-v2/stock/index.cpp
  src/slot-v2/section/index.cpp
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
