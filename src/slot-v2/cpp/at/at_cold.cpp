#include "at/at_cold.hpp"

namespace slotv2::at_cold {

bool fromRoll(uint8_t roll_0_to_9) {
    return static_cast<uint8_t>(roll_0_to_9 % 10u) < 6u;
}

bool draw(Rng& rng) {
    return fromRoll(
        static_cast<uint8_t>(rng.uniformBelow(10u))
    );
}

uint32_t scaleGrowthCount(
    uint32_t count,
    bool cold
) {
    if (!cold) return count;
    return static_cast<uint32_t>(
        (static_cast<uint64_t>(count) * 7u) / 10u
    );
}

void reroll(
    Rng& rng,
    at_state::State& state
) {
    state.cold = draw(rng);
}

} // namespace slotv2::at_cold
