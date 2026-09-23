#pragma once
#include <stdint.h>
#include "shared/rng.hpp"

namespace slotv2::upper_special {

struct State {
    bool active{false};
    uint32_t rounds{0};
    uint32_t total_added_games{0};
};

struct Step {
    bool active_before{false};
    bool success{false};
    bool ended{false};
    uint16_t added_games{0};
    uint32_t round{0};
};

void start(State& state);
uint16_t addGamesFromRoll(uint16_t roll_0_to_99);
Step playOne(State& state, Rng& rng);

} // namespace slotv2::upper_special
