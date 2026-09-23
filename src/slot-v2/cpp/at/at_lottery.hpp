#pragma once
#include <stdint.h>
#include "../shared/rng.hpp"
#include "../at-state/index.hpp"

namespace slotv2::at_lottery {

struct Draw {
    bool hit{false};
    bool fall{false};
    bool add_games{false};
    bool special{false};
    bool episode{false};
    bool upper_special{false};
};

// One AT game consumes exactly one draw from one shared box.
// The configured event regions are disjoint, so at most one flag can be true.
Draw draw(Rng& rng, const at_state::State& state);

uint8_t count(const Draw& draw);

} // namespace slotv2::at_lottery
