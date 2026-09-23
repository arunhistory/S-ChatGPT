#pragma once
#include <stdint.h>

namespace slotv2::normal_state {

struct State {
    uint32_t actual_games{0};
    uint32_t display_games{0};
};

void reset(State& state);
void onLever(State& state);
void addDisplayGames(State& state, uint32_t games);

} // namespace slotv2::normal_state
