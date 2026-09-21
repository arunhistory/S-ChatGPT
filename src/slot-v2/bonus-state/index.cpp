#include "index.hpp"

namespace slotv2::bonus_state {

void start(State& state, Kind kind) {
    state.active = true;
    state.kind = kind;
    state.medals_left = kind == Kind::Episode
        ? kEpisodeTargetMedals
        : kRegularTargetMedals;
}

int applyNetGain(State& state, int medals) {
    if (!state.active || medals <= 0) return state.medals_left;

    state.medals_left -= medals;
    if (state.medals_left <= 0) {
        state.medals_left = 0;
        state.active = false;
    }
    return state.medals_left;
}

bool complete(const State& state) {
    return !state.active && state.medals_left == 0;
}

} // namespace slotv2::bonus_state
