#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "at/at_state.hpp"

namespace slotv2::at_lottery {

struct Draw {
    bool hit{false};
    bool fall{false};
    bool add_games{false};
    bool special{false};
    bool episode{false};
    bool upper_special{false};
    bool chain_zone{false};
};

// Disjoint event regions after tier normalization and cold scaling.
struct Regions {
    uint32_t hit{0}, fall{0}, add_games{0}, special{0};
    uint32_t episode{0}, upper_special{0}, chain_zone{0};
    uint32_t eligibleTotal() const {
        return hit+add_games+special+episode+upper_special+chain_zone;
    }
};
Regions regionsFor(const at_state::State& state);

// One AT game consumes exactly one draw from one shared box.
// The configured event regions are disjoint, so at most one flag can be true.
Draw draw(Rng& rng, const at_state::State& state);

uint8_t count(const Draw& draw);

} // namespace slotv2::at_lottery
