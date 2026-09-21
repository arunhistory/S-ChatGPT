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

bool Rng::oneIn(uint32_t denominator) {
    if (denominator == 0u) return false;
    if (denominator == 1u) return true;

    // 2^64空間をdenominatorで割り切れる範囲へrejectionして剰余偏りを除く。
    const uint64_t threshold =
        (static_cast<uint64_t>(0) - static_cast<uint64_t>(denominator))
        % static_cast<uint64_t>(denominator);

    uint64_t value = 0;
    do {
        value = next64();
    } while (value < threshold);

    return (value % static_cast<uint64_t>(denominator)) == 0u;
}

} // namespace slotv2
