#include "rng.hpp"
#include "types.hpp"

namespace slotv2 {

void Rng::reset(uint64_t seed) {
    state_ = seed ? seed : 0x9E3779B97F4A7C15ULL;
}

uint64_t Rng::next64() {
    uint64_t x = state_;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    state_ = x;
    return x * 2685821657736338717ULL;
}

uint32_t Rng::next27() {
    return static_cast<uint32_t>(next64() & (kRngSpace - 1u));
}

} // namespace slotv2
