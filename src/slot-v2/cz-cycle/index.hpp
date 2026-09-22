#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../shared/types.hpp"
#include "../cz-state/index.hpp"

namespace slotv2::cz_cycle {

struct Result {
    bool active{false};
    bool base_hit{false};
    uint8_t games_left{0};
    bool ended{false};
};

// Ten-game CZ. Breakthrough is determined only from the internally
// established role for the game.
Result playOne(Rng& rng, cz_state::State& state, RoleFlag role);

} // namespace slotv2::cz_cycle
