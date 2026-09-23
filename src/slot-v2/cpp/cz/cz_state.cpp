#include "cz/cz_state.hpp"

namespace slotv2::cz_state {

void start(State& state) {
    state.active = true;
    state.games_left = kGames;
}

bool consumeGame(State& state) {
    if (!state.active || state.games_left == 0u) return false;
    --state.games_left;
    if (state.games_left == 0u) state.active = false;
    return state.active;
}

void resolve(State& state) {
    state.active = false;
    state.games_left = 0u;
}

} // namespace slotv2::cz_state
