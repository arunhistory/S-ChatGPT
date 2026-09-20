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
