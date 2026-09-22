#include "index.hpp"

namespace slotv2::normal_state {

void reset(State& state) {
    state = {};
}

void onLever(State& state) {
    ++state.actual_games;
    ++state.display_games;
}

void addDisplayGames(State& state, uint32_t games) {
    state.display_games += games;
}

} // namespace slotv2::normal_state
