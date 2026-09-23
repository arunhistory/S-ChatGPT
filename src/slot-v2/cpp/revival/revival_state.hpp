#pragma once
#include <stdint.h>
#include "at/at_state.hpp"

namespace slotv2::revival_state {

static constexpr uint8_t kGames = 5;

struct State {
    bool active{false};
    uint8_t games_left{0};
    at_state::Tier revive_tier{at_state::Tier::Lower};
    bool kicked_normal_hit{false};
};

void start(State& state, at_state::Tier ended_tier);
void recordKickedNormalHit(State& state);
void clear(State& state);

} // namespace slotv2::revival_state
