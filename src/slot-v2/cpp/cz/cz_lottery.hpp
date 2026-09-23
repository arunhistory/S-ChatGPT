#pragma once
#include "shared/rng.hpp"
#include "shared/types.hpp"

namespace slotv2::cz_lottery {

// Per-role CZ breakthrough lottery.
// One-medal and miss do not participate.
uint16_t thresholdPerThousand(RoleFlag role);
bool fromRoll(RoleFlag role, uint16_t roll_0_to_999);
bool draw(Rng& rng, RoleFlag role);

} // namespace slotv2::cz_lottery
