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
    state.wait_games_left = kWaitGames;
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

WaitGame beginWaitGame(const State& state) {
    if (state.phase != Phase::Waiting
        || state.wait_games_left == 0u) {
        return {};
    }

    return {
        true,
        state.wait_games_left
    };
}

bool finalizeWaitGame(
    State& state,
    const WaitGame& game
) {
    if (!game.active
        || state.phase != Phase::Waiting
        || state.wait_games_left == 0u) {
        return false;
    }

    --state.wait_games_left;
    if (state.wait_games_left == 0u) {
        state.phase = Phase::ButtonReady;
        return true;
    }

    return false;
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
