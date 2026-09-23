#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "at/at_state.hpp"

namespace slotv2::at_cold {

bool fromRoll(uint8_t roll_0_to_9);
bool draw(Rng& rng);

// Cold lowers growth-event regions by 30% while leaving fall unchanged.
uint32_t scaleGrowthCount(
    uint32_t count,
    bool cold
);

void reroll(
    Rng& rng,
    at_state::State& state
);

} // namespace slotv2::at_cold
