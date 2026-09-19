#include "slot_engine.hpp"

#include <cstdint>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

namespace {
schatgpt::SlotEngine engine;
std::string buffer;
const char* hold(std::string v) { buffer = std::move(v); return buffer.c_str(); }
}

extern "C" {
EXPORT void slot_reset(std::uint32_t seed_lo, std::uint32_t seed_hi) {
    const std::uint64_t seed = (static_cast<std::uint64_t>(seed_hi) << 32) | seed_lo;
    engine.reset(seed);
}
EXPORT const char* slot_state_json() { return hold(engine.stateJson()); }
EXPORT const char* slot_spin_normal_json() { return hold(engine.spinNormalJson()); }
EXPORT const char* slot_spin_at_json() { return hold(engine.spinATJson()); }
}
