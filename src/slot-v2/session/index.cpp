#include "index.hpp"

namespace slotv2::session {

void reset(State& state) {
    state = {};
    state.phase = Phase::Idle;
}

bool canLever(const State& state) {
    return state.phase == Phase::Idle || state.phase == Phase::Complete;
}

bool begin(
    State& state,
    LeverResult lever,
    special_result::Result special,
    freeze::Directive freeze
) {
    if (!canLever(state)) return false;

    state.lever = lever;
    state.special = special;
    state.freeze = freeze;
    state.stop_count = 0;
    for (int i = 0; i < 3; ++i) {
        state.stopped[i] = false;
        state.position[i] = 0;
    }

    state.phase = lever.special == SpecialHit::None
        ? Phase::Stopping
        : Phase::SpecialPending;
    return true;
}

bool canStop(const State& state, ReelId reel) {
    const auto i = static_cast<uint8_t>(reel);
    if (i > 2u) return false;
    if (state.phase != Phase::Stopping) return false;
    return !state.stopped[i];
}

stop_shared::Context makeStopContext(
    const State& state,
    ReelId reel,
    uint8_t pressed_position
) {
    stop_shared::Context ctx{};
    ctx.role = state.lever.role;
    ctx.reel = reel;
    ctx.pressed_position = static_cast<uint8_t>(pressed_position % kReelSize);
    ctx.stop_order = state.stop_count;

    for (int i = 0; i < 3; ++i) {
        ctx.stopped[i] = state.stopped[i];
        ctx.stopped_position[i] = state.position[i];
    }
    return ctx;
}

void acceptStop(State& state, ReelId reel, const stop_shared::Result& result) {
    if (!canStop(state, reel)) return;

    if (result.status != stop_shared::ResolveStatus::Ok
        && result.status != stop_shared::ResolveStatus::RoleMissed) {
        return;
    }

    const auto i = static_cast<uint8_t>(reel);
    state.stopped[i] = true;
    state.position[i] = result.final_position;
    if (state.stop_count < 3u) ++state.stop_count;

    if (state.stop_count == 3u) {
        state.phase = Phase::Complete;
    }
}

void completeSpecial(State& state) {
    if (state.phase == Phase::SpecialPending) {
        state.phase = Phase::Complete;
    }
}

} // namespace slotv2::session
