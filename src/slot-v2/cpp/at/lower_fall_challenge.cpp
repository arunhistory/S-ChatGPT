#include "at/lower_fall_challenge.hpp"

namespace slotv2::lower_fall_challenge {

void clear(State& state) {
    state = {};
}

void arm(
    Rng& rng,
    State& state,
    int32_t saved_games
) {
    state.phase = Phase::Armed;
    state.success_fixed = rng.uniformBelow(2u) == 0u;
    state.saved_games = saved_games > 0 ? saved_games : 0;
}

bool beginWaiting(State& state) {
    if (state.phase != Phase::Armed) return false;
    state.phase = Phase::Waiting;
    return true;
}

bool blocksATFlow(const State& state) {
    return state.phase != Phase::Inactive;
}

bool buttonReady(const State& state) {
    return state.phase == Phase::ButtonReady;
}

WaitGame beginWaitGame(Rng& rng, const State& state) {
    if (state.phase != Phase::Waiting) {
        return {};
    }

    return {
        true,
        rng.uniformBelow(2u) == 0u
    };
}

bool finalizeWaitGame(
    State& state,
    const WaitGame& game
) {
    if (!game.active
        || state.phase != Phase::Waiting) {
        return false;
    }

    if (!game.judge_bell) {
        return false;
    }

    state.phase = Phase::ButtonReady;
    return true;
}

PushOutcome push(
    State& state,
    at_state::State& at
) {
    if (state.phase != Phase::ButtonReady) {
        return PushOutcome::NotReady;
    }

    const bool success = state.success_fixed;
    const int32_t saved_games = state.saved_games;
    clear(state);

    if (success) {
        at.games_left = saved_games;
        return PushOutcome::Continued;
    }

    at.games_left = 0;
    return PushOutcome::Failed;
}

} // namespace slotv2::lower_fall_challenge
