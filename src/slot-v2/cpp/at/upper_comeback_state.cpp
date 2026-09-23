#include "at/upper_comeback_state.hpp"

namespace slotv2::upper_comeback {

void start(State& state) {
    state.active = true;
    state.games_left = kWindowGames;
}

bool consumeGame(State& state) {
    if (!state.active || state.games_left == 0u) return false;
    --state.games_left;
    if (state.games_left == 0u) state.active = false;
    return state.active;
}

bool judge(Rng& rng) {
    return rng.oneIn(5u);
}

} // namespace slotv2::upper_comeback
