#pragma once
#include <stdint.h>
#include "shared/rng.hpp"
#include "at/at_state.hpp"

namespace slotv2::lower_fall_challenge {

enum class Phase : uint8_t {
    Inactive = 0,
    Armed = 1,
    Waiting = 2,
    ButtonReady = 3
};

enum class PushOutcome : uint8_t {
    NotReady = 0,
    Continued = 1,
    Failed = 2
};

struct State {
    Phase phase{Phase::Inactive};
    bool success_fixed{false};
    int32_t saved_games{0};
};

struct WaitGame {
    bool active{false};
    bool judge_bell{false};
};

void clear(State& state);

// Called on the Fall game's lever-on.
// Result is fixed here: 50% success / 50% failure.
// saved_games is the AT games-left value from before the Fall game was consumed.
void arm(
    Rng& rng,
    State& state,
    int32_t saved_games
);

// Called when the Fall event is resolved after the Fall game.
// Starts the variable wait without changing saved_games.
bool beginWaiting(State& state);

bool blocksATFlow(const State& state);
bool buttonReady(const State& state);

// Each waiting lever-on independently creates the judge-start bell at 1/2.
// No AT-internal lottery or AT remaining-game decrement occurs here.
WaitGame beginWaitGame(Rng& rng, const State& state);

// Called after all three reels stop for a wait game.
// Only a judge-bell game moves the challenge to the one-shot PUSH.
bool finalizeWaitGame(
    State& state,
    const WaitGame& game
);

// One-shot decision display only. No lottery occurs here.
// Success restores the exact pre-Fall saved_games value.
PushOutcome push(
    State& state,
    at_state::State& at
);

} // namespace slotv2::lower_fall_challenge
