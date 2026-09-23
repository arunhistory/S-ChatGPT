#include "index.hpp"

namespace slotv2::at_state {

void start(State& state, Tier tier) {
    state.active = true;
    state.tier = tier;
    state.table = Table::Normal;
    state.games_left = kInitialSTGames;
    state.cold = false;
}

void end(State& state) {
    state.active = false;
    state.games_left = 0;
    state.cold = false;
}

void addGames(State& state, int games) {
    if (games > 0) state.games_left += games;
}

bool consumeGame(State& state) {
    if (!state.active || state.games_left <= 0) return false;
    --state.games_left;
    return state.games_left > 0;
}

void setTier(State& state, Tier tier) {
    state.tier = tier;
}

int netPerGame(const State& state) {
    switch (state.tier) {
        case Tier::Lower: return kLowerNetPerGame;
        case Tier::Middle: return kMiddleNetPerGame;
        case Tier::Upper: return kUpperNetPerGame;
    }
    return 0;
}

} // namespace slotv2::at_state
