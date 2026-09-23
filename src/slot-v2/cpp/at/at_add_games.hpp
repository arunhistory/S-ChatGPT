#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"

namespace slotv2::at_add_games {

// Setting-6/Japan-target distribution.
// +10 40%, +20 30%, +30 18%, +50 8%, +100 3%, +200 1%.
// Mean: 24.4G.
uint16_t fromRoll(uint16_t roll_0_to_99);
uint16_t draw(Rng& rng);

} // namespace slotv2::at_add_games
