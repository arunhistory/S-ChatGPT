#include "index.hpp"

namespace slotv2::revival_state {

void start(State& state, at_state::Tier ended_tier) {
    state.active = true;
    state.games_left = kGames;
    state.revive_tier = ended_tier;
    state.kicked_normal_hit = false;
}

void recordKickedNormalHit(State& state) {
    if (state.active) state.kicked_normal_hit = true;
}

void clear(State& state) {
    state = {};
}

} // namespace slotv2::revival_state
