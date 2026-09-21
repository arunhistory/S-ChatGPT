#pragma once
#include <stdint.h>

namespace slotv2::cz_state {

static constexpr uint8_t kGames = 10;

struct State {
    bool active{false};
    uint8_t games_left{0};
};

void start(State& state);
bool consumeGame(State& state);
void resolve(State& state);

} // namespace slotv2::cz_state
