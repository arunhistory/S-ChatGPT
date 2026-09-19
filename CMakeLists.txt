cmake_minimum_required(VERSION 3.20)
project(SChatGPTSlot LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(slot_engine src/slot_engine.cpp)
target_include_directories(slot_engine PUBLIC src)

add_executable(slot_sim src/sim_main.cpp)
target_link_libraries(slot_sim PRIVATE slot_engine)

if(EMSCRIPTEN)
  add_executable(slot_wasm src/wasm_api.cpp)
  target_link_libraries(slot_wasm PRIVATE slot_engine)
  set_target_properties(slot_wasm PROPERTIES OUTPUT_NAME "slot")
  target_link_options(slot_wasm PRIVATE
    "-sEXPORTED_FUNCTIONS=['_slot_reset','_slot_state_json','_slot_spin_normal_json','_slot_spin_at_json']"
    "-sEXPORTED_RUNTIME_METHODS=['UTF8ToString','ccall']"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=createSlotModule"
  )
endif()
