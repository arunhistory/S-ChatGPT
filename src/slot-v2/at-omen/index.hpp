#pragma once
#include <stdint.h>
#include "../machine-state/index.hpp"

namespace slotv2::at_omen {

struct State {
    bool active{false};
    bool episode{false};
    uint8_t games_left{0};
};

struct Game {
    bool active{false};
    bool episode{false};
    uint8_t omen_before{0};
    uint8_t omen_after{0};
    bool ended{false};
    bool at_window_empty{false};
};

enum class FinalizeOutcome : uint8_t {
    None = 0,
    BonusEntryQueued = 1,
    EpisodeEntryQueued = 2
};

void start(State& state, bool episode);
void clear(State& state);

Game beginGame(
    at_state::State& at,
    State& omen
);

FinalizeOutcome finalize(
    machine_state::State& machine,
    State& omen,
    const Game& game
);

} // namespace slotv2::at_omen
